#!/usr/bin/python3

## Parser application
#  Parses Polygon data from SVG, DAT and Matlab to SVG, DAT and Matlab
#  Usage: ./parse_dat.py --help
#  License: GPL-v3
#  (c) Wolf Vollprecht <wolfv@student.ethz.ch>

## TODO
#  Implement remaining formats if needed
#  Investigate .mat usage for matlab (scipy)


import sys
import argparse 

from bs4 import BeautifulSoup
import re

from math import *

r = re.compile(r"([a-z])([^a-z]*)", flags=re.IGNORECASE)

parser = argparse.ArgumentParser(description="Convert SVG, DAT and MAT input and output.")
parser.add_argument('--to', '-t', metavar="format", type=str, help="Filetype to convert to: svg, mat, dat")
parser.add_argument('filename', type=str, help="Filename to convert")

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
                print(c)
                self.coords.append([rel_coords[0] + float(c[x]), rel_coords[1] + float(c[x + 1])])
                print(self.element_type, self.coords)
        else:
            print(self.element_type, self.coords)

            for x in range(0, len(c), 2):
                self.coords.append([float(c[x]), float(c[x + 1])])


def run():
    ns = parser.parse_args()
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
        def parse_path(path):
            d = path['d']
            m = re.findall(r, d)
            if m:
                print(m)
                for match in m:
                    coords = match[1]
                    coords = coords.lstrip()
                    coords = coords.rstrip()
                    coords_list = re.split("[^\d\-.]*", coords)
                    if(coords_list[0] == ''): continue
                    for x in range(0, int(len(coords_list)), 6):
                        inst_coords = coords_list[x:x+6]
                        inst = SVGElement(match[0], inst_coords )
                    svg_element_list.append(inst)

        xml = f.read()
        f.close()
        soup = BeautifulSoup(xml)
        base = dict()
        base['w'] = soup.svg.width
        base['h'] = soup.svg.height
        
        path = soup.path
        parse_path(path)

        for el in svg_element_list:
            poly.append([el.coords[0][0], el.coords[0][1]])
        print(poly)
    if ns.to == "mat":
        text = "poly = [";
        for p2 in poly:
            text += "%s, %s;\n" % (p2[0], p2[1])
        text += "];"
        name = ns.filename.split(".")[0]
        name += ".m"
        fw = open(name, 'w')
        fw.write(text)
   
    if ns.to == "dat":
        text = "%s\n" % len(poly);
        for p2 in poly:
            text += "%s %s\n" % (p2[0], p2[1])
        text = text[:   -1]
        print(text)
        name = ns.filename.split(".")[0]
        name += ".dat"
        fw = open(name, 'w')
        fw.write(text)


if __name__ == '__main__':
    run()
