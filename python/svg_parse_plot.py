from math import *
import numpy as np
import matplotlib.pyplot as plt

import re

from bs4 import BeautifulSoup

r = re.compile(r"([a-z])([^a-z]*)", flags=re.IGNORECASE)

# [map(float,l.split(',')) for l in d.split()]

svg_element_list = []
complete_path = None

class SVGElement():
    def __init__(self, t, c, prev_el = None, relative_to = None):
        self.element_type = t
        self.coords = list();
        if not relative_to: relative_to = prev_el
        if prev_el:
            self.coords.append(prev_el.coords[-1])
        if t.islower() and t != "m":
            if relative_to:
                rel_coords = relative_to.coords[-1]
            else: relative_to = [0,0]
            for x in range(0, len(c), 2):
                self.coords.append([rel_coords[0] + float(c[x]), rel_coords[1] + float(c[x + 1])])
        else:
            for x in range(0, len(c), 2):
                self.coords.append([float(c[x]), float(c[x + 1])])

def parse_path(path):
    d = path['d']
    m = re.findall(r, d)
    if m:
        for match in m:
            coords = match[1]
            coords = coords.lstrip()
            coords = coords.rstrip()
            coords_list = re.split("[^\d\-.]*", coords)
            if(match[0] != 'm' and match[0] != 'M'):
                prev = svg_element_list[-1]
                for x in range(0, int(len(coords_list)), 6):
                    inst_coords = coords_list[x:x+6]
                    inst = SVGElement(match[0], inst_coords, svg_element_list[-1], relative_to=prev)
                    
            else:
                inst = SVGElement(match[0], coords_list)
            svg_element_list.append(inst)

def parse_svg(filename):
    f = open(filename, 'r')
    xml = f.read()
    f.close()
    soup = BeautifulSoup(xml)
    base = dict()
    base['w'] = soup.svg.width
    base['h'] = soup.svg.height
    
    path = soup.path
    parse_path(path)
    
def plot():
    t = np.linspace(0,1, 100)
    for el in svg_element_list:
        if el.element_type == "m" or el.element_type == "M":
            continue

        r = bezier(el.coords, t)
        plt.plot(r[:,0], r[:,1], 'k')
        plt.plot([r[0] for r in el.coords], [r[1] for r in el.coords], 'ro')
    plt.axis('equal')
    plt.gca().invert_yaxis()
    plt.show()

def bezier(P, t):
    res = np.zeros([len(t),2])
    for i, k in enumerate(t):
        for j, c in enumerate(P):
            res[i][0] = res[i][0] + c[0] * bernstein( len(P) - 1, j , t[i] )
            res[i][1] = res[i][1] + c[1] * bernstein( len(P) - 1, j , t[i] )
    return res

def bernstein(n, i, t):
    return factorial(n) / (factorial(i) * factorial(n - i)) \
       * t**i * (1-t)**(n-i)


def get_vehicle_path():
    # spiegeln am pfad
    pass

def get_path_dif(simple=True):
    res = np.array([[0,0]])
    if simple:
        dif = complete_path[:-1] - complete_path[1:]

        #for i in range(0, len(complete_path)):
            #dif = [complete_path[i] - complete_path[i+1]]
            #print(dif, res)
            #res = np.concatenate((res, dif), axis=0)
    return dif

def get_path_result():
    parse_svg("assets/test_curve.svg")
    t = np.linspace(0, 1, 100)
    res = np.array([[0,0]])
    for el in svg_element_list:
        if el.element_type == "m" or el.element_type == "M":
            continue
        bez = bezier(el.coords, t)
        res = np.concatenate((res, bez), axis=0)
    res = np.delete(res, 0, 0)
    global complete_path
    complete_path = res
    return res

if __name__ == "__main__":
    parse_svg("assets/test_curve.svg")
    get_path_result()
    get_path_dif()
    plot()

