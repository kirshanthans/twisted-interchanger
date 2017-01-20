/* Copyright (c) 2017, Kirshanthan Sundararajah, Laith Sakka and Milind Kulkarni 
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the Purdue University nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. */
#include "main.h"
#include "common.h"
#ifdef PERF
#define NUM 8
#endif
//Globals
vector<Point*> inPoints;
int dim;
int knn;
int npoints;
long long nNodes;
long long nWorkNodes;

int main(int argc, char **argv) 
{
		nNodes = 0;
		nWorkNodes = 0;
		readInput(argc, argv);
#ifdef DEBUG
		cout << "dim " << dim << endl;
#endif

#ifdef PERF
		string instance("Original");
		int ret;
		int events[] = {PAPI_TOT_CYC, PAPI_TOT_INS, PAPI_L2_DCA, PAPI_L2_DCM, PAPI_L2_ICA, PAPI_L2_ICM, PAPI_L3_TCA, PAPI_L3_TCM};
		string defs[] = {"Cycle Count", "Instruction Count", "L2 Data Access Count", "L2 Data Miss Count", "L2 Instruction Access Count", "L2 Instruction Miss Count", "L3 Total Access Count", "L3 Total Miss Count"};
		long long values[NUM];
		long long rcyc0, rcyc1, rusec0, rusec1;
		long long vcyc0, vcyc1, vusec0, vusec1;

		ret = PAPI_library_init(PAPI_VER_CURRENT); 
		if (ret != PAPI_VER_CURRENT){ 
				cerr << "PAPI Init Error" << endl;
				exit(1);
		}
		for(int i=0; i<NUM; ++i){
				ret = PAPI_query_event(events[i]);
				if(ret != PAPI_OK){
						cerr << "PAPI Event " << i << " does not exist" << endl;
						handleError(ret);
				}
		}
#endif

		root1 = buildTrees(inPoints, 0, npoints, 0);
		root1->isRoot = true;

#ifdef DEBUG
		cout << "Tree Depth " << tree_depth << " Levels" << endl;
#endif

#ifdef PERF
		//Performance Counters Start
		rcyc0  = PAPI_get_real_cyc();
		rusec0 = PAPI_get_real_usec();

		vcyc0  = PAPI_get_virt_cyc();
		vusec0 = PAPI_get_virt_usec();

		ret = PAPI_start_counters(events, NUM);
		if (ret != PAPI_OK){
				cerr << "PAPI Error starting counters" << endl;
				handleError(ret);
		}
#endif
		recurse1(root1, root1);
#ifdef PERF
		//Performance Counters Read
		ret = PAPI_read_counters(values, NUM);
		if (ret != PAPI_OK){
				cerr << "PAPI Error reading counters" << endl;
				handleError(ret);
		}

		rcyc1  = PAPI_get_real_cyc();
		rusec1 = PAPI_get_real_usec();

		vcyc1  = PAPI_get_virt_cyc();
		vusec1 = PAPI_get_virt_usec();
#endif

#ifdef PERF
		//Performance Counters Printed
		cout << "Performance Counters" << endl;
		for(int i=0; i<NUM; ++i){
				cout << instance << " " << defs[i] << ": " << values[i] << endl;
		}
		cout << endl;
		cout << instance << " Wall Clock Cycles: "  << rcyc1-rcyc0 << endl;
		cout << instance << " Wall Clock Time(s): " << (rusec1-rusec0)/1000000.0 << endl;
		cout << endl;
		cout << instance << " Virtual Clock Cycles: "  << vcyc1-vcyc0 << endl;
		cout << instance << " Virtual Clock Time(s): " << (vusec1-vusec0)/1000000.0 << endl;
#endif

#ifdef PERF
		cout << instance << " Work: " << nWorkNodes << endl;
#endif

		long long sum = 0;
		for (int i = 0; i < npoints; i++) {
				Point *p;
				while(inPoints[i]->size()>0){
						p = inPoints[i]->top();
						inPoints[i]->pop();
				}
				if (pointEqual(p, inPoints[i])) sum++;
		}

#ifdef DEBUG	
		cout << "sum " << sum << endl;
		cout << "correct rate: " << (float)sum / npoints << endl;
		cout << "successors of root " << root1->succnum << endl;
		cout << "traversed nodes " << nNodes << endl;
		cout << "worked nodes " << nWorkNodes << endl;
#endif

#if !(defined(DEBUG) || defined(PERF))
		cout << nNodes << endl;
#endif

		for (vector<Point*>::iterator it = inPoints.begin() ; it != inPoints.end(); ++it)
		{
				delete (*it);
		} 
		inPoints.clear();
		destroyTrees(root1);
		return 0;
}
