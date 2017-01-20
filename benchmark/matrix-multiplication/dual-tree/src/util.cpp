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

		if(argc != 2) {
				fprintf(stderr, "usage: pointcorr <DIM>\n");
				exit(1);
		}

		dim = atoi(argv[1]);
		if(dim <= 0) {
				fprintf(stderr, "Invalid DIM\n");
				exit(1);
		}

		row = new Point[dim];
		col = new Point[dim];
		out = (float*)malloc(sizeof(float) * dim * dim);

		for(int i = 0; i < dim; i++) {
				readPoint(&row[i]);
				row[i].label = i;
				readPoint(&col[i]);
				col[i].label = i;
		}
}

void readPoint(Point *p)
{
		for(int i = 0; i < dim; i++) {
				p->coord[i] = 1.0;
		}
}

#ifdef PERF
void handleError(int retval)
{
		cout << "PAPI Error " << retval << ": " << PAPI_strerror(retval) << endl;
		exit(1);
}
#endif
