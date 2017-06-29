/* This header file is writen by qp09
 * usually just for fun
 * Fri December 11 2015
 */
#ifndef TAGPOOL_H
#define TAGPOOL_H

//#include "ID.h"

class TagPool {
public:
	TagPool();
	~TagPool();
	int getTag();
private:
	unsigned int count;
};

extern TagPool nidTag;
extern TagPool sidTag;
extern TagPool sendTag;
extern TagPool recvTag;

#endif /* IDPOOL_H */

