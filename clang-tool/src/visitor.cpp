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
#include "visitor.h"
#include "common.h"

Rewriter TheRewriter;
string currentFunc;
string treeType;
string returnType;
string innerTreeTravInvokeCond;
map<string, string> termCond;
map<string, string> firstParam;
map<string, string> secondParam;
map<string, vector<string> > work;

bool FindFunctionVisitor :: VisitFunctionDecl(FunctionDecl *declaration) {    
		string filename_cpp(filename);
		string filename_h(filename_cpp);
		filename_h.replace(filename_h.end()-4, filename_h.end(), ".h");
		string funcName = declaration->getNameInfo().getName().getAsString();
		if (funcName == functions[0] || funcName == functions[1]) {
				FullSourceLoc fullLocation = Context->getFullLoc(declaration->getLocStart());
				if (fullLocation.isValid()){
						FileID fileID = fullLocation.getFileID();
						const SourceManager& sourceManager = fullLocation.getManager();
						const FileEntry* fileEntry = sourceManager.getFileEntryForID(fileID);
						if(filename_cpp.compare(fileEntry->getName()) == 0 || filename_h.compare(fileEntry->getName()) == 0){
								currentFunc = funcName;
								treeType   = QualType::getAsString(declaration->parameters()[0]->getType().split());
								returnType = QualType::getAsString(declaration->getCallResultType().split());
								firstParam[funcName]  = declaration->parameters()[0]->getDeclName().getAsString();
								secondParam[funcName] = declaration->parameters()[1]->getDeclName().getAsString();
						}
				}
		}
		TheRewriter.overwriteChangedFiles();
		return true;
}

bool FindFunctionVisitor :: VisitStmt(Stmt *stmt){
		string filename_cpp(filename);
		string filename_h(filename_cpp);
		filename_h.replace(filename_h.end()-4, filename_h.end(), ".h");
		if(isa<IfStmt>(stmt)){
				IfStmt *ifstmt = cast<IfStmt>(stmt);
				FullSourceLoc fullLocation = Context->getFullLoc(stmt->getLocStart());
				if (fullLocation.isValid()){
						FileID fileID = fullLocation.getFileID();
						const SourceManager& sourceManager = fullLocation.getManager();
						const FileEntry* fileEntry = sourceManager.getFileEntryForID(fileID);
						if(fileEntry){
								if(filename_cpp.compare(fileEntry->getName()) == 0){
										Stmt* thenStmt = ifstmt->getThen();
										Stmt* elseStmt = ifstmt->getElse();
										Expr* condExpr = ifstmt->getCond();
										if(isa<ReturnStmt>(thenStmt) && elseStmt == NULL){
												if(currentFunc == functions[0]){
														termCond[functions[0]] = stmtTostr(condExpr, Context->getSourceManager());
												}
												else if(currentFunc == functions[1]){
														termCond[functions[1]] = stmtTostr(condExpr, Context->getSourceManager());
												}
										}
								}
						}
				}
		}

		TheRewriter.overwriteChangedFiles();
		return true;
}

bool FindWorkVisitor :: VisitFunctionDecl(FunctionDecl *declaration) {
		string filename_cpp(filename);
		string filename_h(filename_cpp);
		filename_h.replace(filename_h.end()-4, filename_h.end(), ".h");
		string funcName = declaration->getNameInfo().getName().getAsString();
		if (funcName == functions[1]) {
				FullSourceLoc fullLocation = Context->getFullLoc(declaration->getLocStart());
				if (fullLocation.isValid()){
						FileID fileID = fullLocation.getFileID();
						const SourceManager& sourceManager = fullLocation.getManager();
						const FileEntry* fileEntry = sourceManager.getFileEntryForID(fileID);
						if(filename_cpp.compare(fileEntry->getName()) == 0){
								currentFunc = funcName;
										if (declaration->hasBody()){
												Stmt* body = declaration->getBody();
												if (isa<CompoundStmt>(body)) {
														CompoundStmt *cmpBody = cast<CompoundStmt>(body);
														unsigned int x = 0;
														for (Stmt::child_iterator i = body->child_begin(); i != body->child_end(); ++i){
																if (x > 0 && x < cmpBody->size()-2) work[functions[1]].push_back(stmtTostr(*i, Context->getSourceManager()));
																x++;
														}
												}

										}
						}
				}
		}
		TheRewriter.overwriteChangedFiles();
		return true;
}

