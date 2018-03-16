#include <clang/Frontend/TextDiagnosticPrinter.h>
#include <clang/Rewrite/Core/Rewriter.h>
#include <clang/Tooling/CommonOptionsParser.h>

#include "marker_record_handler.h"

using namespace clang;
using namespace clang::ast_matchers;
using namespace clang::tooling;

int main(int argc, const char **argv) try {
	llvm::cl::OptionCategory ToolingSampleCategory("Tooling Sample");
	CommonOptionsParser op(argc, argv, ToolingSampleCategory);
	RefactoringTool tool(op.getCompilations(), op.getSourcePathList());

	MarkerRecordHandler record_handler(tool.getReplacements());

	MatchFinder finder;
	finder.addMatcher(recordDecl().bind(MarkerRecordHandler::bind_name), &record_handler);

	if (int result = tool.run(newFrontendActionFactory(&finder).get())) {
		llvm::errs() << "Whoops! tool.run() returned " << result << '\n';
		return result;
	}

	// We need a SourceManager to set up the Rewriter.
	IntrusiveRefCntPtr<DiagnosticOptions> diag_opts = new DiagnosticOptions();
	DiagnosticsEngine diagnostics(
	                        IntrusiveRefCntPtr<DiagnosticIDs>(new DiagnosticIDs()),
	                        &*diag_opts,
	                        new TextDiagnosticPrinter(llvm::errs(), &*diag_opts), true);
	SourceManager sm(diagnostics, tool.getFiles());

	// Apply all replacements to a rewriter.
	Rewriter rewrite(sm, LangOptions());
	tool.applyAllReplacements(rewrite);

	// Query the rewriter for all the files it has rewritten, dumping their new
	// contents to stdout.
	for (auto i = rewrite.buffer_begin(), e = rewrite.buffer_end(); i != e; ++i) {
		const FileEntry *entry = sm.getFileEntryForID(i->first);
		if (i != rewrite.buffer_begin())
			llvm::outs() << "--8<--\n";
		llvm::outs() << entry->getName() << "\n";
		i->second.write(llvm::outs());
	}

	return 0;
} catch (const llvm::Error &) {
	llvm::errs() << "Whoops! Error. How do I handle them in LLVM?\n";
	return 1;
}
