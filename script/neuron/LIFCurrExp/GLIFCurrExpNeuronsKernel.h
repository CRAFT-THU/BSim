#ifndef GLIFCURREXPNEURONSKERNEL_H
#define GLIFCURREXPNEURONSKERNEL_H

#include "GLIFCurrExpNeurons.h"

__global void find_lifcurrexp_neurons(GLIFCurrExpNeurons *d_neurons, int num, int start_id);
__global void update_lifcurrexp_neurons(GLIFCurrExpNeurons *d_neurons, int num, int start_id);
#endif /* LIFCURREXPNEURONS_H */
