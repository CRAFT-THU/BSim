
#include <assert.h>
#include <math.h>

#include "../../third_party/json/json.h"

#include "STDPSynapse.h"
#include "STDPData.h"

const Type STDPSynapse::type = STDP;

STDPSynapse::STDPSynapse(real weight, real delay, real aPre, real aPost, real dPre, real dPost, real tauSyn, real tauPre, real tauPost)
	: Synapse(0, weight, delay), _aPre(aPre), _aPost(aPost), _dPre(dPre), _dPost(dPost), _tauSyn(tauSyn), _tauPre(tauPre), _tauPost(tauPost)
{
}

STDPSynapse::STDPSynapse(const STDPSynapse &synapse) : Synapse()
{
	this->_weight = synapse._weight;
	this->_delay = synapse._delay;
	this->_aPre = synapse._aPre;
	this->_aPost = synapse._aPost;
	this->_dPre = synapse._dPre;
	this->_dPost = synapse._dPost;
	this->_tauSyn = synapse._tauSyn;
	this->_tauPre= synapse._tauPre;
	this->_tauPost = synapse._tauPost;
}

STDPSynapse::~STDPSynapse()
{
}

Type STDPSynapse::getType() const
{
	return type;
}

int STDPSynapse::hardCopy(void * data, int idx, int base, const SimInfo &info)
{
	STDPData *p = (STDPData *) data;

	real dt = info.dt;
	int delaySteps = static_cast<int>(round(_delay/dt));
	real weight = this->_weight;
	assert(fabs(_tauSyn) > ZERO);
	if (fabs(_tauSyn) > ZERO) {
		real c1 = exp(-(_delay-dt*delaySteps)/_tauSyn);
		weight = weight * c1;
	}

	setID(idx+base);
	p->pWeight[idx] = weight;
	p->pAPre[idx] = this->_aPre;
	p->pAPost[idx] = this->_aPost;
	p->pDPre[idx] = this->_dPre;
	p->pDPost[idx] = this->_dPost;
	// p->pTauSyn[idx] = this->_tauSyn;
	p->pTauPre[idx] = this->_tauPre;
	p->pTauPost[idx] = this->_tauPost;
	p->pDst[idx] = this->getDst()->getID();

	return 1;
}

