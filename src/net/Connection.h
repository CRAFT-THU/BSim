/* This header file is writen by qp09
 * usually just for fun
 * Thu December 29 2016
 */
#ifndef CONNECTION_H
#define CONNECTION_H

struct N2SConnection {
	//int *pSynapsesIdx; 
	//int synapsesNum; 
	int *delayStart;
	int *delayNum;
	int *rev_delayStart;
	int *rev_delayNum;
	int *rev_map2sid;

	int n_num;
	int s_num;
};

#endif /* CONNECTION_H */

