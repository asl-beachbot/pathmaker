#!/usr/bin/python

import numpy as np
import cairo
from scipy import ndimage
import matplotlib as mpl
#matplotlib.use("GTK3Cairo")
from matplotlib.collections import PolyCollection

import matplotlib.pyplot as plt
from scipy import misc
from math import sin, cos

# Everything is in Pixel Space for now
# 

class Rectangle():
    def draw_line(self, point1, point2):
        pass
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
            self.fresh = True
        points = np.transpose(points)
        xy = np.array([self.x, self.y]) - points[3]
        self.points = points + xy
        print(self.points)
        return self.points
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
    #r = Rectangle(10, 5, 10, 20, 0.13)
    #r.get_polygon_points()
    numpoly, numverts = 100, 4
    centers = 100 * (np.random.random((numpoly,2)) - 0.5)
    offsets = 10 * (np.random.random((numverts,numpoly,2)) - 0.5)
    verts = centers + offsets
    verts = np.swapaxes(verts, 0, 1)

    # In your case, "verts" might be something like:
    # verts = zip(zip(lon1, lat1), zip(lon2, lat2), ...)
    # If "data" in your case is a numpy array, there are cleaner ways to reorder
    # things to suit.

    # Color scalar...
    # If you have rgb values in your "colorval" array, you could just pass them
    # in as "facecolors=colorval" when you create the PolyCollection
    z = np.random.random(numpoly) * 500
    fig, ax = plt.subplots()

    coll = PolyCollection(verts, array=z, cmap=mpl.cm.jet, edgecolors='none', antialiased=False)
    ax.add_collection(coll)
    ax.autoscale_view()
    plt.axis('off')
    
    plt.show()
    #print(fig.canvas.tostring_rgb())
    data = np.fromstring(fig.canvas.tostring_rgb(), dtype=np.uint8, sep='')
    data = data.reshape(fig.canvas.get_width_height()[::-1] + (3, ))
    print(data.shape)
    plt.matshow(data[:,:,1])
    plt.show()
if __name__ == "__main__":
    run()
