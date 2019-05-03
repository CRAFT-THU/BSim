/* This program is writen by qp09.
 * usually just for fun.
 * Tue December 15 2015
 */

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "../../include/BSim.h"

using ::testing::AtLeast;

GNetwork *net = NULL;

// TEST(NetworkTest, NeuronTest) {
// 	GLIFNeurons *n = (GLIFNeurons*)net->ppNeurons[0];
// }

TEST(NetworkTest, SynapseTest) {
	GStaticSynapses *s = (GStaticSynapses*)net->ppSynapses[0];
	ASSERT_THAT(std::vector<real>(s->pWeight, s->pWeight + 9), ::testing::ElementsAreArray({1.0, 1.1, 1.2, 1.3, 1.4, 1.5, 2.0, 2.1, 2.2}));
	ASSERT_THAT(std::vector<int>(s->pDst, s->pDst + 9), ::testing::ElementsAreArray({2, 3, 4, 2, 3, 4, 5, 5, 5, 0, 0, 0}));
}
 
// TEST(NetworkTest, ConnectionTest) {
// 	Connection *c = net->pConnection;
// }


int main(int argc, char **argv)
{
	Network c;
	Population<LIF_curr_exp> *pn0 = c.createPopulation(2, LIF_curr_exp(LIFNeuron(0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0), 6.0, 7.0));
	Population<LIF_curr_exp> *pn1 = c.createPopulation(3, LIF_curr_exp(LIFNeuron(0.2, 1.2, 2.2, 3.2, 4.2, 5.2, 6.2, 7.2, 8.2, 9.2), 6.2, 7.2));
	Population<LIF_curr_exp> *pn2 = c.createPopulation(1, LIF_curr_exp(LIFNeuron(0.4, 1.4, 2.4, 3.4, 4.4, 5.4, 6.4, 7.4, 8.4, 9.4), 6.4, 7.4));

	real weight0[] = {1.0, 1.1, 1.2, 1.3, 1.4, 1.5};
	real weight1[] = {2.0, 2.1, 2.2};
	real delay0[] = {1e-4, 2e-4, 1e-4, 2e-4, 1e-4, 3e-4};
	real delay1[] = {1e-4, 2e-4, 3e-4};

	//Network.connect(population1, population2, weight_array, delay_array, Exec or Inhi array, num)
	c.connect(pn0, pn1, weight0, delay0, NULL, 6);
	c.connect(pn1, pn2, weight1, delay1, NULL, 3);

	SGSim sg(&c, 1.0e-4);

	net = sg.getNet();

	::testing::InitGoogleMock(&argc, argv);
	return RUN_ALL_TESTS();
} 
