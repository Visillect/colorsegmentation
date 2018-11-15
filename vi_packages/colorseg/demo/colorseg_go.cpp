#include <algorithm>
#include <cmath>
#include <iostream>

#include <minbase/crossplat.h>
#include <minimgapi/minimgapi-helpers.hpp>
#include <minimgapi/imgguard.hpp>
#include <minimgio/minimgio.h>
#include <mximg/image.h>
#include <mximg/ocv.h>
#include <vi_cvt/std/exception_macros.hpp>
#include <vi_cvt/ocv/image.hpp>

#include <colorseg/color_distance_func.h>
#include <colorseg/colorspace_homography.hpp>
#include <colorseg/color_vertex.h>
#include <remseg/segmentator.hpp>
#include <remseg/utils.h>

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

THIRDPARTY_INCLUDES_BEGIN
#include <boost/filesystem.hpp>
#include <tclap/CmdLine.h>
THIRDPARTY_INCLUDES_END

namespace bfs = boost::filesystem;

using namespace vi::remseg;
using namespace vi::colorseg;

const int    BILATERAL_D = 15;
const double BILATERAL_SIGMA_COLOR = 50;
const double BILATERAL_SIGMA_SPACE = 50;

double KL(Eigen::Vector3d const & mean1, Eigen::Vector3d const & mean2,
          Eigen::Matrix3d const & cov1, Eigen::Matrix3d const & cov2)
{
  auto s1 = (cov2.inverse() * cov1).trace();
  auto s2 = (mean2 - mean1).transpose() * cov2.inverse() * (mean2 - mean1);
  auto s3 = std::log(cov2.determinant() / cov1.determinant());
  return (s1 + s2 + s3 - 3)/ 2;
}

void obtainBlockList(std::set<std::pair<int, int> > & blockList,
                      Segmentator<ColorVertex> const & segmentator,
                      double threshold)
{
  const ImageMap &imageMap = segmentator.getImageMap();
  auto stats = imageMap.getSegmentStats();
  for (auto const & stat : stats)
  {
    SegmentID id = stat.first;
    ColorVertex * v = segmentator.vertexById(id);
    ColorVertex::HelperStats const & hs = v->getHelperStats();
    std::vector<EdgeValue> dists;
    for (auto const& n : stat.second.neighbours)
    {
      ColorVertex * v_n = segmentator.vertexById(n);
      ColorVertex::HelperStats const & hs_n = v_n->getHelperStats();
      dists.push_back(KL(hs.mean, hs_n.mean, hs.covariance, hs_n.covariance));
    }

    if (!dists.empty() && *std::min_element(dists.begin(), dists.end()) > threshold)
      blockList.insert(stat.second.leftTopPoint);
  }
}

void offscaleFix(Segmentator<ColorVertex> & segmentator, double threshold)
{
  const ImageMap &imageMap = segmentator.getImageMap();
  auto const stats = imageMap.getSegmentStats();
  std::set<SegmentID> merged;

  for (auto const & stat : stats)
  {
    if (merged.find(stat.first) != merged.end())
      continue;

    ColorVertex * v = segmentator.vertexById(stat.first);
    ColorVertex::HelperStats const & hs = v->getHelperStats();

    if (homographyInv(hs.mean, ColorVertex::getHomographyA(),
                      ColorVertex::getHomographyK()).mean() < threshold)
      continue;

    if (v->size() == 1)
    {
      merged.insert(segmentator.getId(v->begin()->vertex));
      segmentator.merge(v, dynamic_cast<ColorVertex*>(v->begin()->vertex));
    }
    else
    {
      int i = 0, j = 0;

      std::vector<int> ids;
      std::set<int> merged_ids;

      for (ConstJoint it = v->begin(); it != v->end(); it++)
        ids.push_back(segmentator.getId(it->vertex));

      for (auto it1 = ids.begin(); it1 != ids.end(); it1++)
      {
        if (merged_ids.find(*it1) != merged_ids.end())
          continue;

        for (auto it2 = v->begin(); it2 != v->end(); it2++)
        {
          ColorVertex * v1 = segmentator.vertexById(*it1);
          ColorVertex * v2 = dynamic_cast<ColorVertex*>(it2->vertex);

          if (v1 == v2)
            continue;

          Edge * edge = 0;
          for (auto it3 = v2->begin(); it3 != v2->end(); it3++)
            if (dynamic_cast<ColorVertex*>(it3->vertex) == v1)
              edge = it3->edge;

            if (edge == 0)
              continue;

            if (!isLTCluster(v1, v2))
              continue;

          merged.insert(segmentator.getId(v1));
          merged.insert(segmentator.getId(v2));

          segmentator.merge(v, v1);
          segmentator.merge(v, v2);

          merged_ids.insert(*it1);
          merged_ids.insert(segmentator.getId(v2));
          break;
        }
      }
    }
    segmentator.updateMapping();
  }
}

