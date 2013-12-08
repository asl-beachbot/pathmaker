#/bin/python3

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
    for line in f:
        print(line)
if __name__ == '__main__':
    run()