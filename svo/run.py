import os
import subprocess
import time
import numpy as np
import csv
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D


runs = 1
targetNum = 1 # Target means both platform and configration
sequence = 33  # 10(50fps), 11(25fps), 33(30fps), 44 (21fps), 30 (21fps)
nomt = 0
nomt_QSD = 1
end = 1700
Maff = 2
Taff = 3
wait = 10
readExistingData = 1  # in the data file

#for i in range(runs):

    # note that the files produced by executing the following command will be
    # available at the directory of this python script
    # note the addition of the cd command this is important for obtaing the result file in that dir as it have a permission to write there
    # also use the full path without enviroment variable, scine they will not be decleared in the adb shell.


#os.environ["DSO_DIR"] = "/home/ak/software/dso_expremintal2/dso"
os.system('/localhome/mbax2am3/checkout/svo_ws_cmake/rpg_svo/svo/bin/test_pipeline | tee result.csv')

print("done!")

# if target == 0:
#     if os.path.isfile("/home/ak/software/dso_expremintal2/dso/build/bin/result.txt"):
#         pass
#     else:
#         print("No result provided by DSO,\nExiting ...")
#         quit()
#     print("copying results ...")
#     os.system(
#         'cp /home/ak/software/dso_expremintal2/dso/build/bin/result.txt result.csv')
#     os.system(
#         'rm /home/ak/software/dso_expremintal2/dso/build/bin/result.txt')
#     print("done!")
#
# else:
#     print("Pulling the result from the QSD820")
#     os.system('adb pull /data/local/tmp/dso/result.txt')
#     print("done")
#     if os.path.isfile("result.txt"):
#         os.system('adb shell rm /data/local/tmp/dso/result.txt')
#     else:
#         print("No result provided by DSO,\nExiting ...")
#         quit()
#     print("copying results ...")
#     os.system('cp result.txt result.csv')
#     print("done!")
print("preprocess data ...")
csvfile = open('result.csv', 'rb')

csvReader = csvfile.read()
csvfile.close()

csvReader = csvReader.replace(' ', ',')
csvReader = csvReader.replace(',,', ',')
csvReader = csvReader.replace(',,', ',')
csvReader = csvReader.replace(',,', ',')

# You need to write these changes into a file so that you can corrctly extrect
# data
fileout = open('final_result.csv', 'w')

fileout.write(csvReader)
fileout.close()
#os.system('rm result.csv')
print("done!")
# end if not readExistingData

print("plot DSO Track ...")

# create a 3d list to hold the values of multiple targets and runs runs
x = [[[] for i in range(runs)] for target in range(targetNum)]
y = [[[] for i in range(runs)] for target in range(targetNum)]
z = [[[] for i in range(runs)] for target in range(targetNum)]

xx = [[[] for i in range(runs)] for target in range(targetNum)]
yy = [[[] for i in range(runs)] for target in range(targetNum)]
zz = [[[] for i in range(runs)] for target in range(targetNum)]

#for i in range(runs):

csvfile = open('final_result.csv', 'rb')

# split based on ','
csvReader = csv.reader(csvfile, delimiter=',')

for row in csvReader:
    timestamp, tx, ty, tz, qx, qy, qz, qw = row
    x[target][i].append(tx)
    y[target][i].append(ty)
    z[target][i].append(tz)

csvfile = open('ground_truth.csv', 'rb')
csvReader = csv.reader(csvfile, delimiter=',')
for row in csvReader:
    timestamp, tx, ty, tz, qx, qy, qz, qw = row
    xx[target][i].append(tx)
    yy[target][i].append(ty)
    zz[target][i].append(tz)

# the data need to be mapped into float to avoid the error {}

x[target][i] = map(float, x[target][i])
y[target][i] = map(float, y[target][i])
z[target][i] = map(float, z[target][i])

xx[target][i] = map(float, xx[target][i])
yy[target][i] = map(float, yy[target][i])
zz[target][i] = map(float, zz[target][i])


# to plot all the graphs in one figure just name the figure to be 1 as follows
fig = plt.figure(0)
plt.hold(True)
ax = fig.add_subplot(111, projection='3d')
#ax_ground = fig.add_subplot(111, projection='3d')

# for target in range(targetNum):
#     for i in range(runs):
#        if target == 0:

#ax.set_color_cycle(['red', 'blue'])

ax.plot(x[target][i], y[target][i], z[target]
        [i], color='red')

