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

#ifdef DEBUG
int Point::pcount = 0;	
int Node::ncount = 0;
#endif
//functions
Point::Point() 
{
		this->coord = (float*)malloc(sizeof(float) * dim);
#ifdef DEBUG
		this->pid = pcount++;
#endif
}

Point::~Point()
{
		free(coord);
}

Node::Node() 
{
		this->data = NULL;
		this->l    = NULL;
		this->r    = NULL;

		this->succnum = 0;

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

Node * buildTrees(Point *points, int lb, int ub, int depth)
{
		Node *node = new Node();

		int size = ub - lb + 1;
		int mid  = (ub + lb) / 2;

		if (size == 0) return NULL;
		
		else if (size == 1) {

				node->data = &points[mid];
				node->l    = NULL;
				node->r    = NULL;

				node->succnum  = 0;

				return node;
		} 
		else {

				node->data = &points[mid];
				node->l    = buildTrees(points, lb, mid-1, depth + 1);
				node->r    = buildTrees(points, mid+1, ub, depth + 1);

				if      (node->l != NULL && node->r != NULL) node->succnum = node->l->succnum + node->r->succnum + 2;
				else if (node->l != NULL && node->r == NULL) node->succnum = node->l->succnum + 1;
				else if (node->l == NULL && node->r != NULL) node->succnum = node->r->succnum + 1;
				else node->succnum = 0;

				return node;
		}	
}

void destroyTrees(Node *n)
{
		if (n->l != NULL) destroyTrees(n->l);
		if (n->r != NULL) destroyTrees(n->r);
		delete n;
}

void multi(Point *row, Point *col) 
{
		int sum = 0;
		for(int i=0; i<dim; i++){
				sum += (row->coord[i]*col->coord[i]);
		}
		out[row->label * dim + col->label] = sum;
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
