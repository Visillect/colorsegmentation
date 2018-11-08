#include <algorithm>
#include <iostream>
#include <memory>
#include <minbase/crossplat.h>
#include <minimgapi/minimgapi-helpers.hpp>
#include <minimgapi/imgguard.hpp>
#include <minimgio/minimgio.h>
#include <vi_cvt/std/exception_macros.hpp>
#include <vi_cvt/ocv/image.hpp>

#include <colorseg/color_distance_func.h>
#include <colorseg/colorspace_transform.h>
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

EdgeValue dummy(ColorVertex const * v1, ColorVertex const * v2)
{
  return 0;
}

int main(int argc, const char *argv[])
{
  i8r::AutoShutdown i8r_shutdown;

  TCLAP::CmdLine cmd("Run Range-Based Region Merge Segmentation on a Single image");
  TCLAP::ValueArg<std::string> distanceFunc("f", "dist_func", "distance function, one of the following: [rank0, rank1, rank2]", false, "rank0", "string", cmd);
  TCLAP::ValueArg<double> distanceLimit("r", "dist_limit", "distance limit", false, -1, "double", cmd);
  TCLAP::ValueArg<double> errorLimit("e", "error_limit", "average error limit", false, -1, "int", cmd);
  TCLAP::ValueArg<int> segmentsLimit("n", "segm_limit", "segments limit", false, -1, "int", cmd);
  TCLAP::ValueArg<std::string> blockListPath("b", "block", "path to json-file with edges info", false, "", "string", cmd);
  TCLAP::ValueArg<std::string> imageMapPath("m", "map", "path to file with image map source in tif-convertible format", false, "", "string", cmd);
  TCLAP::UnlabeledValueArg<std::string> imagePath("image", "path to source RGB-image in tif-convertible format", true, "", "string", cmd);
  TCLAP::ValueArg<std::string> output("o", "output", "path to output dir", false, ".", "string", cmd);
  TCLAP::SwitchArg debug("d", "debug", "debug mode", cmd, false);
  TCLAP::ValueArg<int> debugIter("i", "debug_iter", "debug iterations", false, 1, "int", cmd);
  TCLAP::ValueArg<int> maxSegments("s", "max_segments", "max segments for debug output", false, -1, "int", cmd);
  TCLAP::ValueArg<double> maxModelDistance("", "model_distance", "model distance", false, 20, "double", cmd);
  TCLAP::ValueArg<double> glareThresh("", "glare_thresh", "glare threshold", false, 230, "double", cmd);
  TCLAP::SwitchArg log("l", "log", "log mode", cmd, false);
  TCLAP::SwitchArg prefilter("p", "prefilter", "use image pre-filtering", cmd, false);

	cmd.parse(argc, argv);

  ColorVertex::setMaxModelDistance(maxModelDistance.getValue());
  ColorVertex::setGlareThresh(glareThresh.getValue());

  if (!bfs::is_directory(output.getValue()))
    throw std::runtime_error("Failed to find output directory " + output.getValue());

  if (debug.getValue())
     i8r::configure(output.getValue());

  EdgeValue (*error_func)(const ColorVertex *v);
  EdgeValue (*dist_func)(const ColorVertex *v1, const ColorVertex *v2);
  if      (distanceFunc.getValue() == "rank0")      error_func = shouldnotcall, dist_func = criteria_r0;
  else if (distanceFunc.getValue() == "rank0_old")  error_func = shouldnotcall, dist_func = criteria_r0_old;
  else if (distanceFunc.getValue() == "rank1")      error_func = error_r1, dist_func = criteria_r1;
  else if (distanceFunc.getValue() == "rank1_old")  error_func = shouldnotcall, dist_func = criteria_r1_old;
  else if (distanceFunc.getValue() == "rank2_new")  error_func = error_r2, dist_func = criteria_r2_new;
  else if (distanceFunc.getValue() == "rank2_supernew")  error_func = error_r2, dist_func = criteria_r2_supernew;
  else if (distanceFunc.getValue() == "offscale_fix")  error_func = error_r2, dist_func = dummy;
  else if (distanceFunc.getValue() == "rank2")      error_func = error_r2, dist_func = criteria_r2;
  else if (distanceFunc.getValue() == "rank2_old")  error_func = shouldnotcall, dist_func = criteria_r2_old;
  else throw std::runtime_error("Wrong distance function name - " + distanceFunc.getValue());

  bool norm = distanceFunc.getValue() == "rank0" ||
              distanceFunc.getValue() == "rank1" ||
              distanceFunc.getValue() == "rank2" ||
              distanceFunc.getValue() == "rank2_new" ||
              distanceFunc.getValue() == "rank2_supernew" ;

  std::string const basename = bfs::path(imagePath.getValue()).stem().string();
  std::string const imgres_filename = bfs::absolute(basename + "." + distanceFunc.getValue() + ".png", output.getValue()).string();
  std::string const logres_filename = bfs::absolute(basename + "." + distanceFunc.getValue() + ".log.json", output.getValue()).string();
  std::string const filtered_filename = bfs::absolute(basename + ".filtered.png", output.getValue()).string();

  try
  {
    std::unique_ptr<Image> image(new Image(imagePath.getValue().c_str(), false));

    if (image->getChannelsNum() != 3)
      throw std::runtime_error("Image should have exact 3 channels");

    cv::Mat cv_image_filtered;
    if (prefilter.getValue())
    {
      cv::Mat cv_image = vi::cvt::ocv::as_cvmat(image->getMinImg());
      // cv::cvtColor(cv_image, cv_image, CV_BGR2RGB);
      // cv::GaussianBlur(cv_image, cv_image_filtered, cv::Size( 3, 3 ), 0, 0 );
      cv::bilateralFilter(cv_image, cv_image_filtered, BILATERAL_D, BILATERAL_SIGMA_COLOR, BILATERAL_SIGMA_SPACE);
      cv::imwrite(filtered_filename, cv_image_filtered);
      MinImg min_image_filtered = vi::cvt::ocv::as_minimg(cv_image_filtered);
      image.reset(new Image(&min_image_filtered, false));
    }

    bool blocking_policy = BLOCK_SEGMENTS;
    std::set<std::pair<int, int> > blockList;
    if (!blockListPath.getValue().empty())
      readBlockList(blockList, blocking_policy, blockListPath.getValue());

    std::unique_ptr<Segmentator<ColorVertex> > segmentator;
    if (!imageMapPath.getValue().empty())
    {
      ImageMap imageMap(imageMapPath.getValue().c_str());
      segmentator.reset(new Segmentator<ColorVertex>(*image, imageMap, error_func, dist_func,
                                                     blockList, blocking_policy, norm));
    }
    else
      segmentator.reset(new Segmentator<ColorVertex>(*image, error_func, dist_func, norm));

    auto dbg = i8r::logger("debug." + basename + "." + distanceFunc.getValue());
    const double GLARE_THRES = ColorVertex::getGlareThresh();

    if (distanceFunc.getValue() != "offscale_fix")
      segmentator->mergeToLimit(distanceLimit.getValue(), errorLimit.getValue(), segmentsLimit.getValue(),
                                dbg, debugIter.getValue(), maxSegments.getValue());
    else
    {
      const ImageMap &imageMap = segmentator->getImageMap();
      auto const stats = imageMap.getSegmentStats();
      std::set<SegmentID> merged;

      for (auto const & stat : stats)
      {
        if (merged.find(stat.first) != merged.end())
          continue;

        ColorVertex * v = segmentator->vertexById(stat.first);
        ColorVertex::HelperStats const & hs = v->getHelperStats();

        if (Konovalenko2RGB(hs.mean).mean() < GLARE_THRES)
          continue;

        if (v->size() == 1)
        {
          // std::cout << "spherical" << std::endl;
          merged.insert(segmentator->getId(dynamic_cast<ColorVertex*>(v->begin()->vertex)));
          segmentator->merge(v, dynamic_cast<ColorVertex*>(v->begin()->vertex));
        }
        else
        {
          // std::cout << "cylindrical" << std::endl;
          int i = 0, j = 0;

          std::vector<int> ids;
          std::set<int> merged_ids;

          for (ConstJoint it = v->begin(); it != v->end(); it++)
            ids.push_back(segmentator->getId(dynamic_cast<ColorVertex*>(it->vertex)));

          for (auto it1 = ids.begin(); it1 != ids.end(); it1++)
          {
            if (merged_ids.find(*it1) != merged_ids.end())
              continue;

            for (auto it2 = v->begin(); it2 != v->end(); it2++)
            {
              ColorVertex * v1 = segmentator->vertexById(*it1);
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

              merged.insert(segmentator->getId(v1));
              merged.insert(segmentator->getId(v2));

              segmentator->merge(v, v1);
              segmentator->merge(v, v2);

              merged_ids.insert(*it1);
              merged_ids.insert(segmentator->getId(v2));
              break;
            }
          }
        }
        segmentator->updateMapping();
      }
    }
    const ImageMap &imageMap = segmentator->getImageMap();

    DECLARE_GUARDED_MINIMG(imgres);
    visualize(&imgres, imageMap);
    THROW_ON_MINERR(SaveMinImage(imgres_filename.c_str(), &imgres));

    if (log.getValue())
      segmentator->saveLog(logres_filename);
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
