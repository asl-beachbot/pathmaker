import numpy as np
import matplotlib.pyplot as plt
import matplotlib.colors
import argparse


filename = ""
def do_plot(fn):
    global filename
    filename = fn
    main()
def main():
    a = []
    with open(filename) as g:
        for line in g:
            if not len(line.strip()) == 0:
                a.append([float(n) for n in line.strip().split(' ')])
    numbers = np.array(a);
    print(numbers)
    plt.axis("equal")
    # plt.hold()
    prev_rake = numbers[0,2];
    i = 0
    plot_arr = list()
    while(i < len(a)):
        if(numbers[i, 2] == prev_rake):
            plot_arr.append(numbers[i, :])
        else:
            np_plot = np.array(plot_arr)
            # print np_plot
            if(len(plot_arr)):
                if(plot_arr[0][2] == 0):
                    c = (0, 0, 0)
                else:
                    c = (1, plot_arr[0][2]/256, 0)
                plt.plot(np_plot[:, 0], np_plot[:, 1], color=c)
                prev_rake = numbers[i,2]
            plot_arr = list()
        i += 1
    np_plot = np.array(plot_arr)
    if(len(plot_arr)):
        if(plot_arr[0][2] == 0):
            c = (0, 0, 0)
        else:
            c = (1, plot_arr[0][2]/256, 0)
        plt.plot(np_plot[:, 0], np_plot[:, 1], color=c)

    plt.show()

if __name__ == '__main__':
    parser = argparse.ArgumentParser(
             description="Plot the shizzle.")
    parser.add_argument('filename', type=str, help="Filename to plot")
    ns = parser.parse_args()
    global filename
    filename = ns.filename
    main()