int main(int argc, const char *argv[])
{
  i8r::AutoShutdown i8r_shutdown;

  TCLAP::CmdLine cmd("Run Range-Based Region Merge Segmentation on a Single image");
  TCLAP::ValueArg<double> errorLimit("e", "error_limit", "average error limit", false, -1, "double", cmd);
  TCLAP::ValueArg<int> segmentsLimit("n", "segm_limit", "segments limit", false, -1, "int", cmd);
  TCLAP::UnlabeledValueArg<std::string> imagePath("image", "path to source RGB-image in tif-convertible format", true, "", "string", cmd);
  TCLAP::ValueArg<std::string> output("o", "output", "path to output dir", false, ".", "string", cmd);
  TCLAP::SwitchArg debug("d", "debug", "debug mode", cmd, false);
  TCLAP::ValueArg<int> debugIter("i", "debug_iter", "debug iterations", false, 1, "int", cmd);
  TCLAP::ValueArg<int> maxSegments("s", "max_segments", "max segments for debug output", false, -1, "int", cmd);
  TCLAP::ValueArg<double> blockingThresh("g", "blocking_thresh", "blocking threshold value", false, 1, "double", cmd);
  TCLAP::ValueArg<double> maxModelDistance("", "model_distance", "model distance", false, 20, "double", cmd);
  TCLAP::ValueArg<double> glareThresh("", "glare_thresh", "glare threshold", false, 230, "double", cmd);
  TCLAP::SwitchArg prefilter("p", "prefilter", "use image pre-filtering", cmd, false);

  cmd.parse(argc, argv);

  ColorVertex::setMaxModelDistance(maxModelDistance.getValue());

  if (!bfs::is_directory(output.getValue()))
    throw std::runtime_error("Failed to find output directory " + output.getValue());

  if (debug.getValue())
     i8r::configure(output.getValue());

  std::string const basename = bfs::path(imagePath.getValue()).stem().string();
  std::string const imgres_filename = bfs::absolute(basename + ".png", output.getValue()).string();
  std::string const filtered_filename = bfs::absolute(basename + ".filtered.png", output.getValue()).string();

  try
  {
    mximg::PImage image = mximg::Image::imread(imagePath.getValue().c_str());
    if ((*image)->channels != 3)
      throw std::runtime_error("Image should have exact 3 channels for color segmentation");

    cv::Mat cv_image_filtered;
    if (prefilter.getValue())
    {
      cv::Mat cv_image = vi::cvt::ocv::as_cvmat(*image);
      cv_image.convertTo(cv_image, CV_32F);
      cv::bilateralFilter(cv_image, cv_image_filtered, BILATERAL_D, BILATERAL_SIGMA_COLOR, BILATERAL_SIGMA_SPACE);
      cv::imwrite(filtered_filename, cv_image_filtered);
      image = mximg::createByCopy(cv_image_filtered);
    }

    auto dbg = i8r::logger("debug." + basename + ".pointlike");
    Segmentator<ColorVertex> segmentatorPointlike(*image, shouldnotcall, criteria_r0, true);
    segmentatorPointlike.mergeToLimit(-1, errorLimit.getValue(), segmentsLimit.getValue(),
                                      dbg, debugIter.getValue(), maxSegments.getValue());

    std::set<std::pair<int, int> > blockList;
    obtainBlockList(blockList, segmentatorPointlike, blockingThresh.getValue());

    Segmentator<ColorVertex> segmentatorLinear(*image, &segmentatorPointlike.getImageMap(),
                                               error_r1, criteria_r1,
                                               blockList, BLOCK_SEGMENTS, true);
    segmentatorLinear.mergeToLimit(-1, errorLimit.getValue() * std::sqrt(2./3), segmentsLimit.getValue(),
                                   dbg, debugIter.getValue(), maxSegments.getValue());

    Segmentator<ColorVertex> segmentatorPlanar(*image, &segmentatorLinear.getImageMap(),
                                                error_r2, criteria_r2,
                                                {}, BLOCK_SEGMENTS, true);
    segmentatorPlanar.mergeToLimit(-1, errorLimit.getValue() * std::sqrt(1./3), segmentsLimit.getValue(),
                                   dbg, debugIter.getValue(), maxSegments.getValue());

    offscaleFix(segmentatorPlanar, glareThresh.getValue());
    const ImageMap &imageMap = segmentatorPlanar.getImageMap();

    DECLARE_GUARDED_MINIMG(imgres);
    visualize(&imgres, imageMap);
    THROW_ON_MINERR(SaveMinImage(imgres_filename.c_str(), &imgres));
  }
  catch (std::exception const& e)
  {
    std::cerr << "Exception caught: " << e.what() << "\n";
    return 1;
  }
  catch (...)
  {
    std::cerr << "UNTYPED exception\n";
    return 2;
  }

  return 0;
}
