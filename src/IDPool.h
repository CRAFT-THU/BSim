/* This header file is writen by qp09
 * usually just for fun
 * Fri December 11 2015
 */
#ifndef IDPOOL_H
#define IDPOOL_H

#include "ID.h"

class IDPool {
public:
	IDPool();
	~IDPool();
	ID getID();
private:
	ID id;
};

extern IDPool nidPool;
extern IDPool sidPool;

#endif /* IDPOOL_H */

