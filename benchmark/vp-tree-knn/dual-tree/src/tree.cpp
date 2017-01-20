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
#include "tree.h"
#include "util.h"
#include "common.h"
Node *root1, *root2;
int tree_depth = 0;

#ifdef DEBUG
int Point::pcount = 0;	
int Node::ncount = 0;
#endif
//functions
Point::Point() 
{
		this->coord = (float*)malloc(sizeof(float) * dim);
		this->heap.reserve(knn);
#ifdef DEBUG
		this->pid = pcount++;
#endif
}

Point::~Point()
{
		free(coord);
}

Point* Point::top()
{
		make_heap(this->heap.begin(), this->heap.end(), Compare(this));
		return this->heap.front();
}

void Point::push(Point* p)
{
		this->heap.push_back(p);
}

void Point::pop()
{
		pop_heap(this->heap.begin(), this->heap.end(), Compare(this));
		this->heap.pop_back();
}

int Point::size()
{
		return this->heap.size();

}

Node::Node() 
{
		this->data    = NULL;
		this->thresh  = 0.0;
		
		this->succnum = 0;
		this->isRoot  = false;

		this->l       = NULL;
		this->r       = NULL;
		this->p       = NULL;
		
		this->lor     = -1;

		this->vtrunc    = 0;
		this->vsubtrunc = 0;
#ifdef DEBUG
		this->nid = ncount++;
#endif
}

Node::~Node(){}

bool isLeaf(Node *n)
{
		return (n->l == NULL && n->r == NULL);
}

bool isEnd(Node *n)
{
		return n == NULL;
}

bool isInner(Node *n)
{
		return (isLeaf(n->l) || isLeaf(n->r));
}

Node * buildTrees(vector<Point*>& points, int lb, int ub, int depth)
{
		if (ub == lb) return NULL;

		tree_depth = (depth > tree_depth) ? depth : tree_depth;
		
		Node *node = new Node();

		if (ub - lb > 1) {
				int i = (int)((float)rand() / RAND_MAX * (ub - lb - 1)) + lb;
				int mid  = (ub + lb) / 2;
				
				swap(points[lb], points[mid]);
				nth_element(points.begin()+lb+1, points.begin()+mid, points.begin()+ub, Compare(points[lb]));

				node->thresh   = sqrt(distanceEuclid(points[lb], points[mid]));

				node->l        = buildTrees(points, lb+1 , mid, depth+1);
				node->r        = buildTrees(points, mid, ub, depth+1);

				if      (node->l != NULL && node->r != NULL) node->succnum = node->l->succnum + node->r->succnum + 2;
				else if (node->l != NULL && node->r == NULL) node->succnum = node->l->succnum + 1;
				else if (node->l == NULL && node->r != NULL) node->succnum = node->r->succnum + 1;
				else node->succnum = 0;

				if (node->l != NULL) {node->l->p = node; node->l->lor = 0;}
				if (node->r != NULL) {node->r->p = node; node->r->lor = 1;}

		}	
		
		node->data = points[lb];
		
		return node;
}

void destroyTrees(Node *n)
{
		if (n->l != NULL) destroyTrees(n->l);
		if (n->r != NULL) destroyTrees(n->r);
		delete n;
}

float distanceAxis(Point *a, Point *b, int axis) 
{
		return (a->coord[axis] - b->coord[axis]) * (a->coord[axis] - b->coord[axis]);
}

float distanceEuclid(Point *a, Point *b) 
{
		float d = 0;
		for(int i = 0; i < dim; i++) {
				d += distanceAxis(a,b,i);
		}
		return sqrt(d);
}

bool canCorrelate(Node *t1, Node *t2)
{
		if(t2->p == NULL) return true;

		float dist = distanceEuclid(t1->data, t2->p->data);
		float farDist = distanceEuclid(t1->data, t1->data->top());
		
		if(t2->lor == 0 && sqrt(dist) <  t2->p->thresh && sqrt(dist) - sqrt(farDist) <= t2->p->thresh) return true;
		if(t2->lor == 1 && sqrt(dist) <  t2->p->thresh && sqrt(dist) + sqrt(farDist) >= t2->p->thresh) return true;
		if(t2->lor == 1 && sqrt(dist) >= t2->p->thresh && sqrt(dist) + sqrt(farDist) >= t2->p->thresh) return true;
		if(t2->lor == 0 && sqrt(dist) >= t2->p->thresh && sqrt(dist) - sqrt(farDist) <= t2->p->thresh) return true;

		return false;
}

void updateQueue(Point *point, Point *candidate) 
{
		float dist = distanceEuclid(point, candidate);
		if (point->size() == knn){
				if(sqrt(dist) < sqrt(distanceEuclid(point, point->top()))){
						point->pop();
						point->push(candidate);
				}
		}
		else{
				point->push(candidate);
		}
}

bool pointEqual(Point *a, Point* b)
{
		for(int i=0; i<dim; i++)
				if(a->coord[i] != b->coord[i]) return false;
		return true;
}
#ifdef DEBUG
void drawTree(Node* n, ofstream& file){
		if(isEnd(n)) return;
		if(!isEnd(n->l)) file << setw(ceil(log10(Node::ncount))) << setfill('0') << n->nid << " -> " << setw(ceil(log10(Node::ncount))) << setfill('0') << n->l->nid << ";" << endl;
		drawTree(n->l, file);
		if(!isEnd(n->r)) file << setw(ceil(log10(Node::ncount))) << setfill('0') << n->nid << " -> " << setw(ceil(log10(Node::ncount))) << setfill('0') << n->r->nid << ";" << endl;
		drawTree(n->r, file);
}
#endif
