// This file is part of SVO - Semi-direct Visual Odometry.
//
// Copyright (C) 2014 Christian Forster <forster at ifi dot uzh dot ch>
// (Robotics and Perception Group, University of Zurich, Switzerland).
//
// SVO is free software: you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the Free Software
// Foundation, either version 3 of the License, or any later version.
//
// SVO is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include <svo/config.h>
#include <svo/frame_handler_mono.h>
#include <svo/map.h>
#include <svo/frame.h>
#include <vector>
#include <string>
#include <vikit/math_utils.h>
#include <vikit/vision.h>
#include <vikit/abstract_camera.h>
#include <vikit/atan_camera.h>
#include <vikit/pinhole_camera.h>
#include <opencv2/opencv.hpp>
#include <sophus/se3.h>
#include <iostream>
#include "test_utils.h"

namespace svo {

class BenchmarkNode
{
  vk::AbstractCamera* cam_;
  svo::FrameHandlerMono* vo_;

public:
  BenchmarkNode();
  ~BenchmarkNode();
  void runFromFolder();
};

BenchmarkNode::BenchmarkNode()
{
  cam_ = new vk::PinholeCamera(752, 480, 315.5, 315.5, 376.0, 240.0);
  //cam_ = new vk::ATANCamera(752, 480,  0.509326, 0.796651, 0.45905, 0.510056, 0.9320);
  vo_ = new svo::FrameHandlerMono(cam_);
  vo_->start();
}

BenchmarkNode::~BenchmarkNode()
{
  delete vo_;
  delete cam_;
}

void BenchmarkNode::runFromFolder()
{

  Sophus::SE3 T_world_from_vision_ = Sophus::SE3(vk::rpy2dcm(Eigen::Vector3d(3.14, 0.0, 0.5109)), // rotation
                                                             Eigen::Vector3d(0.1131, 0.1131, 2.0)); // translation
  // Sophus::SE3 T_world_from_vision_ = Sophus::SE3(vk::rpy2dcm(Eigen::Vector3d(3.14, 0.0, 0.0)), // rotation
  //                                                             Eigen::Vector3d(0.0, 0.0, 0.0)); // translation
for(int img_id = 0; img_id < 185; ++img_id)
  {
    // load image
    std::stringstream ss;
    ss << svo::test_utils::getDatasetDir() << "/sin2_tex2_h1_v8_d/img/"
       << std::setw( 6 ) << std::setfill( '0' ) << img_id << ".png";
    if(img_id == 2);
      //std::cout << "reading image " << ss.str() << std::endl;
    cv::Mat img(cv::imread(ss.str().c_str(), 0));
    assert(!img.empty());

    // process frame
    vo_->addImage(img, 0.01*img_id);

    // display tracking quality
    if(vo_->lastFrame() != NULL)
    {
    	// std::cout << "Frame-Id: " << vo_->lastFrame()->id_ << " \t"
        //           << "#Features: " << vo_->lastNumObservations() << " \t"
        //           << "Proc. Time: " << vo_->lastProcessingTime()*1000 << "ms \n";
        //*mat = vo_->lastFrame()->T_f_w_.matrix().cast<float>();
    const FramePtr& frame = vo_->lastFrame();

    Sophus::SE3 T_world_from_cam(T_world_from_vision_ * frame->T_f_w_.inverse());
    Eigen::Quaterniond q = Eigen::Quaterniond(T_world_from_cam.rotation_matrix());
    Eigen::Vector3d p = T_world_from_cam.translation();

    std::cout << 0.01*img_id << ","
              << p[0] << ","
              << p[1] << ","
              << p[2] << ","
              << q.x() << ","
              << q.y() << ","
              << q.z() << ","
              << q.w() << std::endl;
    }
    //usleep(120000);
  }
}

} // namespace svo

int main(int argc, char** argv)
{
  {
    svo::BenchmarkNode benchmark;
    benchmark.runFromFolder();
  }
  //printf("BenchmarkNode finished.\n");
  return 0;
}
