from math import *
import copy
import numpy as np
import matplotlib
#matplotlib.use('cairo')
from matplotlib.patches import Circle, Rectangle
import matplotlib.pyplot as plt
from matplotlib.transforms import Affine2D
from matplotlib.animation import FuncAnimation
from matplotlib.collections import PatchCollection

from svg_parse_plot import *


## General settings

fig, ax = plt.subplots(1, figsize=(6,6))
ax.set_xlim(0,1000)
ax.set_ylim(0,1000)
plt.gca().invert_yaxis() # SVG -> correct direction

init_pos = np.array([0,0])

robot = Rectangle(init_pos, 30, 18, color="red")

rake_pos = init_pos - np.array([3,3])
rake = Rectangle(init_pos, 3, 10, color="blue")

fw_pos = init_pos + np.array([3,3])
front_wheel = Rectangle(init_pos, 5, 4, fill=False, linestyle="solid", color="black", linewidth=1, zorder=100)

path = np.zeros((1,2))
rake_trace = []
rake_trace_line, = ax.plot([0,0])
def center_from_rectangle(rect):
    return np.array([rect.get_x() + rect.get_width() / 2,
                     rect.get_y() + rect.get_height() / 2])

def animate(i):
    x,y = path[i]
    #theta = path_dif(i)
    path_dif = path[i+1] - path[i]
    theta = atan2(path_dif[1], path_dif[0])
    print(robot.get_width(), robot.get_height())

    robot.set_xy((x - robot.get_width() / 2,y - robot.get_height() / 2))
    robot_center = center_from_rectangle(robot)

    rake.set_xy(robot_center - np.array([20, rake.get_height() / 2]))
    front_wheel.set_xy(robot_center + np.array([5, - front_wheel.get_height()/2]))
    trans = ax.transData
    t = Affine2D().rotate_deg_around(robot_center[0],
                                     robot_center[1], theta / pi * 180)
    robot.set_transform(t + trans)
    rake.set_transform(t + trans)
    front_wheel.set_transform(t + trans)

    rake_trace.append([center_from_rectangle(rake)[0], center_from_rectangle(rake)[1]])
    print(rake_trace)
    #rake_trace_line.set_data(rake_trace[0], rake_trace[1])
def init_robot():
    ax.add_patch(robot)
    ax.add_patch(rake)
    ax.add_patch(front_wheel)
    plt.plot(path[:,0], path[:,1], 'k')

def run():
    global path
    path = get_path_result()
    print(path)
    init_robot()
    anim = FuncAnimation(fig, animate, 350, interval=10)
    plt.show()


if __name__ == "__main__":
    run()
