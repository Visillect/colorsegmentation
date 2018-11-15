#include <iostream>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <memory>

#include <minimgapi/minimgapi.h>
#include <minimgapi/imgguard.hpp>
#include <minbase/minresult.h>
#include <minimgio/minimgio.h>
#include <mximg/image.h>
#include <mximg/ocv.h>
#include <vi_cvt/std/exception_macros.hpp>
#include <vi_cvt/ocv/image.hpp>

#include <remseg/segmentator.hpp>
#include <remseg/weight_func.h>

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

THIRDPARTY_INCLUDES_BEGIN
#include <tclap/CmdLine.h>
THIRDPARTY_INCLUDES_END

using namespace vi::remseg;

int main(int argc, const char *argv[])
{
  TCLAP::CmdLine cmd("Run Region Merge Segmentation on a Single image");
  TCLAP::ValueArg<double> weightLimit("w", "weight_limit", "weight limit", false, -1, "double", cmd);
  TCLAP::ValueArg<int> segmentsLimit("n", "segm_limit", "segments limit", false, 10, "int", cmd);
  TCLAP::ValueArg<std::string> imageMapPath("m", "map", "path to file with image map source in tif-convertible format", false, "", "string", cmd);
  TCLAP::UnlabeledValueArg<std::string> imagePath("image", "path to source RGB-image in tif-convertible format", true, "", "string", cmd);

  cmd.parse(argc, argv);

  try
  {
    mximg::PImage image = mximg::Image::imread(imagePath.getValue().c_str());
    cv::Mat cv_image = vi::cvt::ocv::as_cvmat(*image);
    cv_image.convertTo(cv_image, CV_32F);
    image = mximg::createByCopy(cv_image);

    std::unique_ptr<Segmentator<Vertex> > segmentator;

    if (!imageMapPath.getValue().empty())
    {
      ImageMap imageMap(imageMapPath.getValue().c_str());
      segmentator.reset(new Segmentator<Vertex>(*image, &imageMap, student_error, student_weight, {}, LOCK_SEGMENTS, false));
    }
    else
      segmentator.reset(new Segmentator<Vertex>(*image, student_error, student_weight));

    segmentator->mergeToLimit(weightLimit.getValue(), -1, segmentsLimit.getValue());
    const ImageMap &imageMap = segmentator->getImageMap();

    DECLARE_GUARDED_MINIMG(out);
    visualize(&out, imageMap);
    THROW_ON_MINERR(SaveMinImage("segmentation_go.tif", &out));
  }
    catch (std::exception const& e)
  {
    std::cerr << "Unhandled exception: " << e.what() << "\n";
    return 1;
  }
  catch (...)
  {
    std::cerr << "Unhandled UNTYPED exception\n";
    return 2;
  }

  return 0;
}
