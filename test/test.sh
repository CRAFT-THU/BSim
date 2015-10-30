#!/bin/bash
g++ -o test.out -lm  neural_test.cpp ../LIFNeuron.cpp ../Simulator.cpp ../Connector.cpp ../ExpSyn.cpp ../IF_curr_exp.cpp ../ID.cpp && ./test.out | less
g++ -o test.out -lm  ./synapse_test.cpp ../LIFNeuron.cpp ../Simulator.cpp ../Connector.cpp ../ExpSyn.cpp ../IF_curr_exp.cpp ../ID.cpp && ./test.out | less
