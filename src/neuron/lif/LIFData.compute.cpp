
#include "../../utils/runtime.h"
#include "../../net/Connection.h"

#include "LIFData.h"

void updateLIF(Connection *connection, void *_data, real *currentE, real *currentI, int *firedTable, int *firedTableSizes, int num, int offset, int time)
{
	LIFData *data = (LIFData *)_data;
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
				firedTable[firedTableSizes[currentIdx] + cFiredTableCap * currentIdx] = gnid;
				firedTableSizes[currentIdx]++;

				data->pRefracStep[nid] = data->pRefracTime[nid] - 1;
				data->pV_m[nid] = data->pV_reset[nid];
			} else {
				data->pI_e[nid] += currentE[gnid];
				data->pI_i[nid] += currentI[gnid];
			}
	
		} else {
			data->pRefracStep[nid]--;
		}
		currentE[gnid] = 0;
		currentI[gnid] = 0;
	}
}

