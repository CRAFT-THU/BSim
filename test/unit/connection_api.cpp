/* This program is writen by qp09.
 * usually just for fun.
 * Tue December 15 2015
 */

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "../../include/BSim.h"
#include "../../src/net/GNetwork.h"

using std::vector;
using ::testing::AtLeast;
using ::testing::ElementsAreArray;


TEST(ConnectionAPITest, SynapseTemplTest) {
	Network c0, c1;
	Population *pn0 = c0.createPopulation(2, LIF_curr_exp(LIFNeuron(1.0, 1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.7, 1.8, 1.9), 1.6, 1.7));
	Population *pn1 = c0.createPopulation(3, LIF_curr_exp(LIFNeuron(2.0, 2.1, 2.2, 2.3, 2.4, 2.5, 2.6, 2.7, 2.8, 2.9), 2.6, 2.7));
	Population *pn2 = c0.createPopulation(1, LIF_curr_exp(LIFNeuron(3.0, 3.1, 3.2, 3.3, 3.4, 3.5, 3.6, 3.7, 3.8, 3.9), 3.6, 3.7));

	Population *pn0_1 = c1.createPopulation(2, LIF_curr_exp(LIFNeuron(1.0, 1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.7, 1.8, 1.9), 1.6, 1.7));
	Population *pn1_1 = c1.createPopulation(3, LIF_curr_exp(LIFNeuron(2.0, 2.1, 2.2, 2.3, 2.4, 2.5, 2.6, 2.7, 2.8, 2.9), 2.6, 2.7));
	Population *pn2_1 = c1.createPopulation(1, LIF_curr_exp(LIFNeuron(3.0, 3.1, 3.2, 3.3, 3.4, 3.5, 3.6, 3.7, 3.8, 3.9), 3.6, 3.7));

	real weight0[] = {1.0, 1.1, 1.2, 1.3, 1.4, 1.5};
	real weight1[] = {2.0, 2.1, 2.2};
	real delay0[] = {1e-4, 2e-4, 1e-4, 2e-4, 1e-4, 3e-4};
	real delay1[] = {1e-4, 2e-4, 3e-4};

	StaticSynapse *s0 = new StaticSynapse[6];
	StaticSynapse *s1 = new StaticSynapse[3];

	for (int i=0; i<6; i++) {
		s0[i] = StaticSynapse(weight0[i], delay0[i], 1.6);
	}

	for (int i=0; i<3; i++) {
		s1[i] = StaticSynapse(weight1[i], delay1[i], 2.6);
	}


	//Network.connect(population1, population2, weight_array, delay_array, Exec or Inhi array, num)
	c0.connect(pn0, pn1, weight0, delay0, NULL, 6);
	c0.connect(pn1, pn2, weight1, delay1, NULL, 3);

	c1.connect(pn0_1, pn1_1, s0, 6);
	c1.connect(pn1_1, pn2_1, s1, 3);


	// SGSim sg(&c, 1.0e-4);

	DistriNetwork *n0 = c0.buildNetworks(SimInfo(1.0e-4));
	DistriNetwork *n1 = c1.buildNetworks(SimInfo(1.0e-4));

	ASSERT_TRUE(isEqualNetwork(n0->_network, n1->_network));

}

int main(int argc, char **argv)
{
	::testing::InitGoogleMock(&argc, argv);
	return RUN_ALL_TESTS();
} 
