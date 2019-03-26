#!/usr/bin/python

import sys, getopt
import re
import ast

def column_sub(file1="", file2=""):
    infile1 = open(file1, "r+")
    infile2 = open(file2, "r+")
    outfile = open("sub.res", "w+")

    inlines1 = infile1.readlines()
    inlines2 = infile2.readlines()
    for l0,l1 in zip(inlines1, inlines2):
        line0 = l0.strip()
        line1 = l1.strip()
        string0 = filter(None, re.split(r'\s*[;,:\s]\s*', line0))
        string1 = filter(None, re.split(r'\s*[;,:\s]\s*', line1))
        data0 = list(map(ast.literal_eval, string0))
        data1 = list(map(ast.literal_eval, string1))

        assert len(data0)==len(data1)

        for i,j in zip(data0, data1):
            diff = i-j
            outfile.write(str(diff) + ' \t')
        outfile.write('\n')



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

