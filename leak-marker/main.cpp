#include <sstream>
#include <string>

#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/ASTConsumers.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"

using namespace clang;
using namespace clang::tooling;


class MarkerASTVisitor : public RecursiveASTVisitor<MarkerASTVisitor> {
public:
	MarkerASTVisitor(Rewriter &R) : rewriter(R) {}

	bool VisitRecordDecl(RecordDecl *rec) {
		std::string rec_name = rec->getNameAsString();
		if (rec_name == "ns_thing")
			llvm::errs() << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n";
		if (rec->isCompleteDefinition()) {
//			rec = rec->getDefinition();
			rec->getQualifiedNameAsString();
			std::string rec_full_name = rec->getQualifiedNameAsString();

			std::stringstream ss;
			ss << "\n/*>>>> >>>>*/  const char leakmark_"<< rec_name << "[] = \"LEAKMARK: " << rec_full_name << "\";\n";
			SourceLocation st = rec->getLocEnd().getLocWithOffset(-1);
			rewriter.InsertTextAfterToken(st, ss.str());
		}

		return true;
	}

private:
	Rewriter &rewriter;
};

// Implementation of the ASTConsumer interface for reading an AST produced
// by the Clang parser.
class MarkerASTConsumer : public ASTConsumer {
public:
	MarkerASTConsumer(Rewriter &rewriter) : visitor(rewriter) {}

	// Override the method that gets called for each parsed top-level declaration.
	bool HandleTopLevelDecl(DeclGroupRef group) override {
		for (Decl* d : group) {
			visitor.TraverseDecl(d);
		}
		return true;
	}

private:
	MarkerASTVisitor visitor;
};

// For each source file provided to the tool, a new FrontendAction is created.
class MarkerFrontendAction : public ASTFrontendAction {
public:
	MarkerFrontendAction() {}

	void EndSourceFileAction() override {
		SourceManager &sm = rewriter.getSourceMgr();
		llvm::errs() << "** EndSourceFileAction for: " << sm.getFileEntryForID(sm.getMainFileID())->getName() << "\n";

		// Now emit the rewritten buffer.
		rewriter.getEditBuffer(sm.getMainFileID()).write(llvm::outs());
	}

	std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &compiler, StringRef file) override {
		llvm::errs() << "** Creating AST consumer for: " << file << "\n";
		rewriter.setSourceMgr(compiler.getSourceManager(), compiler.getLangOpts());
		return llvm::make_unique<MarkerASTConsumer>(rewriter);
	}

private:
	Rewriter rewriter;
};

int main(int argc, const char **argv) {
	//llvm::cl::OptionCategory category(std::string(argv[0]) + " options");
	//CommonOptionsParser op(argc, argv, category);
	//ClangTool Tool(op.getCompilations(), op.getSourcePathList());
	std::string err;
	auto db = CompilationDatabase::autoDetectFromDirectory(argc > 1 ? argv[1] : ".", err);
	if (!db) {
		llvm::errs() << err;
		return 1;
	}
	ClangTool tool(*db, db->getAllFiles());
	MarkerFrontendAction a;
//	return tool.run(newFrontendActionFactory<MarkerFrontendAction>().get());
	return tool.run(newFrontendActionFactory<MarkerFrontendAction>().get());
}
