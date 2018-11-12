#!/usr/bin/python

import os
import re
import sys, getopt

import matplotlib
matplotlib.use('agg')
import matplotlib.pyplot as plt

def draw_point(filename):
    input=open(filename,"r")
    fr=input.readlines()
    neu=[]
    time=[]

    for i in range(len(fr)):
        if fr[i] != '\n' :
	    who=fr[i].split(' ')
	    for person in range(len(who)-1):
	        k=int(who[person])
		time.append(i)
		neu.append(k)
    plt.plot(time, neu, '.k')
    plt.xlabel('Cycle')
    plt.ylabel('Neuron index')
    plt.savefig("fire_rate.png",format="png")  


def main(argv):
    inputfile = ''

    usuage_msg = sys.argv[0] + '-i <inputfile1>'

    try:
        opts, args = getopt.getopt(argv,"hi:",["ifile="])
    except getopt.GetoptError:
        print usuage_msg
        sys.exit(2)
    for opt, arg in opts:
        if opt == '-h':
            print usuage_msg 
            sys.exit()
        elif opt in ("-i", "--ifile"):
            inputfile = arg
    
    draw_point(inputfile)

if __name__ == "__main__":
    main(sys.argv[1:])

