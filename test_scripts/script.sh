#!/bin/bash

mkdir -p results

for rate in {1,2,4,6,8,10,12,14} # framerate = rate * 30 fps
do
  for f in test*; # go through each DVFS script
  do
    # run twice: first time won't work when min freq > max or the other way around
    ./$f;
    sleep 1;
    ./$f;
    for i in {1..3} # run each test 3 times
    do
      echo "Running iteration $i";

      roslaunch svo_ros test_rig3_$(($rate*30)).launch > /dev/null 2>&1 & sleep 10 ; turbostat --debug --Joules ./sleep.sh > result.txt 2>&1 & time rosbag play -r $rate dataset.bag ; killall sleep.sh ; killall roslaunch;

      sleep 2;
      mv result.txt ./results/$(($rate * 30))\_$f\_$i.txt;
      mv /tmp/svo.csv ./results/$(($rate * 30))\_$f\_$i.csv;
    done

    echo "Running fine-grained iteration $i";
    i=$(($i+1)) # and an aditional time with fine-grained turbostat
    sleep 2;

    roslaunch svo_ros test_rig3_$(($rate*30)).launch > /dev/null 2>&1 & sleep 10 ;turbostat -i 0.005 --Joules --debug -S | awk '{ print $13 }' > result.txt 2>&1 & time rosbag play -r $rate dataset.bag ; killall roslaunch; sleep 5; killall turbostat;

      sleep 2;
      mv result.txt ./results/$(($rate * 30))\_$f\_$i.txt;
      mv /tmp/svo.csv ./results/$(($rate * 30))\_$f\_$i.csv;
  done
done
sleep 5;
