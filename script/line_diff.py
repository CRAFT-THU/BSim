#!/usr/bin/python

import sys, getopt
import re
import ast

def column_sub(file1="", file2=""):
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
        data0 = map(ast.literal_eval, string0)
        data1 = map(ast.literal_eval, string1)

        assert len(data0)==len(data1)

        outfile.write('Line ' + str(linenum) + ' \t')
        columnum = 0
        for i,j in zip(data0, data1):
            diff = i-j
            if abs(diff) > 1e-10:
                diffed = True
                outfile.write(str(columnum) + ':' + str(diff) + ' \t');
            columnum = columnum + 1
        outfile.write('\n')
        linenum = linenum + 1

    if diffed:
        print "Diff"
    else:
        print "Same"



def main(argv):
    inputfile = ''
    outputfile = ''
    try:
        opts, args = getopt.getopt(argv,"hi:o:",["ifile=","ofile="])
    except getopt.GetoptError:
        print 'test.py -i <file1> -o <file2>'
        sys.exit(2)
    for opt, arg in opts:
        if opt == '-h':
            print 'test.py -i <file1> -o <file2>'
            sys.exit()
        elif opt in ("-i", "--ifile"):
            inputfile = arg
        elif opt in ("-o", "--ofile"):
            outputfile = arg
    
    column_sub(inputfile, outputfile);


if __name__ == "__main__":
    main(sys.argv[1:])

