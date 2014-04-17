#!/usr/bin/python3

## Parser application
#  Parses Polygon data from SVG, DAT and Matlab to SVG, DAT and Matlab
#  Usage: ./parse_dat.py --help
#  License: GPL-v3
#  (c) Wolf Vollprecht <wolfv@student.ethz.ch>

## TODO
#  Implement remaining formats if needed
#  Investigate .mat usage for matlab (scipy)

import argparse

from bs4 import BeautifulSoup
import re

import numpy as np

from math import *

from numpy import linspace

r = re.compile(r"([a-z])([^a-z]*)", flags=re.IGNORECASE)

t = linspace(1/10, 1, 10-1)


class SVGElement():
    def __init__(self, t, c, prev_el=None, relative_to=None):
        self.element_type = t
        print(c)
        self.coords = list()
        if t == "z":
            return
        if c[0] == '':
            return
        if prev_el:
            self.coords.append(prev_el.coords[-1])
        if t.islower():
            if relative_to or t == "C":
                rel_coords = relative_to.coords[-1]
            else:
                relative_to = [0, 0]
            for x in range(0, len(c), 2):
                # print(c)
                self.coords.append([rel_coords[0] + float(c[x]),
                                    rel_coords[1] + float(c[x + 1])])
        else:
            for x in range(0, len(c), 2):
                self.coords.append([float(c[x]), float(c[x + 1])])

        # print(self)

    def __repr__(self):
        print(self.element_type, self.coords)

    def __str__(self):
        return "%s | %r" % (self.element_type, self.coords)

    @staticmethod
    def bezier(P, t):
        def bernstein(n, i, t):
            return factorial(n) / (factorial(i) * factorial(n - i)) \
                * t**i * (1-t)**(n-i)
        res = np.zeros([len(t), 2])
        for i, k in enumerate(t):
            for j, c in enumerate(P):
                res[i][0] = res[i][0] + c[0] * bernstein(len(P) - 1,
                                                         j, t[i])
                res[i][1] = res[i][1] + c[1] * bernstein(len(P) - 1,
                                                         j, t[i])
        return res

    def to_poly(self):
        if self.element_type == "z":
            return
        if self.element_type == "c" or self.element_type == "C":
            return SVGElement.bezier(self.coords, t)
        if self.element_type in ["l", "L"]:
            print (self.coords[-1])
            return [self.coords[-1]]
        return self.coords

    @staticmethod
    def parse_path(path, container=list()):
        print(path)
        print("test")
        d = path['d']
        element = dict()
        if path.has_attr("fill") and path["fill"].lower() != "none":
            element['filled'] = True
        else:
            element['filled'] = False
        if d.endswith("z") or d.endswith("Z"):
            element['closed'] = True
        else:
            element['closed'] = False

        m = re.findall(r, d)
        element['svg_elems'] = list()
        if m:
            for match in m:
                coords = match[1]
                coords = coords.lstrip()
                coords = coords.rstrip()
                coords_list = re.split("[^\d\-.]*", coords)
                #if(coords_list[0] == ''): continue
                for x in range(0, int(len(coords_list)), 6):
                    prev_el = element['svg_elems'][-1] if element['svg_elems'] else None
                    inst_coords = coords_list[x:x+6]
                    inst = SVGElement(match[0], inst_coords, prev_el)

                element['svg_elems'].append(inst)
        container.append(element)
        return container


