/* This header file is writen by qp09
 * usually just for fun
 * Wed December 02 2015
 */
#ifndef OBJECTBASE_H
#define OBJECTBASE_H

class ObjectBase {
	virtual size_t getSize() = 0;
	virtual size_t hardCopy(void *data) = 0;
};

#endif /* OBJECTBASE_H */

