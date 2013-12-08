#/bin/python3

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

parser = argparse.ArgumentParser(description="Convert SVG, DAT and MAT input and output.")
parser.add_argument('--to', '-t', metavar="format", type=str, help="Filetype to convert to: svg, mat, dat")
parser.add_argument('filename', type=str, help="Filename to convert")

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

    firstline = False
    poly = list();
    for line in f:
        if " " in line:
            (x, y) = line.rstrip("\n").split(" ")
            poly.append([x, y])
        else:
            if firstline:
                break
            firstline = True
    if ns.to == "mat":
        text = "poly = [";
        for p2 in poly:
            text += "%s, %s;\n" % (p2[0], p2[1])
        text += "];"
        name = ns.filename.split(".")[0]
        name += ".m"
        fw = open(name, 'w')
        fw.write(text)


if __name__ == '__main__':
    run()
