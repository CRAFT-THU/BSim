/* This header file is writen by qp09
 * usually just for fun
 * Fri December 11 2015
 */
#ifndef IDPOOL_H
#define IDPOOL_H

#include "ID.h"

class IDPool {
public:
	IDPool(int gid = -1);
	~IDPool();
	ID getID();
private:
	int _gid;
	int _id;
};

extern IDPool nidPool;
extern IDPool sidPool;
extern IDPool sendTag;
extern IDPool recvTag;

#endif /* IDPOOL_H */

