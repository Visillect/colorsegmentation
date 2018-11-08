#include <iostream>
#include <cstdlib>
#include <vector>
#include <memory>
#include <minimgapi/minimgapi.h>
#include <minimgapi/imgguard.hpp>
#include <minbase/minresult.h>
#include <minimgio/minimgio.h>
#include <remseg/segmentator.hpp>
#include <cstring>

THIRDPARTY_INCLUDES_BEGIN
#include <tclap/CmdLine.h>
THIRDPARTY_INCLUDES_END

using namespace vi::remseg;

int main(int argc, const char *argv[])
{
  TCLAP::CmdLine cmd("Run Region Merge Segmentation on a Single image");
  TCLAP::ValueArg<double> distanceLimit("r", "dist_limit", "distance limit", false, -1, "double", cmd);
  TCLAP::ValueArg<int> segmentsLimit("n", "segm_limit", "segments limit", false, 10, "int", cmd);
  TCLAP::ValueArg<std::string> imageMapPath("m", "map", "path to file with image map source in tif-convertible format", false, "", "string", cmd);
  TCLAP::UnlabeledValueArg<std::string> imagePath("image", "path to source RGB-image in tif-convertible format", true, "", "string", cmd);

  cmd.parse(argc, argv);

  try
  {
    Image image(imagePath.getValue().c_str(), true);
    std::unique_ptr<Segmentator<Vertex> > segmentator;

    if (!imageMapPath.getValue().empty())
    {
      ImageMap imageMap(imageMapPath.getValue().c_str());
      segmentator.reset(new Segmentator<Vertex>(image, imageMap));
    }
    else
      segmentator.reset(new Segmentator<Vertex>(image));

    segmentator->mergeToLimit(distanceLimit.getValue(), -1, segmentsLimit.getValue());
    const ImageMap &imageMap = segmentator->getImageMap();

    DECLARE_GUARDED_MINIMG(out);
    PROPAGATE_ERROR(NewMinImagePrototype(
      &out, image.getWidth(), image.getHeight(), 3, TYP_UINT8));

    auto colors = imageMap.getColorMap();
    for (int y = 0; y < image.getHeight(); y++)
    {
      RGB* line = reinterpret_cast<RGB*>(GetMinImageLine(&out, y));
      for (int x = 0; x < image.getWidth(); x++)
      {
        const int idx = imageMap(x, y);
        memcpy(line+x, &colors[idx], sizeof(RGB));
      }
    }

    PROPAGATE_ERROR(SaveMinImage("segmentation_go.tif", &out));
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
