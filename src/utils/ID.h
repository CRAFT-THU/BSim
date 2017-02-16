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
	char machine_id;
	char type_id;
	short grp_id;
	int id;
};

struct ID {
public:
	ID();
	ID(int id);
	ID(int grp_id, int id);
	ID(short grp_id, int id);
	ID(char type_id, short grp_id, int id);
	ID(char machine_id, char type_id, short grp_id, int id);
	ID(const ID &id);
	~ID();

	void advance();
	void changeID(ID &id);
	void changeNode(char node);
	inline int getID() const
	{ 
		return id.detailed_id.id; 
	}
	inline int getGID() const
	{  
		int grp_id = id.detailed_id.type_id;
		grp_id = grp_id << 16;
		grp_id += id.detailed_id.grp_id;
		return grp_id;
	}
	inline int getNode() const
	{ 
		return id.detailed_id.machine_id; 
	}
	string getInfo() const;

	bool operator==(const ID &id1)const; 
	bool operator<(const ID &id1)const; 

private:
	union Union_Id {
		long long packed_id;
		Detailed_ID detailed_id;
	} id;
};

#endif /* ID_H */

