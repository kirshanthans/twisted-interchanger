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
#ifndef TREE_H_
#define TREE_H_
#include <cstdlib>
#include <cstdint>
#include <cfloat>
#include <cmath>
#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
#include "defines.h"
using namespace std;
//Constants
const int MAX_POINTS_IN_CELL = 1;
class Point;
class Node;
class Compare;

//Traversal Terminating Conditions
bool isLeaf(Node *n);
bool isEnd(Node *n);
bool isInner(Node *n);

//Tree Building
Node * buildTrees(Point *points, int lb, int ub, int depth);
void destroyTrees(Node *n);

//Point Utility Functions
float distanceAxis(Point *a, Point *b, int axis);
float distanceEuclid(Point *a, Point *b); 
int comparePoint(const void *a, const void *b); 
bool canCorrelate(Node *p, Node *n);
void updateQueue(Point*point, Point *candidate);
bool pointEqual(Point *a, Point* b);

//comparator
class Compare{
		public:
				Point* item;
				Compare(Point* _item){item = _item;}
				bool operator()(Point* a, Point* b){ return sqrt(distanceEuclid(item, a)) < sqrt(distanceEuclid(item, b));}
};
//Point Data Structure
class Point{
		public:
				Point();
				~Point();
				int label;
				float *coord;
				//Queue functions
				Point* top();
				void push(Point* p);
				void pop();
				int size();
#ifdef DEBUG
				int pid;
				static int pcount;
#endif
		private:
				//priority Queue
				vector<Point*> heap;
};
//Tree Data Structure
class Node{
		public:
				Node();
				~Node();
				//Point in the node
				Point *data;
				Point *points[MAX_POINTS_IN_CELL];
				//splitting dimentions
				int axis;
				//splitting value
				float splitval;
				//#successors below
				int succnum;
				//Root flag
				bool isRoot;
				//Links to subtrees and parents
				Node *l;
				Node *r;
				//bounding boxes
				float * min;
				float * max;
				//truncation flag
				long long vtrunc;
				long long vsubtrunc;
#ifdef DEBUG
				int nid;
				static int ncount;
#endif
};

#ifdef DEBUG
void drawTree(Node* n, ofstream& file);
#endif
#endif
/*TREE_H_*/
