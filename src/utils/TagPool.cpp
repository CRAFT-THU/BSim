/* This program is writen by qp09.
 * usually just for fun.
 * Fri December 11 2015
 */

#include "TagPool.h"

TagPool sidTag;
TagPool nidTag;
TagPool sendTag;
TagPool recvTag;

TagPool::TagPool()
{
	count = 0;
}

int TagPool::getTag()
{
	return count++;
}

TagPool::~TagPool()
{
}
