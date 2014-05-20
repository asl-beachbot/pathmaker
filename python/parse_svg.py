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
import copy
import svg
pp = pprint.PrettyPrinter(indent=4)

from bs4 import BeautifulSoup
import re

import numpy as np

from math import *

from numpy import linspace

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
            "startpoint": el["startpoint"],
            "manually_modified": el["manually_modified"],
            "stroke_width": el["stroke"],
            "rake_states": el["rake_info"],
            "coords": poly,
            "holes" : holes
            })
    return res

def new_parse_file(filename):
    try:
        f = open(filename, 'r')
    except:
        print("Illegal Filename")
        return False
    res = dict()
    s = svg.parse(filename)
    # for i in s.items:
    #     print(i)
    #     print(i.items)
    #     if hasattr(i, "style"): print(i.style)
    a,b = s.bbox()


    width, height = (a+b).coord()

    base = dict()
    base['width'] = float(width)
    base['height'] = float(height)
    base["rake_sizes"] = s.root.get("rake_sizes")
    if(s.root.get("scale_to_disp")):
        base['scale_for_disp'] = float(s.root.get("scale_for_disp"))
    else:
        base['scale_for_disp'] = None
    res = dict()
    res['svg_base'] = base
    res["elements"] = list()

    for i in s.flatten():
        if hasattr(i, "segments"):
            stroke = False
            startpoint = False
            stroke_width = 0
            if(i.get_style("stroke-width") and i.get_style("stroke-width").val > 0 and i.get_style("stroke")):
                if i.get_style("stroke").val not in ["none", "#ffffff", "#fff", "#ff0000", "#00ff00"]:
                    stroke = True;
                    stroke_width = i.get_style("stroke-width").val
                if i.get_style("stroke").val == "#00ff00":
                    startpoint = True
            coords = i.segments(precision=100)
            print("coords0", isinstance(coords[0], list), coords[0])
            if isinstance(coords[0], list):
                print("First elem list!")

                for l_coords in coords:
                    if i.closed and signed_area(l_coords) > 0: 
                        print ("signed area  > 0")
                        l_coords.reverse()

                    print("LCOORDS: ", l_coords)
                    res["elements"].append({
                        "closed": i.closed,
                        "filled": i.get_style("fill") is not None if i.get_style("fill") else False,
                        "stroke": stroke,
                        "startpoint": startpoint,
                        "manually_modified": "manually_modified" in i.attributes,
                        "stroke_width": stroke_width,
                        "rake_states": [j for j in i.attributes.get("rake_info").split(" ")] if i.attributes.get("rake_info") else None,
                        "coords": l_coords,
                        "holes" : list()
                        })
            else:
                if i.closed and signed_area(coords) > 0: 
                    print ("signed area  > 0")
                    coords.reverse()

                res["elements"].append({
                    "closed": i.closed,
                    "filled": i.get_style("fill") is not None if i.get_style("fill") else False,
                    "stroke": stroke,
                    "startpoint": startpoint,
                    "manually_modified": "manually_modified" in i.attributes,
                    "stroke_width": stroke_width,
                    "rake_states": [j for j in i.attributes.get("rake_info").split(" ")] if i.attributes.get("rake_info") else None,
                    "coords": coords,
                    "holes" : list()
                    })

    print(res)
    return res

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
    return new_parse_file(filename)
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
    res = dict()
    s = svg.parse(ns.filename)
    # for i in s.items:
    #     print(i)
    #     print(i.items)
    #     if hasattr(i, "style"): print(i.style)
    res["elements"] = list()
    sres = "<svg><path d=\"M 0,0 "
    for i in s.flatten():
        if hasattr(i, "segments"):
            stroke = False
            startpoint = False
            coords = i.segments(precision=4)
            if isinstance(coords[0], list):
                for l in coords:
                    for p in l:
                        sres += "L {0} {1}".format(p[0], p[1])
            else:
                for p in coords:
                    sres += "L {0} {1}".format(p[0], p[1])
    sres += "\"/></svg>"
    # print (sres)
    return



    #         if(i.get_style("stroke-width") and i.get_style("stroke-width").val > 0 and i.get_style("stroke")):
    #             if i.get_style("stroke").val not in ["none", "#ffffff", "#fff", "#ff0000", "#00ff00"]:
    #                 stroke = True;
    #                 stroke_width = i.get_style("stroke-width").val
    #             if i.get_style("stroke").val == "#00ff00":
    #                 startpoint = true

    #         res["elements"].append({
    #             "closed": i.closed,
    #             "filled": i.get_style("fill") is not None if i.get_style("fill") else False,
    #             "stroke": stroke,
    #             "startpoint": startpoint,
    #             "manually_modified": "manually_modified" in i.attributes,
    #             "stroke_width": stroke,
    #             "rake_states": [j for j in i.attributes.get("rake_info").split(" ")] if i.attributes.get("rake_info") else None,
    #             "coords": i.segments(precision=4)[0],
    #             "holes" : None
    #             })

    # print(res)
    return
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

        parse_file(ns.filename)
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
