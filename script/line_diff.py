#!/usr/bin/python

import sys, getopt
import re
import ast

def line_diff(file1="", file2=""):
    infile1 = open(file1, "r+")
    infile2 = open(file2, "r+")
    outfile = open("diff.res", "w+")

    inlines1 = infile1.readlines()
    inlines2 = infile2.readlines()

    diffed = False

    linenum = 0;
    for l0,l1 in zip(inlines1, inlines2):
        line0 = l0.strip()
        line1 = l1.strip()
        string0 = filter(None, re.split(r'\s*[;,:\s]\s*', line0))
        string1 = filter(None, re.split(r'\s*[;,:\s]\s*', line1))
        set0 = set(string0)
        set1 = set(string1)


        outfile.write('Line ' + str(linenum) + ' \t')
        diff0 = set0 - set1
        diff1 = set1 - set0
        outfile.write('+++\t' + str(diff1))
        outfile.write('---\t' + str(diff0))
        outfile.write('\n')

        if (len(diff0) > 0 or len(diff1) > 0):
            diffed = True

        linenum = linenum + 1

    if diffed:
        print("Diff")
    else:
        print("Same")



def main(argv):
    file1 = ''
    file2 = ''

    usuage_msg = sys.argv[0] + ' -1 <file1> -2 <file2>'

    try:
        opts, args = getopt.getopt(argv,"h1:2:",["file1=","file2="])
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
    
    line_diff(file1, file2);


if __name__ == "__main__":
    main(sys.argv[1:])

