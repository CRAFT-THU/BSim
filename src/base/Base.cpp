/* This program is writen by qp09.
 * usually just for fun.
 * Tue April 05 2016
 */

#include "Base.h"

Base::Base(ID id, int node) {
	this->_id = id;
	this->_node = node;
}

Base::~Base()
{
}

//ID Base::getID()
//{
//	return _id;
//}
//
//size_t Base::getIdx()
//{
//	return _idx;
//}
//
//int Base::getNode()
//{
//	return _node;
//}
//
//void Base::setID(ID id)
//{
//	this->_id = id;
//}
//
//void Base::setIdx(size_t idx)
//{
//	this->_idx = idx;
//}
//
//void Base::setNode(int node) 
//{
//	this->_node = node;
//}
