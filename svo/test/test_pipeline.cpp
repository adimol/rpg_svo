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
#include <fstream>
#include "test_utils.h"
#include <thread>


std::string source = "";
// int sleepTime = -1;



namespace svo {

class BenchmarkNode
{
  vk::AbstractCamera* cam_;
  svo::FrameHandlerMono* vo_;
  std::vector<cv::Mat> images;
  void read_memory();

public:
  BenchmarkNode();
  ~BenchmarkNode();
  void runFromFolder();
};

BenchmarkNode::BenchmarkNode()
{
  // cam_ = new vk::PinholeCamera(752, 480, 315.5, 315.5, 376.0, 240.0);
  cam_ = new vk::ATANCamera(752, 480,  0.509326, 0.796651, 0.45905, 0.510056, 0.9320);

  vo_ = new svo::FrameHandlerMono(cam_);
  vo_->start();
}

BenchmarkNode::~BenchmarkNode()
{
  delete vo_;
  delete cam_;
}

void BenchmarkNode::read_memory() {
  std::cout << "Reading dataset in memory..." << std::endl;
  int img_id = 0;
  while (true) {
    std::stringstream ss;
    ss << svo::test_utils::getDatasetDir() << "/img/"
       << std::setw(5) << std::setfill('0') << img_id++ << ".png";
    cv::Mat img(cv::imread(ss.str().c_str(), 0));
    if (img.empty())
      break;
    images.push_back(img);
  }
}

void BenchmarkNode::runFromFolder()
{
  struct timeval t1;
  gettimeofday(&t1, NULL);

  ofstream result;
  result.open("result.txt");

  // Sophus::SE3 T_world_from_vision_ = Sophus::SE3(vk::rpy2dcm(Eigen::Vector3d(3.14, 0.0, 0.5109)), // rotation
  //                                                            Eigen::Vector3d(0.1131, 0.1131, 2.0)); // translation
  Sophus::SE3 T_world_from_vision_ = Sophus::SE3(vk::rpy2dcm(Eigen::Vector3d(3.14, 0.0, 0.0)), // rotation
                                                              Eigen::Vector3d(0.0, 0.0, 0.0)); // translation

  if (source == "")
    source = svo::test_utils::getDatasetDir();

  read_memory(source);

  struct timeval t2;
  gettimeofday(&t2, NULL);

  printf("The time taken before the actual start is %.3f ms\n", (t2.tv_sec - t1.tv_sec) * 1000.0f +
                        (t2.tv_usec - t1.tv_usec) / 1000.0f);


  // if (sleepTime != -1) {
  //   std::this_thread::sleep_for(std::chrono::seconds(sleepTime));
  //   printf("Waited %d seconds after loading All images!\n",sleepTime);
  // }




  vk::Timer time;
  time.start();

  for(unsigned int img_id = 0; img_id < images.size(); ++img_id)
  {
    assert(!images[img_id].empty()); // just to be safer

    // process frame
    vo_->addImage(images[img_id], 0.01*img_id);

    // display tracking quality
    if(vo_->lastFrame() != NULL)
    {
    	std::cout << "Frame-Id: " << vo_->lastFrame()->id_ << " \t"
                  << "#Features: " << vo_->lastNumObservations() << " \t"
                  << "Proc. Time: " << vo_->lastProcessingTime()*1000 << "ms \n";

    	// access the pose of the camera via vo_->lastFrame()->T_f_w_.

      const FramePtr& frame = vo_->lastFrame();

      Sophus::SE3 T_world_from_cam(T_world_from_vision_ * frame->T_f_w_.inverse());
      Eigen::Quaterniond q = Eigen::Quaterniond(T_world_from_cam.rotation_matrix());
      Eigen::Vector3d p = T_world_from_cam.translation();

      // std::cout << img_id << ","
      //           << p[0] << ","
      //           << p[1] << ","
      //           << p[2] << ","
      //           << q.x() << ","
      //           << q.y() << ","
      //           << q.z() << ","
      //           << q.w() << std::endl;

      result << std::setprecision(15);
      result << img_id << " "
                << p[0] << " "
                << p[1] << " "
                << p[2] << " "
                << q.x() << " "
                << q.y() << " "
                << q.z() << " "
                << q.w() << "\n";

    }
  }
  printf("The time from actual start is %.3f ms\n", time.stop());
  result.close();
}

} // namespace svo


void parseArgument(char *arg) {
  int option;
  float foption;
  char buf[1000];

  // if (1 == sscanf(arg, "wait=%d", &option)) {
  //     sleepTime = option;
  //     return;
  // }


  // if (1 == sscanf(arg, "Taff=%d", &option)) {
  //   if (option == 2 || option == 3) {
  //     trackingThreadAff = option;
  //     printf("Tracking thread affinity on QSD820: Big(%d)\n", trackingThreadAff);
  //   } else if (option == 0 || option == 1) {
  //     trackingThreadAff = option;
  //     printf("Tracking thread affinity on QSD820: LITTLE(%d)\n", trackingThreadAff);
  //   }else
  //   {
  //     printf("Note: The Affinity must be 0 to 3 for QSD820, using default setting\n");
  //   }
  //   return;
  // }
  //
  // if (1 == sscanf(arg, "Maff=%d", &option)) {
  //   if (option == 2 || option == 3) {
  //     mappingThreadAff = option;
  //     printf("Mapping thread affinity on QSD820: Big(%d)\n", mappingThreadAff);
  //   } else if (option == 0 || option == 1) {
  //     mappingThreadAff = option;
  //     printf("Mapping thread affinity on QSD820: LITTLE(%d)\n", mappingThreadAff);
  //   }else
  //   {
  //     printf("Note: The Affinity must be 0 to 3 for QSD820, using default setting\n");
  //   }
  //   return;
  // }

  if (1 == sscanf(arg, "dir=%s", buf)) {
    source = buf;
    printf("loading data from %s!\n", source.c_str());
    return;
  }


  printf("could not parse argument \"%s\"!!!!\n", arg);
}



int main(int argc, char** argv)
{

  for (int i = 1; i < argc; i++)
    parseArgument(argv[i]);


  {

    // if (trackingThreadAff != -1) {
    //
    //   cpu_set_t cpuset;
    //   CPU_ZERO(&cpuset);
    //   CPU_SET(trackingThreadAff, &cpuset);
    //   /* sched_setaffinity returns 0 in success */
    //   if (sched_setaffinity(0, sizeof(cpuset), &cpuset) == -1) {
    //     printf("Could not set CPU Affinity for tracking thread\n");
    //     exit(0);
    //   }
    // }


    svo::BenchmarkNode benchmark;
    benchmark.runFromFolder();
  }
  printf("BenchmarkNode finished.\n");
  return 0;
}
