#!/usr/bin/python

import sys, getopt
import re
import ast
import numpy as np

def column_sub(file1="", file2=""):
    data1 = np.loadtxt(file1)
    data2 = np.loadtxt(file2)

    diff = data1 - data2

    print (np.max(np.abs(diff)))

def main(argv):
    file1 = ''
    file2 = ''

    usuage_msg = sys.argv[0] + ' -1 <file1> -2 <file2>'

    try:
        opts, args = getopt.getopt(argv,"h1:2:",["ifile=","ofile="])
    except getopt.GetoptError:
        print(usuage_msg)
        sys.exit(2)
    for opt, arg in opts:
        if opt == '-h':
            print(usuage_msg)
            sys.exit()
        elif opt in ("-1", "--file1"):
            file1 = arg
        elif opt in ("-2", "--file2"):
            file2 = arg
    
    column_sub(file1, file2);


if __name__ == "__main__":
    main(sys.argv[1:])

