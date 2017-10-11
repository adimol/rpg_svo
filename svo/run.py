#!/usr/bin/python2.7
import os
import subprocess
import time
import numpy as np
import csv
import glob
import matplotlib as mpl
from mpl_toolkits.mplot3d import Axes3D
import matplotlib.pyplot as plt


# change readExistingData to 0 for generating new data, or 1 for plotting from existing csv files.

# graphs are read for each csv file

# ground_truth for both artificial and mav_circle datasets are located in the ground_truth folder. Copy
#    the one specific to the used dataset in this main SVO folder. Adjust the camera parameters in SVO
#    in the file ./test/test_pipeline.cpp, then run the python script to generate the data and the graphs.

# the generated data from SVO is placed in a file final_result.csv

runs = 10 # number of graphs to be plotted
targetNum = 1 # Target means both platform and configration
readExistingData = 0  # in the data file

if readExistingData == 0:
    os.system('./bin/test_pipeline | tee final_result.csv')

    print("done!")

    # print("preprocess data ...")
    # csvfile = open('result.csv', 'rb')
    #
    # csvReader = csvfile.read()
    # csvfile.close()
    #
    # csvReader = csvReader.replace(' ', ',')
    # csvReader = csvReader.replace(',,', ',')
    # csvReader = csvReader.replace(',,', ',')
    # csvReader = csvReader.replace(',,', ',')

# You need to write these changes into a file so that you can corrctly extrect
# data
    # fileout = open('final_result1.csv', 'w')
    #
    # fileout.write(csvReader)
    # fileout.close()
#os.system('rm result.csv')
    # print("done!")
# end if not readExistingData


# create a 3d list to hold the values of multiple targets and runs runs
x = [[[] for i in range(runs)] for target in range(targetNum)]
y = [[[] for i in range(runs)] for target in range(targetNum)]
z = [[[] for i in range(runs)] for target in range(targetNum)]



mpl.rcParams['legend.fontsize'] = 10

fig = plt.figure()
# plt.hold(True)
ax = fig.gca(projection='3d')
# ax = fig.add_subplot(111, projection='3d')


#for i in range(runs):
i = 0
for file in glob.glob("*.csv"):
    csvfile = open(file, 'rb')
#csvfile = open('final_result.csv', 'rb')

# split based on ','
    csvReader = csv.reader(csvfile, delimiter=',')

    for row in csvReader:
        timestamp, tx, ty, tz, qx, qy, qz, qw = row
        x[target][i].append(tx)
        y[target][i].append(ty)
        z[target][i].append(tz)


    x[target][i] = map(float, x[target][i])
    y[target][i] = map(float, y[target][i])
    z[target][i] = map(float, z[target][i])



    ax.plot(x[target][i], y[target][i], z[target]
            [i], label=file)

    i += 1

# use this to control the scale of the axesis
ax.set_xlim3d(-5, 25)
ax.set_ylim3d(-5, 25)
ax.set_zlim3d(-5, 25)

# labels
ax.set_xlabel('X')
ax.set_ylabel('Y')
ax.set_zlabel('Z')

# show the plot on the screen
ax.legend()
plt.show()
print("done!")