bool AddVisitor :: VisitFunctionDecl(FunctionDecl *declaration) {    
		
		string cond1 = firstParam[functions[0]]  + " == NULL";
		string cond2 = secondParam[functions[0]] + " == NULL";

		bool select = (termCond[functions[0]] == cond1) && (termCond[functions[1]] == cond2);
		
		string filename_cpp(filename);
		string filename_h(filename_cpp);
		filename_h.replace(filename_h.end()-4, filename_h.end(), ".h");
		string funcName = declaration->getNameInfo().getName().getAsString();
		if (funcName == functions[0] || funcName == functions[1]) {
				FullSourceLoc fullLocation = Context->getFullLoc(declaration->getLocStart());
				if (fullLocation.isValid()){
						FileID fileID = fullLocation.getFileID();
						const SourceManager& sourceManager = fullLocation.getManager();
						const FileEntry* fileEntry = sourceManager.getFileEntryForID(fileID);
						if(filename_cpp.compare(fileEntry->getName()) == 0 || filename_h.compare(fileEntry->getName()) == 0){
								currentFunc = funcName;
								ParmVarDecl* lastParm = declaration->parameters()[(declaration->getNumParams()-1)];
								if (!select) TheRewriter.InsertText(lastParm->getLocEnd().getLocWithOffset(lastParm->getDeclName().getAsString().size()), ", int val", true, true);
						}
				}
		}
		TheRewriter.overwriteChangedFiles();
		return true;
}

