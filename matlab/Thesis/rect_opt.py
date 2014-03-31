#!/usr/bin/python

import numpy as np
import cairo
from scipy import ndimage
import matplotlib
matplotlib.use("GTK3Cairo")
import matplotlib.pyplot as plt
from scipy import misc
from math import sin, cos

# Everything is in Pixel Space for now
# 

class Rectangle():
    def render(self):
        
        pass
    def get_polygon_points(self):
        if not self.fresh:
            w,h = self.w, self.h
            points = np.array([[-w/2, h/2],
                               [w/2, h/2],
                               [w/2, -h/2],
                               [-w/2, -h/2]])
            points = np.dot(self.transform, np.transpose(points))
        points = np.transpose(points)
        print(points)
        xy = np.array([self.x, self.y]) - points[3]
        points = points + xy
        print(points)
    def __init__(self, x, y, w, h, alpha):
        self.x = x
        self.y = y
        self.w = w
        self.h = h
        self.transform = np.array([[sin(alpha), cos(alpha)],
                [cos(alpha), -sin(alpha)]])
        self.alpha = alpha
        self.fresh = False
        self.points = None
# calculate a rectangle
# with x, y the lower left corner
# and transform a rotation around the center
# width and height are self-explanatory
def rectangle(x, y, width, height, transform):
    pass

def run():
    image = misc.imread('test.png')
    r = Rectangle(10, 5, 10, 20, 0.13)
    r.get_polygon_points()
    plt.imshow(image)
    
    plt.show()
if __name__ == "__main__":
    run()