ax.plot(xx[target][i], yy[target][i], zz[target]
        [i], color='blue')
        # if target == 1:
        #     ax.plot(x[target][i], y[target][i], z[target]
        #             [i], c=((i / (i + 1)), 1, (i / (i + 1))))
        # if target == 2:
        #     ax.plot(x[target][i], y[target][i], z[target]
        #             [i], c=((i / (i + 1)), (i / (i + 1)), 1))
        #
        # if target == 3:
        #     ax.plot(x[target][i], y[target][i], z[target]
        #             [i], c=(0.2, 1, 0.2), ls='--')
        #
        # if target == 4:
        #     ax.plot(x[target][i], y[target][i], z[target]
        #             [i], c=(0.2, 0.2, 1), ls='-.') #ls meas line style

# use this to control the scale of the axesis
ax.set_xlim3d(-15, 15)
ax.set_ylim3d(-15, 15)
ax.set_zlim3d(-15, 15)

# labels
ax.set_xlabel('X')
ax.set_ylabel('Y')
ax.set_zlabel('Z')

# show the plot on the screen
plt.show()
print("done!")

plt



#
# for target in range(targetNum):
#     for i in range(runs):
#
#         if target == 0:
#             csvfile = open('data/processedData%d%d_seq%d_MT%d.csv' %
#                            ((target), (i), (sequence), (not nomt)), 'rb')
#         if target == 1:
#             Maff = 2
#             Taff = 3
#             csvfile = open('data/processedData%d%d_seq%d_MT%d_affTM%d%d.csv' %
#                            ((target), (i), (sequence), (not nomt_QSD), (Taff), (Maff)), 'rb')
#
#         if target == 2:
#             Maff = 0
#             Taff = 1
#             csvfile = open('data/processedData%d%d_seq%d_MT%d_affTM%d%d.csv' %
#                            ((target), (i), (sequence), (not nomt_QSD), (Taff), (Maff)), 'rb')
#         if target == 3:
#             Maff = 0
#             Taff = 2
#             csvfile = open('data/processedData%d%d_seq%d_MT%d_affTM%d%d.csv' %
#                            ((target), (i), (sequence), (not nomt_QSD), (Taff), (Maff)), 'rb')
#
#         if target == 4:
#             Maff = 2
#             Taff = 0
#             csvfile = open('data/processedData%d%d_seq%d_MT%d_affTM%d%d.csv' %
#                            ((target), (i), (sequence), (not nomt_QSD), (Taff), (Maff)), 'rb')
#
#         # split based on ','
#         csvReader = csv.reader(csvfile, delimiter=',')
#
#         for row in csvReader:
#             timestamp, tx, ty, tz, qx, qy, qz, qw = row
#             x[target][i].append(tx)
#             y[target][i].append(ty)
#             z[target][i].append(tz)
#
#         # the data need to be mapped into float to avoid the error {}
#
#         x[target][i] = map(float, x[target][i])
#         y[target][i] = map(float, y[target][i])
#         z[target][i] = map(float, z[target][i])
#
# # set the size of the figure and the background color
# mlab.figure(1, size=(400, 400), bgcolor=(0, 0, 0))
# mlab.clf()
#
# for target in range(targetNum):
#     for i in range(runs):
#
#         if target == 0:
#             mlab.plot3d(x[target][i], y[target][i], z[target]
#                         [i], color=(1, (i / (i + 1)), (i / (i + 1))), tube_radius=0.01, name="HP-desktop")
#         if target == 1:
#             mlab.plot3d(x[target][i], y[target][i], z[target]
#                         [i], color=((i / (i + 1)), 1, (i / (i + 1))), tube_radius=None)
#         if target == 2:
#             mlab.plot3d(x[target][i], y[target][i], z[target]
#                         [i], color=((i / (i + 1)), (i / (i + 1)), 1), tube_radius=None)
#
#         if target == 3:
#             mlab.plot3d(x[target][i], y[target][i], z[target]
#                         [i], color=(0.8, 0.8, 0.2), tube_radius=None)
#
#         if target == 4:
#             mlab.plot3d(x[target][i], y[target][i], z[target]
#                         [i], color=(0.2, 0.8, 0.8), tube_radius=None)  # ls meas line style
#
# # use this to control the scale of the axesis
# # ax.set_xlim3d(-15, 15)
# # ax.set_ylim3d(-15, 15)
# # ax.set_zlim3d(-15, 15)
# #
# # labels
#
# axes =mlab.axes(xlabel='X', ylabel='Y', zlabel='Z')
#
#
# # show the plot on the screen
# mlab.show()
# print("done!")
#
