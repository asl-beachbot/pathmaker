import numpy as np
import matplotlib.pyplot as plt
import argparse

def main():
    a = []
    with open(ns.filename) as g:
        for line in g:
            if not len(line.strip()) == 0:
                a.append([float(n) for n in line.strip().split(' ')])
    numbers = np.array(a);
    print(numbers)
    plt.axis("equal")
    plt.plot(numbers[:, 0], numbers[:, 1])
    plt.show()

if __name__ == '__main__':
    parser = argparse.ArgumentParser(
             description="Plot the shizzle.")
    parser.add_argument('filename', type=str, help="Filename to plot")
    ns = parser.parse_args()

    main()
