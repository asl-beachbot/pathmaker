#!/usr/bin/python

import numpy as np
import cairo
from scipy import ndimage
import random
from PIL import Image, ImageDraw
#from matplotlib.patches import Rectangle
import matplotlib.pyplot as plt
from scipy import misc
from math import sin, cos, pi

# Everything is in Pixel Space for now
# 
width, height = 100, 100
size = width * height
last_step = None

def resize_handler(arg):
    return False

class Rectangle():
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

    def rotate(self, alpha):
        self.alpha = alpha
        self.fresh = False

    def move(self, x, y):
        self.x = x
        self.y = y
        self.fresh = False

    def scale(self, w, h):
        self.w = w
        self.h = h
        self.fresh = False

 # calculate a rectangle
 # with x, y the lower left corner
 # and transform a rotation around the center
 # width and height are self-explanatory
def rectangle(x, y, width, height, transform):
    pass

def modify_poly_list(polys):
    dmin, dmax = -10, 10
    global last_step
    rand_start = 1
    if(len(polys)) > 20:
        rand_start = 2
    selection = random.randint(rand_start, 4)
    if len(polys) <= 1:
        selection = 1
    if selection == 1:
        x = random.randint(0, width)
        y = random.randint(0, height)
        mw = 30 if width - x > 30 else width - x 
        mh = 30 if (height - y) > 30 else  height - y
        # mw, mh = 1,1
        w = random.randint(0, mw)
        h = random.randint(0, mh)
        alpha = random.random() * pi
        rect = Rectangle(x, y, w, h, alpha)
        polys.append(rect)
        last_step = ("append_rect", rect)
    elif selection == 2:
        i = random.randint(0, len(polys) -1)
        last_step = ("rotate_rect", i, polys[i].alpha)
        polys[i].rotate(random.random() * pi)
    elif selection == 3:
        mx = random.randint(dmin, dmax)
        my = random.randint(dmin, dmax)
        i = random.randint(0, len(polys) - 1)
        last_step = ("move_rect", i, polys[i].x, polys[i].y)
        polys[i].move(polys[i].x + mx, polys[i].y + my)
    elif selection == 4:
        mw = random.randint(dmin, dmax)
        mh = random.randint(dmin, dmax)
        i = random.randint(0, len(polys) - 1)
        last_step = ("scale_rect", i, polys[i].w, polys[i].h)
        polys[i].scale(polys[i].w + mw, polys[i].h + mh)
    # pass
def undo_last_step(polys):
    global last_step
    if last_step[0] == "append_rect":
        # polys.remove(last_step[1])
        del polys[-1]
    elif last_step[0] == "rotate_rect":
        polys[last_step[1]].rotate(last_step[2])
    elif last_step[0] == "move_rect":
        polys[last_step[1]].move(last_step[2], last_step[3])
    elif last_step[0] == "scale_rect":
        polys[last_step[1]].scale(last_step[2], last_step[3])

def calc_error(image, res):
    ## error: 
    ##  every region that is black and and should be white
    ##  and every pixel that is white and should be black
    ##  counts towards the error
    bool_arr = image == res
    error = size - np.count_nonzero(bool_arr)
    return error

def run():
    image = misc.imread('test.png')
    polys = list()
    # r = Rectangle(10, 5, 10, 20, -0.55)
    # polys.append(r)
    img = Image.new('L', (width, height), 255)
    draw = ImageDraw.Draw(img)
    data = np.array(img)
    i = 0 
    error = calc_error(image, data)
    last_error = 100000
    while error > 500:
        i = i + 1
        modify_poly_list(polys)
        img = Image.new('L', (width, height), 255)
        for poly in polys:
            ImageDraw.Draw(img).polygon(poly.get_polygon_points().flatten().tolist(), outline=0, fill=0)
        data = np.array(img)
        if not i % 500:
            img.save("./results/test_" + str(i) + ".png")
        error = calc_error(image, data)
        print "i: %d | Error: %d | LastError: %d" % (i, error, last_error)
        if error > last_error:
            print "i %d | Error: %d" % (i, error) 
            undo_last_step(polys)
            print "Ruckgangig %s" % last_step.__repr__()
            img = Image.new('L', (width, height), 255)
            for poly in polys:
                ImageDraw.Draw(img).polygon(poly.get_polygon_points().flatten().tolist(), outline=0, fill=0)
            data = np.array(img)
            error_new = calc_error(image, data)
            print "ErrorNeu: %d" % error_new 
        else:
            last_error = error
        if not i % 1000:
            print "ERROR = %s" % error

        if i > 10000:
            # img_transparent = Image.new("RGBA", (width, height), (255, 255, 255, 0))
            img2 = Image.open("test.png")
            for poly in polys:
                ImageDraw.Draw(img2).polygon(poly.get_polygon_points().flatten().tolist(), outline=(255,0,0,129), fill=(255,0,0,129))
            img2.save("./results/_final_result.png")
            img2.show()
            break;
        
    img2 = Image.open("test.png").convert("RGBA")
    rgb_start = (25, 180, 0, 0)
    i = 0
    for poly in polys:
        ImageDraw.Draw(img2, "RGBA").polygon(poly.get_polygon_points().flatten().tolist(), outline=(255,25 + i*15,0,129), fill=(255 - i*15,0,15*i,129))
        i = i + 1
    img2.save("./results/_final_result.png")
    img2.show()
if __name__ == "__main__":
    run()
