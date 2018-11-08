#include <i8r/i8r.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <json-cpp/value.h>
#include <tclap/CmdLine.h>


namespace i8r_demo {

static cv::Mat generate_img(double step_adjustment)
{
  int const rows = 600;
  int const cols = 1000;
  cv::Mat img(rows, cols, CV_8UC1, cv::Scalar(255));

  int const step = std::max(1, static_cast<int>(step_adjustment * cols / 50));

  for (int x = 3 * step; x < img.cols / 2 - 2 * step; x += step)
  {
    cv::Point pt1(x, 0);
    cv::Point pt2(x, img.rows);
    cv::line(img, pt1, pt2, cv::Scalar(0), 4, CV_AA);
  }


  int radius = step / 4;
  for (int y = 3 * step; y < img.rows - 2 * step; y += step)
  {
    for (int x = 3 * step + img.cols / 2; x < img.cols - 2 * step; x += step)
    {
      cv::circle(img, cv::Point(x,y), radius, cv::Scalar(0), -1);
    }
  }

  return img;
}

static cv::Mat draw_some_lines(int count)
{
  cv::Mat img(480, 640, CV_8UC3, cv::Scalar(255, 255, 255));

  for (int i = 0; i <= count; ++i)
  {
    cv::line(img, {2 * i, 30 * i}, {img.cols - i, img.rows / 2},
             cv::Scalar(128, 128, 128));
  }

  return img;
}

static void main(int argc, char ** argv)
{
  i8r::AutoShutdown i8r_shutdown;

  TCLAP::CmdLine cmd("i8r ('imagesaver') demo program");

  TCLAP::ValueArg<std::string> debug_root("d", "dbg-root", "i8r root dir", false, "", "dir", cmd);
  TCLAP::ValueArg<std::string> debug_config("D", "dbg-cfg", "i8r config file", false, "", "filename.json", cmd);

  cmd.parse(argc, argv);

  i8r::configure_from_file(debug_root.getValue(),
                           debug_config.getValue());
  i8r::with_logger("main", [](i8r::PLogger const& ll) {
    for (int i = 0; i < 20; ++i)
    {
      std::string const id = i8r::id_from_num(i);
      ll->save("txt", id, Json::Value("Some String"), "");
      ll->save("lines", id, draw_some_lines(i), "");
      ll->save("gen", id, generate_img(0.15 * (i + 1)), "");
    }
  });

  i8r::logger("info")->save("kind", "id0", Json::Value(Json::arrayValue), "");
  i8r::logger("info")->save("kind", "id1", Json::Value(Json::arrayValue), "");
}

} // ns i8r_demo

int main(int argc, char ** argv)
{
  int ret = 0;
  try
  {
    i8r_demo::main(argc, argv);
  }
  catch (std::exception const& e)
  {
    std::cerr << "Unhandled exception: " << e.what() << "\n";
    ret = 1;
  }
  catch (...)
  {
    std::cerr << "Unhandled UNTYPED exception\n";
    ret = 2;
  }
  return ret;
}
