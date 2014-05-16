#!/usr/bin/python3

## Parser application
#  Parses Polygon data from SVG, DAT and Matlab to SVG, DAT and Matlab
#  Usage: ./parse_dat.py --help
#  License: GPL-v3
#  (c) Wolf Vollprecht <wolfv@student.ethz.ch>

## TODO
#  Implement remaining formats if needed
#  Investigate .mat usage for matlab (scipy)

# Sublime Text Quick Tip
# Reformat SVG:
# ([LlMmZzCc]) -> $1\n

# TODO: Fill Rule (even - odd?)
# TODO: Connected shapes which are not shape + hole?

import argparse
import pprint
pp = pprint.PrettyPrinter(indent=4)

from bs4 import BeautifulSoup
import re

import numpy as np

from math import *

from numpy import linspace

r = re.compile(r"([a-z])([^a-z]*)", flags=re.IGNORECASE)
css_re = re.compile(r"([\w\-]+):(.+?);")

t = linspace(1/10, 1 - 1/10, num = 8)
t = linspace(0.1, 0.9, 9)

class SVGElement():
    def __init__(self, t, c, prev_el=None, relative_to=None):
        self.element_type = t
        self.coords = list()

        if t == "z":
            return
        if c[0] == '':
            print("Error! First coordinate empty!")
            return
        if prev_el:
            self.coords.append(prev_el.coords[-1])
        if t.islower():
            if (relative_to or t == "C") and len(relative_to.coords):
                rel_coords = relative_to.coords[-1]
            else:
                rel_coords = [0, 0]
            for x in range(0, len(c), 2):
                # print(c)
                self.coords.append([rel_coords[0] + float(c[x]),
                                    rel_coords[1] + float(c[x + 1])])
                if t in ["m"]:
                    rel_coords = self.coords[-1]
        else:
            for x in range(0, len(c), 2):
                self.coords.append([float(c[x]), float(c[x + 1])])

    def __repr__(self):
        return "%s | %r" % (self.element_type, self.coords)
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
        if self.element_type in ["z", "Z"]:
            return
        if self.element_type in ["c", "C"]:
            return SVGElement.bezier(self.coords, t)
        if self.element_type in ["l", "L"]:
            return [self.coords[-1]]
        return self.coords

    @staticmethod
    def parse_path(path, container=list()):
        print(path)
        d = path['d']
        element = dict()
        css = re.findall(css_re, path['style'])
        fill = False
        stroke = False
        stroke_width = 0
        for c in css:
            print(c)
            if c[0].lower() == 'fill'\
                and c[1].lower() not in ["none", "#ffffff", "#fff"]:
                fill = True
            if c[0].lower() == "stroke"\
                and c[1].lower() not in ["none", "#ffffff", "#fff", "#ff0000"]:
                stroke = True;
            if c[0].lower() == "stroke-width":
                stroke_width = float(c[1])

        if stroke and stroke_width:
            element["stroke"] = stroke_width
        else:
            element["stroke"] = -1

        if path.has_attr("fill") and path["fill"].lower() != "none" or fill:
            element['filled'] = True
        else:
            element['filled'] = False
        if d.endswith("z") or d.endswith("Z"):
            element['closed'] = True
        else:
            element['closed'] = False
        if path.has_attr("rake_info"):
            element["rake_info"] = [i for i in path["rake_info"].split(" ")]
        else:
            element["rake_info"] = None

        element["manually_modified"] = False
        if path.has_attr("manually_modified"):
            element["manually_modified"] = (path["manually_modified"].lower() == "true")

        m = re.findall(r, d)
        element['svg_elems'] = list()
        element['holes'] = list()
        curr_el = element['svg_elems']
        i = 0
        if m:
            for match in m:
                if match[0] in ('M', 'm') and i != 0:
                    # This is a hole?
                    print('a hole')
                    curr_el = list()
                    element['holes'].append(curr_el)
                # print("Match: ")
                # print(match)
                coords = match[1]
                coords = coords.lstrip().rstrip()
                coords_list = re.split("[^\d\-.]*", coords)
                #if(coords_list[0] == ''): continue
                if match[0] in ["c", "C"]:
                    for x in range(0, int(len(coords_list)), 6):
                        rel_el = None
                        prev_el = curr_el[-1] if (curr_el and len(curr_el)) else None
                        prev_coords = None
                        if prev_el:
                            rel_el = prev_el
                        # if x / 6 > 0:
                        #     prev_el = 
                        inst_coords = coords_list[x:x+6]
                        inst = SVGElement(match[0], inst_coords, prev_el, rel_el)
                        print (inst)
                        curr_el.append(inst)

                else:
                    # apparently "M 1,2 3,4 5,6" is valid, too :( 
                    rel_el = None
                    prev_el = curr_el[-1] if (curr_el and len(curr_el)) else None
                    prev_coords = None
                    if prev_el:
                        rel_el = prev_el
                    inst_coords = coords_list
                    inst = SVGElement(match[0], inst_coords, prev_el, rel_el)
                    print (inst)
                    curr_el.append(inst)

                # curr_el.append(inst)
                i = i + 1
        # pp.pprint(element)
        container.append(element)
        return container

