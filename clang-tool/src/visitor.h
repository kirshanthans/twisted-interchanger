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
#ifndef VISITOR_H_
#define VISITOR_H_
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "clang/Rewrite/Frontend/Rewriters.h"

#include "llvm/Support/CommandLine.h"

#include <cstdlib>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <utility>
#include "util.h"

using namespace std;
using namespace clang;
using namespace clang::tooling;
using namespace llvm;
using namespace llvm::cl;

static OptionCategory Category("Tool Options");

class FindFunctionVisitor : public RecursiveASTVisitor<FindFunctionVisitor>
{
		public:
				explicit FindFunctionVisitor(ASTContext *Context, Rewriter& R) : Context(Context), TheRewriter(R) {};
				bool VisitFunctionDecl(FunctionDecl *declaration); 
				bool VisitStmt(Stmt *stmt);
		private:
				ASTContext *Context;
				Rewriter&  TheRewriter;
};

class FindWorkVisitor : public RecursiveASTVisitor<FindWorkVisitor>
{
		public:
				explicit FindWorkVisitor(ASTContext *Context, Rewriter& R) : Context(Context), TheRewriter(R) {};
				bool VisitFunctionDecl(FunctionDecl *declaration); 
		private:
				ASTContext *Context;
				Rewriter&  TheRewriter;
};

class AddVisitor : public RecursiveASTVisitor<AddVisitor>
{
		public:
				explicit AddVisitor(ASTContext *Context, Rewriter& R) : Context(Context), TheRewriter(R) {};
				bool VisitFunctionDecl(FunctionDecl *declaration); 
				bool VisitStmt(Stmt *stmt);
		private:
				ASTContext *Context;
				Rewriter&  TheRewriter;

};

class AddSwitchVisitor : public RecursiveASTVisitor<AddSwitchVisitor>
{
		public:
				explicit AddSwitchVisitor(ASTContext *Context, Rewriter& R) : Context(Context), TheRewriter(R) {};
				bool VisitFunctionDecl(FunctionDecl *declaration); 
		private:
				ASTContext *Context;
				Rewriter&  TheRewriter;
};

class FindFunctionConsumer : public ASTConsumer
{
		public:
				explicit FindFunctionConsumer(ASTContext *Context, Rewriter& R) : VisitorA(Context, R), VisitorB(Context, R), VisitorC(Context, R), VisitorD(Context, R) {};
				virtual void HandleTranslationUnit(ASTContext &Context);
		private:
				FindFunctionVisitor VisitorA;
				FindWorkVisitor VisitorB;
				AddVisitor VisitorC;
				AddSwitchVisitor VisitorD;
};

class FindFunctionAction : public ASTFrontendAction
{
		public:
				virtual unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &Compiler, StringRef InFile);
}; 
#endif