def parse(filename):
    # return {
    #     "svg_base": {
    #         "width": 512,
    #         "height": 215
    #     },
    #     "elements": [
    #         {
    #             "closed": True,
    #             "filled": False,
    #             "coords": [(1, 2), (3, 5), (7, 1)]
    #         },
    #         {
    #             "closed": True,
    #             "filled": True,
    #             "coords": [(1, 2), (3, 5), (7, 1)]
    #         }
    #     ]
    # }
    try:
        f = open(filename, 'r')
    except:
        print("Illegal Filename")
        return False
    poly = list()
    svg_element_list = list()
    xml = f.read()
    f.close()
    soup = BeautifulSoup(xml)
    base = dict()
    base['width'] = float(soup.svg['width'])
    base['height'] = float(soup.svg['height'])
    print(soup.svg['width'])
    res = dict()
    res['svg_base'] = base
    res["elements"] = list()
    path = soup.find_all("path")

    for p in path:
        SVGElement.parse_path(p, svg_element_list)

    polys = list()
    for el in svg_element_list:
        poly = list()
        for e in el["svg_elems"]:
            poly.extend(tuple(map(tuple, e.to_poly())))

            # print(e.element_type.__repr__() + e.to_poly().__str__())

            if e.element_type == "z":
                if len(e.coords) == 0:
                    pass
                else:
                    del poly[0]

        print(poly)
        poly.reverse()
        polys.append(poly)
        res["elements"].append({
            "closed": el["closed"],
            "filled": el["filled"],
            "coords": poly
            })
    return res
    text = ""
    for idx, poly in enumerate(polys):
        if(idx == 1):
            text += "%s\n" % (len(polys) - 1)
        text += "%s\n" % len(poly)
        for p2 in poly:
            text += "%s %s\n" % (p2[0], p2[1])
    text = text[:-1]
    # print(text)
    return text


def run():
    if not ns.filename:
        print("No Filename supplied")
        return False
    elif not ns.to:
        ns.to = 'mat'
    try:
        f = open(ns.filename, 'r')
    except:
        print("Illegal Filename")
        return False

    poly = list()

    if ns.filename.endswith("dat"):
        firstline = False
        for line in f:
            if " " in line:
                (x, y) = line.rstrip("\n").split(" ")
                poly.append([x, y])
            else:
                if firstline:
                    break
                firstline = True

    elif ns.filename.endswith("svg"):
        svg_element_list = list()
        xml = f.read()
        f.close()
        soup = BeautifulSoup(xml)
        base = dict()
        base['w'] = soup.svg.width
        base['h'] = soup.svg.height

        path = soup.find_all("path")

        for p in path:
            SVGElement.parse_path(p, svg_element_list)

        polys = list()
        for el in svg_element_list:
            poly = list()
            for e in el:
                poly.extend(e.to_poly())

                # print(e.element_type.__repr__() + e.to_poly().__str__())

                if e.element_type == "z":
                    if len(e.coords) == 0:
                        pass
                    else:
                        del poly[0]

            poly.reverse()
            polys.append(poly)
        # print(polys)
    if ns.to == "mat":
        text = "poly = ["
        for p2 in poly:
            text += "%s, %s;\n" % (p2[0], p2[1])
        text += "];"
        name = ns.filename.split(".")[0]
        name += ".m"
        fw = open(name, 'w')
        fw.write(text)

    if ns.to == "dat":
        text = ""
        for idx, poly in enumerate(polys):
            if(idx == 1):
                text += "%s\n" % (len(polys) - 1)
            text += "%s\n" % len(poly)
            for p2 in poly:
                text += "%s %s\n" % (p2[0], p2[1])
        text = text[:-1]
        # print(text)
        name = ns.filename.split(".")[0]
        name += ".dat"
        fw = open(name, 'w')
        fw.write(text)

if __name__ == '__main__':
    parser = argparse.ArgumentParser(
             description="Convert SVG, DAT and MAT input and output.")
    parser.add_argument('-t', '--to', type=str,
                        help="Filetype to convert to: svg, mat, dat")
    parser.add_argument('filename', type=str, help="Filename to convert")
    parser.add_argument('-m', '--mode', type=str,
                        help="""The mode you want to convert:
                                equal spaced or approximated""")
    parser.add_argument('-r', '--resolution', type=int,
                        help="resolution for approximation")
    ns = parser.parse_args()
    t = linspace(1/10, 1, 10-1) if not ns.resolution else linspace(1/ns.resolution, 1, ns.resolution - 1)

    run()
