#!/usr/bin/python

import sys, getopt

def column_merge(input=[], output=""):
    if (not input):
        return

    files = []
    outfile = open(output, "w+")
    for name in input:
        f = open(name, "r")
        files.append(f)

    f0 = files[0]
    files = files[1:]

    for l0 in f0:
        outfile.write(l0.strip("\n"))
        for f in files:
            line = f.readline()
            outfile.write(line.strip("\n"))
        outfile.write("\n")



def main(argv):
    inputfile = []
    outputfile = ''
    try:
        opts, args = getopt.getopt(argv,"hi:o:",["ifile=","ofile="])
    except getopt.GetoptError:
        print 'test.py -i <inputfile1> -i <inputfile2> ... -i <inputfileN> -o <outputfile>'
        sys.exit(2)
    for opt, arg in opts:
        if opt == '-h':
            print 'test.py -i <inputfile> -o <outputfile>'
            sys.exit()
        elif opt in ("-i", "--ifile"):
            inputfile.append(arg)
        elif opt in ("-o", "--ofile"):
            outputfile = arg
    
    print 'Column Merge: ' + ' '.join(str(e) for e in inputfile) + " to " + str(outputfile) 
    column_merge(inputfile, outputfile);


if __name__ == "__main__":
    main(sys.argv[1:])

