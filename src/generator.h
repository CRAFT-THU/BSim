/* This header file is writen by qp09
 * usually just for fun
 * Sun January 17 2016
 */
#ifndef GENERATOR_H
#define GENERATOR_H

void balanced(int populationNum, double connectionRate, double exciteRate, int minSize, double minWeight, double minDelay, int sizeRange = 0, double weightRange = 0, double delayRange = 0);
void unbalanced(int populationNum, double connectionRate, double exciteRate, int minSize, double minWeight, double minDelay, int sizeRange = 0, double weightRange = 0, double delayRange = 0);

#endif /* GENERATOR_H */

