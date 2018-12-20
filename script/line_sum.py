#!/usr/bin/python

import sys, getopt
import re
import ast

def line_sum(file1=""):
    infile1 = open(file1, "r+")
    outfile = open("sum.res", "w+")

    inlines1 = infile1.readlines()
    for l0 in inlines1:
        line0 = l0.strip()
        string0 = filter(None, re.split(r'\s*[;,:\s]\s*', line0))
        data0 = map(ast.literal_eval, string0)

        sum_res = 0
        for i in data0:
            sum_res = sum_res + i
        outfile.write(str(sum_res))
        outfile.write('\n')



def main(argv):
    file1 = ''

    usuage_msg = sys.argv[0] + ' -f <file>'

    try:
        opts, args = getopt.getopt(argv,"hf:",["file="])
    except getopt.GetoptError:
        print usuage_msg
        sys.exit(2)
    for opt, arg in opts:
        if opt == '-h':
            print usuage_msg
            sys.exit()
        elif opt in ("-f", "--file"):
            file1 = arg
    
    line_sum(file1);


if __name__ == "__main__":
    main(sys.argv[1:])

