#!/usr/bin/python

import sys, getopt
import re
import ast

def line_sort(input_name="i.txt", output_name="o.txt"):
    infile = open(input_name, "r")
    outfile = open(output_name, "w+")
    for l in infile.readlines():
        line = l.strip()
        strings = filter(None, re.split(r'\s*[;,:\s]\s*', line))
        #print strings
        if (strings):
            data = map(ast.literal_eval, strings)
            data = sorted(data)
            strings = map(str, data)
            outfile.write(' '.join(strings) + ' ');
        outfile.write('\n');



def main(argv):
    inputfile = '' 
    outputfile = ''
    try:
        opts, args = getopt.getopt(argv,"hi:o:",["ifile=","ofile="])
    except getopt.GetoptError:
        print(sys.argv[0] + ' -i <inputfile> -o <outputfile>')
        sys.exit(2)
    for opt, arg in opts:
        if opt == '-h':
            print(sys.argv[0] + ' -i <inputfile> -o <outputfile>')
            sys.exit()
        elif opt in ("-i", "--ifile"):
            inputfile = arg
        elif opt in ("-o", "--ofile"):
            outputfile = arg
    
    line_sort(inputfile, outputfile);


if __name__ == "__main__":
    main(sys.argv[1:])

