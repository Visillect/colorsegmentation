#include <algorithm>
#include <iostream>
#include <memory>
#include <vector>
#include <cmath>
#include <minbase/crossplat.h>
#include <minimgapi/minimgapi-helpers.hpp>
#include <minimgapi/imgguard.hpp>
#include <minimgio/minimgio.h>
#include <vi_cvt/std/exception_macros.hpp>

#include <remseg/segmentator.hpp>
#include <remseg/utils.h>

#include <validate_json/validate_json.h>

THIRDPARTY_INCLUDES_BEGIN
#include <json-cpp/json.h>
#include <boost/filesystem.hpp>
#include <Eigen/Dense>
#include <tclap/CmdLine.h>
THIRDPARTY_INCLUDES_END

namespace bfs = boost::filesystem;

using namespace vi::remseg;

void read_mean(Eigen::Vector3d & mean, Json::Value const & json)
{
  for (int i = 0; i < 3; ++i)
    mean(i, 0) = json[i].asDouble();
}

void read_cov(Eigen::Matrix3d & cov, Json::Value const & json)
{
  for (int i = 0; i < 3; ++i)
    for (int j = 0; j < 3; ++j)
      cov(i, j) = json[i][j].asDouble();
}

//double student(std::vector<double> const & m1, std::vector<double> const & m2)
//{
//  return ;
//}

double euclidean(std::vector<double> const & m1, std::vector<double> const & m2)
{
  assert(m1.size() == m2.size());
  double sqrSum = 0;
  for (size_t i = 0; i < m1.size(); i++)
    sqrSum += std::pow(m1[i] - m2[i], 2);
  return std::sqrt(sqrSum);
}


double KL(Eigen::Vector3d const & mean1, Eigen::Vector3d const & mean2,
          Eigen::Matrix3d const & cov1, Eigen::Matrix3d const & cov2)
{
  // auto cov = (cov1 + cov2) / 2;
  // auto s1 = 1. / 8 * (mean1 - mean2).transpose() * cov.inverse() * (mean1 - mean2);
  // auto s2 = 1. / 2 * std::log(cov.determinant() /
  //                             std::sqrt(cov1.determinant() * cov2.determinant()));
  // assert(m1.size() == m2.size());
  // double sqrSum = 0;
  // for (size_t i = 0; i < m1.size(); i++)
  //   sqrSum += std::pow(m1[i] - m2[i], 2);
  // return std::sqrt(sqrSum);

  auto s1 = (cov2.inverse() * cov1).trace();
  auto s2 = (mean2 - mean1).transpose() * cov2.inverse() * (mean2 - mean1);
  auto s3 = std::log(cov2.determinant() / cov1.determinant());
  return (s1 + s2 + s3 - 3)/ 2;
}


int main(int argc, const char *argv[])
{
  TCLAP::CmdLine cmd("Run Range-Based Region Merge Segmentation on a Single image");
  TCLAP::ValueArg<std::string> blockingPolicy("", "blocking_policy", "[segments, edges]", false, "segments", "string", cmd);
  TCLAP::ValueArg<std::string> imageMapPath("m", "map", "path to file with image map source in tif-convertible format", true, "", "string", cmd);
  TCLAP::ValueArg<std::string> logPath("l", "log", "path to logfile", true, "", "string", cmd);
  TCLAP::ValueArg<std::string> output("o", "output", "path to output dir", false, ".", "string", cmd);
  TCLAP::ValueArg<double> threshold("t", "threshold", "threshold value", false, 10, "double", cmd);
  TCLAP::ValueArg<std::string> distFunc("f", "dist_func", "distance function", false, "student", "string", cmd);

  cmd.parse(argc, argv);

  if (!bfs::is_directory(output.getValue()))
    throw std::runtime_error("Failed to find output directory " + output.getValue());

  bool blocking_policy;
  if      (blockingPolicy.getValue() == "segments") blocking_policy = BLOCK_SEGMENTS;
  else if (blockingPolicy.getValue() == "edges") blocking_policy = BLOCK_EDGES;
  else     throw std::runtime_error("Wrong blocking policy");

  // typedef double (*DistanceFunction)(std::vector<double> const & m1,
  //                                    std::vector<double> const & m2);
  // DistanceFunction distance_function = euclidean;

  std::string const basename = bfs::path(imageMapPath.getValue()).stem().string();
  std::string const jsonres_filename = bfs::absolute(basename + ".block.json", output.getValue()).string();
  std::string const imgres_filename = bfs::absolute(basename + ".block.png", output.getValue()).string();

  try
  {
    ImageMap imageMap(imageMapPath.getValue().c_str());
    std::set<std::pair<int, int> > blockList, blockListVis;

    auto stats = imageMap.getSegmentStats();
    Json::Value root = vi::json_from_file(logPath.getValue());
    for (auto& segment : root["segments"])
    {

      std::vector<EdgeValue> dists;
      //std::vector<double> mean = (segment["statistics"]["mean"]);
      Eigen::Vector3d mean, n_mean;
      Eigen::Matrix3d cov, n_cov;
      read_mean(mean, segment["statistics"]["mean"]);
      read_cov(cov, segment["statistics"]["cov"]);

      for (auto const& n_json : segment["neighbours"])
      {
        int n = n_json.asInt();
        read_mean(n_mean, root["segments"][std::to_string(n)]["statistics"]["mean"]);
        read_cov(n_cov, root["segments"][std::to_string(n)]["statistics"]["cov"]);
        dists.push_back(KL(mean, n_mean, cov, n_cov));
        // std::cout << dists.back() << std::endl;
      }

      if (!dists.empty() && *std::min_element(dists.begin(), dists.end()) > threshold.getValue())
      {
        Json::Value ltp = segment["leftTopPoint"];
        int segmId = imageMap.getSegment(ltp[0].asInt(), ltp[1].asInt());
        auto & stat = stats[segmId];
        blockList.insert(stat.leftTopPoint);
        blockListVis.insert(stat.leftTopPoint);
        if (blocking_policy == BLOCK_EDGES)
        {
          for (auto const& n : stat.neighbours)
            blockList.insert(stats[n].leftTopPoint);
        }
      }
    }

    saveBlockList(jsonres_filename, blockList, blocking_policy);

    DECLARE_GUARDED_MINIMG(imgres);
    visualize(&imgres, imageMap, blockListVis);
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
