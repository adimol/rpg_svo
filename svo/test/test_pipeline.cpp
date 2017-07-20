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
#include <thread>
#include <chrono>

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
  SVO_START_TIMER("main");
  for(int img_id = 2; img_id < 188; ++img_id)
  {
    SVO_START_TIMER("live_frames");
    // double start_time = SVO_GET_TIME();
    // double end_time = start_time;

    // SVO_START_TIMER("img_load");
    // load image
    std::stringstream ss;
    ss << svo::test_utils::getDatasetDir() << "/sin2_tex2_h1_v8_d/img/frame_"
       << std::setw( 6 ) << std::setfill( '0' ) << img_id << "_0.png";
    if(img_id == 2)
      std::cout << "reading image " << ss.str() << std::endl;
    SVO_START_TIMER("img_load");
    cv::Mat img(cv::imread(ss.str().c_str(), 0));
    SVO_STOP_TIMER("img_load");
    assert(!img.empty());

    //SVO_STOP_TIMER("img_load");

    // process frame
    vo_->addImage(img, 0.01*img_id);

    //SVO_START_TIMER("final_print");
    // display tracking quality



    if(vo_->lastFrame() != NULL)
    {
    	std::cout << "Frame-Id: " << vo_->lastFrame()->id_ << " \t"
                  << "#Features: " << vo_->lastNumObservations() << " \t"
                  << "Proc. Time: " << vo_->lastProcessingTime()*1000 << "ms \n";

    	// access the pose of the camera via vo_->lastFrame()->T_f_w_.
    }
    //SVO_STOP_TIMER("final_print");
    
    SVO_STOP_TIMER("live_frames");
    double frame_time = SVO_GET_TIMER("live_frames");
    std::this_thread::sleep_for(std::chrono::microseconds(33000 - (int)(frame_time * 1000000.0)));
    //timespec ts;
    //ts.tv_sec = 0;
    //ts.tv_nsec = 33000000 - (long long int)(frame_time * 1000000000.0);
    //nanosleep(&ts, NULL);
  }

  SVO_STOP_TIMER("main");
  g_permon->writeToFile();

}

} // namespace svo

int main(int argc, char** argv)
{
  {
    stick_thread_to_core_id(4);
    svo::BenchmarkNode benchmark;
    benchmark.runFromFolder();
  }
  printf("BenchmarkNode finished.\n");
  return 0;
}
