/* This header file is writen by qp09
 * usually just for fun
 * Wed October 28 2015
 */
#ifndef ID_H
#define ID_H

#include <string>
#include "constant.h"

using std::string;

struct Detailed_ID {
	int grp_id;
	int id;
};

struct ID {
public:
	ID();
	ID(int id);
	ID(int grp_id, int id);
	ID(char machine_id, int grp_id, int id);
	ID(char machine_id, char type_id, short grp_id, int id);
	ID(ID &id);
	ID(const ID &id);
	~ID();

	void advance();
	void setID(ID &id);
	void setNode(char node);
	inline int getId() const
	{ 
		return _id.detailed_id.id; 
	}
	inline int getGid() const
	{  
		return _id.detailed_id.grp_id;
	}

	string getInfo() const;

	bool operator==(const ID &id1)const; 
	bool operator<(const ID &id1)const; 

private:
	union Union_Id {
		long long packed_id;
		Detailed_ID detailed_id;
	} _id;
};

#endif /* ID_H */

