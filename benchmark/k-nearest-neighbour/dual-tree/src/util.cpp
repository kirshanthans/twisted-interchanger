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
#include "util.h"
#include "common.h"

float max(float a, float b)
{
		return a > b ? a : b;
}

float min(float a, float b)
{
		return a < b ? a : b;
}

void readInput(int argc, char **argv)
{
		FILE *in;

		if(argc != 5 && argc != 4) {
				fprintf(stderr, "usage: pointcorr <DIM> <K> <npoints> [input_file]\n");
				exit(1);
		}
		
		dim = atoi(argv[1]);
		if(dim <= 0) {
				fprintf(stderr, "Invalid DIM\n");
				exit(1);
		}

		knn = atoi(argv[2]);
		if(knn <= 0) {
				fprintf(stderr, "Invalid K\n");
				exit(1);
		}

		npoints = atol(argv[3]);
		if(npoints <= 0) {
				fprintf(stderr, "Not enough points.\n");
				exit(1);
		}

		inPoints = new Point[npoints];

		if(argc == 5) {
				in = fopen(argv[4], "r");
				if(in == NULL) {
						fprintf(stderr, "Could not open %s\n", argv[4]);
						exit(1);
				}

				for(int i = 0; i < npoints; i++) {
						readPoint(in, &inPoints[i]);
						inPoints[i].label = i;
				}
				fclose(in);
		} else {
				srand(0);
				for(int i = 0; i < npoints; i++) {
						for(int j = 0; j < dim; j++) {
								inPoints[i].coord[j] = (float)rand() / RAND_MAX;
						}
						inPoints[i].label = i;
				}
		}
}

void readPoint(FILE *in, Point *p)
{
		int dummy;
		if(fscanf(in, "%d", &dummy) != 1) {
				fprintf(stderr, "Input file not large enough.\n");
				exit(1);
		}
		for(int j = 0; j < dim; j++) {
				if(fscanf(in, "%f", &p->coord[j]) != 1) {
						fprintf(stderr, "Input file not large enough.\n");
						exit(1);
				}
		}
}

#ifdef PERF
void handleError(int retval)
{
		cout << "PAPI Error " << retval << ": " << PAPI_strerror(retval) << endl;
		exit(1);
}
#endif
