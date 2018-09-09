#include <iostream>
#include <descartes_light/descartes_light.h>
#include <opw_kinematics/opw_parameters_examples.h>

namespace
{

std::vector<descartes_light::PositionSamplerPtr> makePath()
{
  // The current setup requires that our cartesian sampler is aware of the robot
  // kinematics
  opw_kinematics::Parameters<double> kin_params = opw_kinematics::makeIrb2400_10<double>();
  descartes_light::KinematicsInterface kin_interface (kin_params,
                                                      Eigen::Isometry3d::Identity(),
                                                      Eigen::Isometry3d::Identity());

  Eigen::Isometry3d reference = Eigen::Isometry3d::Identity() * Eigen::Translation3d(1.0, 0, 0.5) *
                                Eigen::AngleAxisd(M_PI, Eigen::Vector3d::UnitY());

  std::vector<descartes_light::PositionSamplerPtr> result;
  for (int i = 0; i < 5; ++i)
  {
    result.push_back(
          std::make_shared<descartes_light::CartesianPointSampler>(
            reference * Eigen::Translation3d(i * .1, 0, 0), kin_interface));
  }

  return result;
}

} // anon ns

int main(int argc, char** argv)
{
  // Define our vertex samplers
  const auto path = makePath();

  // What logic to connect edges?
  auto edge_eval = std::make_shared<descartes_light::DistanceEdgeEvaluator>();

  descartes_light::Solver graph_builder (6);
  if (!graph_builder.build(path, edge_eval))
  {
    std::cerr << "Failed to build graph\n";
    return 1;
  }

  // Search for edges
  if (!graph_builder.search())
  {
    std::cerr << "Search for graph completion failed\n";
    return 1;
  }

  return 0;
}