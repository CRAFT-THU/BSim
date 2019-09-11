
#include "LIFData.h"

#include "../../utils/runtime.h"
#include "../../net/Connection.h"

void updateLIF(Connection *connection, LIFData *data, real *currentE, real *currentI, int *firedTable, int *firedTableSizes, int num, int offset, int time)
{
	int currentIdx = time % (connection->maxDelay+1);
	for (int nid=0; nid<num; nid++) {
		int gnid = offset + nid; 

		if (data->pRefracStep[nid] <= 0) {
			data->pV_m[nid] = data->pC_m[nid] * data->pV_m[nid] + data->pV_tmp[nid] + data->pI_e[nid] * data->pC_e[nid] + data->pI_i[nid] * data->pC_i[nid];

			//data->p_i_syn[nid] = 0;

			data->pI_e[nid] *= data->pCe[nid];
			data->pI_i[nid] *= data->pCi[nid];

			bool fired = data->pV_m[nid] >= data->pV_thresh[nid];

			if (fired) {
				firedTable[firedTableSizes[currentIdx] + gFiredTableCap * currentIdx] = gnid;
				firedTableSizes[currentIdx]++;

				data->pRefracStep[nid] = data->pRefracTime[nid] - 1;
				data->pV_m[nid] = data->pV_reset[nid];
			} else {
				data->pI_e[nid] += currentE[gnid];
				data->pI_i[nid] += currentI[gnid];
			}
	
		} else {
			currentE[gnid] = 0;
			currentI[gnid] = 0;
			data->pRefracStep[nid]--;
		}
	}
}