def signed_area(coords_list):
    l = len(coords_list)
    s = 0
    for i in xrange(len(coords_list) - 1):
        s += (coords_list[i + 1][0] - coords_list[i][0]) * (coords_list[i + 1][1] + coords_list[i][1])
    s += (coords_list[0][0] - coords_list[l - 1][0]) * (coords_list[0][1] + coords_list[l - 1][1])
    return s
def parse_string(svg_string):
    poly = list()
    svg_element_list = list()

    soup = BeautifulSoup(svg_string)
    base = dict()
    base['width'] = float(soup.svg['width'])
    base['height'] = float(soup.svg['height'])

    res = dict()
    res['svg_base'] = base
    res["elements"] = list()
    path = soup.find_all("path")

    for p in path:
        SVGElement.parse_path(p, svg_element_list)

    polys = list()
    pp.pprint(svg_element_list)
    for el in svg_element_list:
        poly = list()
        for e in el["svg_elems"]:
            print(e)

            # print(e.element_type.__repr__() + e.to_poly().__str__())

            if e.element_type in ["z", "Z"]:
                if len(e.coords) == 0:
                    pass
                else:
                    if(e.coords[0] == poly[0][0] and e.coords[1] == poly[0][1]):
                        pass
                    else:
                        poly.extend(tuple(map(tuple, e.to_poly())))
                        del poly[0]
            else:
                poly.extend(tuple(map(tuple, e.to_poly())))
        if el["filled"] and signed_area(poly) > 0:
            print("Signed area  > 0")
            poly.reverse()
        # mark_for_deletion = list()
        # for i in xrange(len(poly) - 1):
        #     if((poly[i][0]-poly[i+1][0]) * (poly[i][0]-poly[i+1][0])
        #         + (poly[i][0]-poly[i+1][0]) * (poly[i][0]-poly[i+1][0]) < 0.001):
        #         mark_for_deletion.append(i)
        # for i in reversed(mark_for_deletion): # delete from the back to not kill enumerator
        #     del poly[i]
        holes = list()
        for hole in el["holes"]:
            hole_poly = list()
            for h in hole:
                if h.element_type == "z":
                    if len(h.coords) == 0:
                        pass
                    else:
                        hole_poly.extend(tuple(map(tuple, h.to_poly())))
                        del hole_poly[0]
                else:
                    hole_poly.extend(tuple(map(tuple, h.to_poly())))
            holes.append(hole_poly)
        #poly.reverse()
        polys.append(poly)
        res["elements"].append({
            "closed": el["closed"],
            "filled": el["filled"],
            "stroke": el["stroke"],
            "manually_modified": el["manually_modified"],
            "stroke_width": el["stroke"],
            "rake_states": el["rake_info"],
            "coords": poly,
            "holes" : holes
            })
    return res

# def scale (c, s):
#     return (c[0] * s, c[1] * s)

# def export_to_timon(e):
#     f = open("export_2.txt", "w+")

#     if(len(el["elements"]) > 1):
#         print "WTF"
#     for i in enumerate(el['coords']):
#         scl = scale (el['coords'][i], s)
#         f.write("{0} {1} {2}\n".format(scl[0], scl[1], rake_info[i])


def parse_file(filename):
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
    xml = f.read()
    f.close()
    return parse_string(xml)

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
