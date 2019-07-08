#ifndef STDPSYNAPSE_H
#define STDPSYNAPSE_H

#include <stdio.h>
#include <list>
#include "../../interface/Synapse.h"

using std::list;

class STDPSynapse : public Synapse {
public:
	STDPSynapse(real weight=0, real delay=1e-4, real aPre=0, real aPost=0, real dPre=0, real dPost=0, real tauSyn=1.0, real tauPre=0, real tauPost=0);
	STDPSynapse(const STDPSynapse &synapse);
	~STDPSynapse();

	virtual Type getType() const override;

	virtual int hardCopy(void * data, int idx, int base, const SimInfo &info) override;

protected:
	const static Type type;
	real _aPre;
	real _aPost;
	real _dPre;
	real _dPost;
	real _tauSyn;
	real _tauPre;
	real _tauPost;
};

#endif /* STDPSYNAPSE_H */
