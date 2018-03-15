#include <sstream>
#include <string>

#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/Frontend/ASTConsumers.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"

using namespace clang;
using namespace clang::ast_matchers;
using namespace clang::tooling;


class MarkerRecordHandler : public MatchFinder::MatchCallback {
public:
	static constexpr char bind_name[] = "rec";

	MarkerRecordHandler(Rewriter &R) : rewriter(R) {}

	void run(const MatchFinder::MatchResult &Result) override {
		if (const RecordDecl* rec = Result.Nodes.getNodeAs<RecordDecl>(bind_name)) {
			if (!rec->isCompleteDefinition() || rec->isUnion())
				return;

			std::string mark_id = rec->getNameAsString();
			std::string leak_mark = "LEAKMARK: " + rec->getQualifiedNameAsString();

			std::stringstream ss;
			ss << "\n/*>>>> >>>>*/  const char leakmark_" << mark_id << "[" << leak_mark.size()+1 << "] = \"" << leak_mark << "\";\n";
			SourceLocation st = rec->getLocEnd().getLocWithOffset(-1);
			rewriter.InsertTextAfterToken(st, ss.str());
		}
	}

private:
	Rewriter &rewriter;
};

class MarkerASTConsumer : public ASTConsumer {
public:
	MarkerASTConsumer(Rewriter &rewriter) : record_handler(rewriter) {
		matcher.addMatcher(recordDecl().bind(record_handler.bind_name), &record_handler);
	}

	// Override the method that gets called for each parsed top-level declaration.
	void HandleTranslationUnit(ASTContext &context) override {
		matcher.matchAST(context);
	}

private:
	MarkerRecordHandler record_handler;
	MatchFinder matcher;
};

// For each source file provided to the tool, a new FrontendAction is created.
class MarkerFrontendAction : public ASTFrontendAction {
public:
	MarkerFrontendAction() {}

	void EndSourceFileAction() override {
		SourceManager &sm = rewriter.getSourceMgr();
		llvm::errs() << "** EndSourceFileAction for: " << sm.getFileEntryForID(sm.getMainFileID())->getName() << "\n";

		rewriter.getEditBuffer(sm.getMainFileID()).write(llvm::outs());
	}

	std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &compiler, StringRef file) override {
		llvm::errs() << "** Creating AST consumer for: " << file << "\n";
		rewriter.setSourceMgr(compiler.getSourceManager(), compiler.getLangOpts());
		return std::make_unique<MarkerASTConsumer>(rewriter);
	}

private:
	Rewriter rewriter;
};

int main(int argc, const char **argv) {
	std::string err;
	auto db = CompilationDatabase::autoDetectFromDirectory(argc > 1 ? argv[1] : ".", err);
	if (!db) {
		llvm::errs() << err;
		return 1;
	}
	ClangTool tool(*db, db->getAllFiles());
	MarkerFrontendAction a;
	return tool.run(newFrontendActionFactory<MarkerFrontendAction>().get());
}
