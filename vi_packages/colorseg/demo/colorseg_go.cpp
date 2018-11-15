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

#include <colorseg/colorspace_homography.hpp>
#include <colorseg/color_vertex.h>
#include <colorseg/color_weight_func.h>
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

double KL(Eigen::Vector3d const & mean1, Eigen::Vector3d const & mean2,
          Eigen::Matrix3d const & cov1, Eigen::Matrix3d const & cov2)
{
  auto s1 = (cov2.inverse() * cov1).trace();
  auto s2 = (mean2 - mean1).transpose() * cov2.inverse() * (mean2 - mean1);
  auto s3 = std::log(cov2.determinant() / cov1.determinant());
  return (s1 + s2 + s3 - 3)/ 2;
}

void obtainLockList(std::set<std::pair<int, int> > & lockList,
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
      lockList.insert(stat.second.leftTopPoint);
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

    if (homographyInv(hs.mean).mean() < threshold)
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
  TCLAP::ValueArg<int> bilateralD("", "bl_d", "bilateral pixel diameter", false, 15, "int", cmd);
  TCLAP::ValueArg<double> bilateralSigmaColor("", "bl_sigma_color", "bilateral sigma color", false, 50, "double", cmd);
  TCLAP::ValueArg<double> bilateralSigmaSpace("", "bl_sigma_space", "bilateral sigma space", false, 50, "double", cmd);
  TCLAP::ValueArg<double> errorLimit("e", "error_limit", "average error limit", false, -1, "double", cmd);
  TCLAP::ValueArg<int> segmentsLimit("n", "segm_limit", "segments limit", false, -1, "int", cmd);
  TCLAP::ValueArg<double> lockThreshold("g", "lock_thresh", "segments locking threshold value", false, 1, "double", cmd);
  TCLAP::ValueArg<double> LTDistance("", "model_distance", "distance betwwen L- or T-shaped clusters", false, 20, "double", cmd);
  TCLAP::ValueArg<double> offscaleThreshold("", "offscale_thresh", "offscale threshold", false, 230, "double", cmd);
  TCLAP::UnlabeledValueArg<std::string> imagePath("image", "path to source RGB-image in tif-convertible format", true, "", "string", cmd);
  TCLAP::ValueArg<std::string> output("o", "output", "path to output dir", false, ".", "string", cmd);
  TCLAP::SwitchArg debug("d", "debug", "debug mode", cmd, false);
  TCLAP::ValueArg<int> debugIter("i", "debug_iter", "debug iterations", false, 1, "int", cmd);
  TCLAP::ValueArg<int> maxSegments("s", "max_segments", "max segments for debug output", false, -1, "int", cmd);

  cmd.parse(argc, argv);

  ColorVertex::setLTDistance(LTDistance.getValue());

  if (!bfs::is_directory(output.getValue()))
    throw std::runtime_error("Failed to find output directory " + output.getValue());

  if (debug.getValue())
     i8r::configure(output.getValue());

  std::string const basename = bfs::path(imagePath.getValue()).stem().string();
  std::string const imgres_filename = bfs::absolute(basename + ".png", output.getValue()).string();
  std::string const filtered_filename = bfs::absolute(basename + ".filtered.png", output.getValue()).string();

  try
  {
    cv::Mat cv_image = cv::imread(imagePath.getValue().c_str());
    if (cv_image.channels() != 3)
      throw std::runtime_error("Image should have exact 3 channels for color segmentation");

    cv::Mat cv_image_filtered;
    cv::bilateralFilter(cv_image, cv_image, bilateralD.getValue(),
                        bilateralSigmaColor.getValue(), bilateralSigmaSpace.getValue());
    cv::imwrite(filtered_filename, cv_image_filtered);

    cv_image_filtered.convertTo(cv_image_filtered, CV_32F);
    mximg::PImage image = mximg::createByCopy(cv_image_filtered);

    auto dbg = i8r::logger("debug." + basename + ".pointlike");
    Segmentator<ColorVertex> segmentatorPointlike(*image, pointlike_error, pointlike_SD, true);
    segmentatorPointlike.mergeToLimit(-1, errorLimit.getValue(), segmentsLimit.getValue(),
                                      dbg, debugIter.getValue(), maxSegments.getValue());

    std::set<std::pair<int, int> > lockList;
    obtainLockList(lockList, segmentatorPointlike, lockThreshold.getValue());

    Segmentator<ColorVertex> segmentatorLinear(*image, &segmentatorPointlike.getImageMap(),
                                               linear_error, linear_SD,
                                               lockList, LOCK_SEGMENTS, true);
    segmentatorLinear.mergeToLimit(-1, errorLimit.getValue() * std::sqrt(2./3), segmentsLimit.getValue(),
                                   dbg, debugIter.getValue(), maxSegments.getValue());

    Segmentator<ColorVertex> segmentatorPlanar(*image, &segmentatorLinear.getImageMap(),
                                                planar_error, planar_SD,
                                                {}, LOCK_SEGMENTS, true);
    segmentatorPlanar.mergeToLimit(-1, errorLimit.getValue() * std::sqrt(1./3), segmentsLimit.getValue(),
                                   dbg, debugIter.getValue(), maxSegments.getValue());

    offscaleFix(segmentatorPlanar, offscaleThreshold.getValue());
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