bool AddVisitor :: VisitStmt(Stmt *stmt){
		
		string cond1 = firstParam[functions[0]]  + " == NULL";
		string cond2 = secondParam[functions[0]] + " == NULL";

		bool select = (termCond[functions[0]] == cond1) && (termCond[functions[1]] == cond2);
		
		string filename_cpp(filename);
		string filename_h(filename_cpp);
		filename_h.replace(filename_h.end()-4, filename_h.end(), ".h");
		if(isa<IfStmt>(stmt)){
				IfStmt *ifstmt = cast<IfStmt>(stmt);
				FullSourceLoc fullLocation = Context->getFullLoc(stmt->getLocStart());
				if (fullLocation.isValid()){
						FileID fileID = fullLocation.getFileID();
						const SourceManager& sourceManager = fullLocation.getManager();
						const FileEntry* fileEntry = sourceManager.getFileEntryForID(fileID);
						if(fileEntry){
								if(filename_cpp.compare(fileEntry->getName()) == 0){
										Stmt* thenStmt = ifstmt->getThen();
										Stmt* elseStmt = ifstmt->getElse();
										Expr* condExpr = ifstmt->getCond();
										if(isa<ReturnStmt>(thenStmt) && elseStmt == NULL){
												if(currentFunc == functions[0]){
														if (!select) TheRewriter.InsertText(condExpr->getLocStart().getLocWithOffset(stmtTostr(condExpr, Context->getSourceManager()).size()), " || " + firstParam[currentFunc] + "->vsubtrunc > val", true, true);
												}
												else if(currentFunc == functions[1]){
														if (!select) TheRewriter.InsertText(condExpr->getLocStart().getLocWithOffset(stmtTostr(condExpr, Context->getSourceManager()).size()), " || " + firstParam[currentFunc] + "->vtrunc > val", true, true);
												}
										}
								}
						}
				}
		}
		else if(isa<CallExpr>(stmt)){
				CallExpr *callexpr = cast<CallExpr>(stmt);
				FunctionDecl* callee = callexpr->getDirectCallee();
				FullSourceLoc fullLocation = Context->getFullLoc(stmt->getLocStart());
				if (fullLocation.isValid()){
						FileID fileID = fullLocation.getFileID();
						const SourceManager& sourceManager = fullLocation.getManager();
						const FileEntry* fileEntry = sourceManager.getFileEntryForID(fileID);
						if(fileEntry){
								if(filename_cpp.compare(fileEntry->getName()) == 0){
										string calleeName = callee->getDeclName().getAsString();
										if(currentFunc == functions[0] && calleeName == functions[0]){
												unsigned int numArgs = callexpr->getNumArgs();
												ostringstream args;
												ostringstream revargs;
												for(unsigned int i=0; i<numArgs-1; ++i){
														args << stmtTostr(callexpr->getArg(i), Context->getSourceManager()) << ", ";
														revargs << stmtTostr(callexpr->getArg(numArgs-1-i), Context->getSourceManager()) << ", ";
												}
												args << stmtTostr(callexpr->getArg(numArgs-1), Context->getSourceManager()); 
												revargs << stmtTostr(callexpr->getArg(0), Context->getSourceManager()); 

												string arg1 = stmtTostr(callexpr->getArg(0), Context->getSourceManager());
												string arg2 = stmtTostr(callexpr->getArg(1), Context->getSourceManager());

												if (!select) TheRewriter.InsertTextBefore(stmt->getLocStart(), "if(" + arg1 + " != NULL && " + arg2 + " != NULL && " + arg2 + "->succnum >= " + arg1 + "->succnum" + ") " + calleeName + "Switch(" + revargs.str() + ", val); else ");
												else         TheRewriter.InsertTextBefore(stmt->getLocStart(), "if(" + arg1 + " != NULL && " + arg2 + " != NULL && " + arg2 + "->succnum >= " + arg1 + "->succnum" + ") " + calleeName + "Switch(" + revargs.str() + "); else ");
												if (!select) TheRewriter.InsertTextBefore(stmt->getLocEnd(), ", val");
										}
										else if(calleeName == functions[1]){
												if (!select) TheRewriter.InsertTextBefore(stmt->getLocEnd(), ", val");
										}

								}
						}
				}
		}

		TheRewriter.overwriteChangedFiles();
		return true;
}
bool AddSwitchVisitor :: VisitFunctionDecl(FunctionDecl *declaration) {

		string cond1 = firstParam[functions[0]]  + " == NULL";
		string cond2 = secondParam[functions[0]] + " == NULL";

		bool select = (termCond[functions[0]] == cond1) && (termCond[functions[1]] == cond2);

		ostringstream switch1, switch2;
		if (!select) {
				switch1 << "\n\n" << returnType << " " << functions[0] << "Switch(" << treeType << secondParam[functions[1]] << ", " << treeType << firstParam[functions[1]] << ", int val)\n" 
						<< "{\n"
						<< "\t\tif (" << secondParam[functions[1]] << " == NULL) return;\n\n"
						<< "\t\tif (" << firstParam[functions[1]] << " != NULL && !(" << firstParam[functions[1]] << "->vsubtrunc > val + 2 * " << secondParam[functions[1]] << "->succnum + 1)) " << functions[1] << "Switch(" << secondParam[functions[0]] << ", " << firstParam[functions[0]] << ", val);\n\n"
						<< "\t\tint valn;\n"
						<< "\t\tvaln = val + 1;\n"
						<< "\t\t" << "if(" << secondParam[functions[1]] << "->l != NULL && " << firstParam[functions[1]] << " != NULL && " << firstParam[functions[1]] << "->succnum >= " << secondParam[functions[1]] << "->l->succnum) " << functions[0] << "(" << firstParam[functions[0]] << ", " << secondParam[functions[0]] << "->l, valn);" << " else " << functions[0] << "Switch(" << secondParam[functions[1]] << "->l, " << firstParam[functions[1]] << ", valn);\n"
						<< "\t\tvaln = (" << secondParam[functions[1]] << "->l == NULL) ? val + 1 : (val + 1) + 2 * " << secondParam[functions[1]] << "->l->succnum + 2;\n"
						<< "\t\t" << "if(" << secondParam[functions[1]] << "->r != NULL && " << firstParam[functions[1]] << " != NULL && " << firstParam[functions[1]] << "->succnum >= " << secondParam[functions[1]] << "->r->succnum) " << functions[0] << "(" << firstParam[functions[0]] << ", " << secondParam[functions[0]] << "->r, valn);" << " else " << functions[0] << "Switch(" << secondParam[functions[1]] << "->r, " << firstParam[functions[1]] << ", valn);\n"
						<< "}";
				switch2 << "\n\n" << "bool" << " " << functions[1] << "Switch(" << treeType << secondParam[functions[0]] << ", " << treeType << firstParam[functions[0]] << ", int val)\n"
						<< "{\n"
						<< "\t\tif (" << firstParam[functions[0]] << " == NULL) return true;\n\n"
						<< "\t\tif (!(" << firstParam[functions[0]] << "->vtrunc > val)){\n"
						<< "\t\t\t\tif (!(" << termCond[functions[1]] << ")){\n";
				for(unsigned int i=0; i<work[functions[1]].size();++i){
						string workStmt = work[functions[1]][i];
						if (workStmt.at(workStmt.length()-1) == ';') switch2 << "\t\t\t\t\t\t" << workStmt << "\n";
						else switch2 << "\t\t\t\t\t\t" << workStmt << ";\n";
				}
				switch2	<< "\t\t\t\t}\n"
						<< "\t\t\t\telse{\n"
						<< "\t\t\t\t\t\t" << firstParam[functions[0]] << "->vtrunc = val + 2 * " << secondParam[functions[0]] << "->succnum + 1;\n"
						<< "\t\t\t\t}\n"
						<< "\t\t}\n";
				switch2	<< "\t\tbool lsf = (" << firstParam[functions[0]] << "->l == NULL || " << firstParam[functions[0]] << "->l->vsubtrunc > val)?\n"
						<< "\t\t\t\ttrue :\n"
						<< "\t\t\t\t" << functions[1] << "Switch(" << secondParam[functions[0]] << ", " << firstParam[functions[0]] << "->l, val);\n"
						<< "\t\tbool rsf = (" << firstParam[functions[0]] << "->r == NULL || " << firstParam[functions[0]] << "->r->vsubtrunc > val)?\n"
						<< "\t\t\t\ttrue :\n"
						<< "\t\t\t\t" << functions[1] << "Switch(" << secondParam[functions[0]] << ", " << firstParam[functions[0]] << "->r, val);\n\n"
						<< "\t\tif(" << firstParam[functions[0]] << "->vtrunc > val && lsf && rsf) " << firstParam[functions[0]] << "->vsubtrunc = val + 2 * " << secondParam[functions[0]] << "->succnum + 1;\n\n"
				        << "\t\treturn " << firstParam[functions[0]] << "->vsubtrunc > val;\n"
						<< "}\n";
		}
		else {
				switch1 << "\n\n" << returnType << " " << functions[0] << "Switch(" << treeType << secondParam[functions[1]] << ", " << treeType << firstParam[functions[1]] << ")\n" 
						<< "{\n"
						<< "\t\tif (" << secondParam[functions[1]] << " == NULL) return;\n\n"
						<< "\t\tif (" << firstParam[functions[1]] << " != NULL) " << functions[1] << "Switch(" << secondParam[functions[0]] << ", " << firstParam[functions[0]] << ");\n\n"
						<< "\t\t" << "if(" << secondParam[functions[1]] << "->l != NULL && " << firstParam[functions[1]] << " != NULL && " << firstParam[functions[1]] << "->succnum >= " << secondParam[functions[1]] << "->l->succnum) " << functions[0] << "(" << firstParam[functions[0]] << ", " << secondParam[functions[0]] << "->l);" << " else " << functions[0] << "Switch(" << secondParam[functions[1]] << "->l, " << firstParam[functions[1]] << ");\n"
						<< "\t\t" << "if(" << secondParam[functions[1]] << "->r != NULL && " << firstParam[functions[1]] << " != NULL && " << firstParam[functions[1]] << "->succnum >= " << secondParam[functions[1]] << "->r->succnum) " << functions[0] << "(" << firstParam[functions[0]] << ", " << secondParam[functions[0]] << "->r);" << " else " << functions[0] << "Switch(" << secondParam[functions[1]] << "->r, " << firstParam[functions[1]] << ");\n"
						<< "}";
				switch2 << "\n\n" << returnType << " " << functions[1] << "Switch(" << treeType << secondParam[functions[0]] << ", " << treeType << firstParam[functions[0]] << ")\n"
						<< "{\n"
						<< "\t\tif (" << firstParam[functions[0]] << " == NULL) return;\n\n";
				for(unsigned int i=0; i<work[functions[1]].size();++i){
						string workStmt = work[functions[1]][i];
						if (workStmt.at(workStmt.length()-1) == ';') switch2 << "\t\t\t\t\t\t" << workStmt << "\n";
						else switch2 << "\t\t" << workStmt << ";\n";
				}
				switch2 << "\n"
						<< "\t\tif(" << firstParam[functions[0]] << "->l != NULL) " << functions[1] << "Switch(" << secondParam[functions[0]] << ", " << firstParam[functions[0]] << "->l);\n"
						<< "\t\tif(" << firstParam[functions[0]] << "->r != NULL) " << functions[1] << "Switch(" << secondParam[functions[0]] << ", " << firstParam[functions[0]] << "->r);\n"
						<< "}\n";
		}

		string filename_cpp(filename);
		string filename_h(filename_cpp);
		filename_h.replace(filename_h.end()-4, filename_h.end(), ".h");
		string funcName = declaration->getNameInfo().getName().getAsString();
		if (funcName == functions[0] || funcName == functions[1]) {
				FullSourceLoc fullLocation = Context->getFullLoc(declaration->getLocStart());
				if (fullLocation.isValid()){
						FileID fileID = fullLocation.getFileID();
						const SourceManager& sourceManager = fullLocation.getManager();
						const FileEntry* fileEntry = sourceManager.getFileEntryForID(fileID);
						if(filename_h.compare(fileEntry->getName()) == 0){
								if(funcName == functions[0]){
										ostringstream oss;
										
										if(!select) oss << "\n" << returnType << " " << functions[0] << "Switch(" << treeType << secondParam[functions[0]] << ", " << treeType << firstParam[functions[0]] << ", int val);\n";
										else oss << "\n" << returnType << " " << functions[0] << "Switch(" << treeType << secondParam[functions[0]] << ", " << treeType << firstParam[functions[0]] << ");\n";
                                          
										TheRewriter.InsertTextAfterToken(declaration->getLocEnd().getLocWithOffset(1), oss.str());
								}
								if(funcName == functions[1]){
										ostringstream oss;
										
										if(!select) oss << "\n" << "bool " << functions[1] << "Switch(" << treeType << secondParam[functions[1]] << ", " << treeType << firstParam[functions[1]] << ", int val);\n";
										else oss << "\n" << returnType << " " << functions[1] << "Switch(" << treeType << secondParam[functions[1]] << ", " << treeType << firstParam[functions[1]] << ");\n";
										
										TheRewriter.InsertTextAfterToken(declaration->getLocEnd().getLocWithOffset(1), oss.str());
								}

						}
						if(filename_cpp.compare(fileEntry->getName()) == 0){
								if(funcName == functions[0]){
										TheRewriter.InsertTextAfterToken(declaration->getLocEnd(), switch1.str());
								}
								if(funcName == functions[1]){
										TheRewriter.InsertTextAfterToken(declaration->getLocEnd(), switch2.str());
								}

						}
				}
		}
		TheRewriter.overwriteChangedFiles();
		return true;
}

void FindFunctionConsumer :: HandleTranslationUnit(ASTContext &Context) {
		VisitorA.TraverseDecl(Context.getTranslationUnitDecl());
		VisitorB.TraverseDecl(Context.getTranslationUnitDecl());
		VisitorC.TraverseDecl(Context.getTranslationUnitDecl());
		VisitorD.TraverseDecl(Context.getTranslationUnitDecl());
}

unique_ptr<ASTConsumer> FindFunctionAction :: CreateASTConsumer(CompilerInstance &Compiler, StringRef InFile) {
		TheRewriter.setSourceMgr(Compiler.getSourceManager(), Compiler.getLangOpts());
		return unique_ptr<ASTConsumer>(new FindFunctionConsumer(&Compiler.getASTContext(), TheRewriter));
}
