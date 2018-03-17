#include "marker_tool.h"

#include <iostream>

#include <clang/Frontend/TextDiagnosticPrinter.h>
#include <clang/Rewrite/Core/Rewriter.h>
#include <clang/Tooling/Refactoring.h>

#include "marker_record_handler.h"


using namespace clang;
using namespace clang::tooling;

namespace {

void write_to_file(const StringRef &filename, clang::RewriteBuffer &buffer) {
	using llvm::sys::fs::OpenFlags;
	std::error_code err;
	llvm::raw_fd_ostream of(filename, err, OpenFlags::F_RW | OpenFlags::F_Text);
	if (err)
		throw std::runtime_error("rewriter couldn't write to file " + filename.str());
	buffer.write(of);
}

}

MarkerTool::MarkerTool(std::string db_path, llvm::ArrayRef<std::string> input_filenames,
                       bool show_output, bool overwrite, bool no_confirmation) {
	this->show_output = show_output;
	this->overwrite = overwrite;
	this->no_confirmation = no_confirmation;

	std::string err = "errmsg??";
	db = CompilationDatabase::autoDetectFromDirectory(db_path, err);
	if (!db)
		throw std::runtime_error(err);

	this->source_files = input_filenames.empty()
	        ? llvm::ArrayRef<std::string>(db->getAllFiles())
	        : input_filenames;
	tool = std::make_unique<RefactoringTool>(*db, source_files);
}


void MarkerTool::rewrite()
{
	MarkerRecordHandler record_handler(tool->getReplacements());

	ast_matchers::MatchFinder finder;
	finder.addMatcher(ast_matchers::recordDecl().bind(MarkerRecordHandler::bind_name), &record_handler);

	if (int result = tool->run(newFrontendActionFactory(&finder).get()))
		throw std::runtime_error("tool.run() returned " + std::to_string(result) + "\n");

	apply_rewrite();
}

void MarkerTool::apply_rewrite()
{
	// We need a SourceManager to set up the Rewriter.
	IntrusiveRefCntPtr<DiagnosticOptions> diag_opts = new DiagnosticOptions();
	DiagnosticsEngine diagnostics(
	            IntrusiveRefCntPtr<DiagnosticIDs>(new DiagnosticIDs()),
	            &*diag_opts,
	            new TextDiagnosticPrinter(llvm::errs(), &*diag_opts), true);
	SourceManager sm(diagnostics, tool->getFiles());

	Rewriter rewrite(sm, LangOptions());
	tool->applyAllReplacements(rewrite);


	// SO + O + NC -> display and rewrite in a single pass
	// .. + O + !NC -> display first, request confirmation, overwrite

	if (show_output) {
		for (auto i = rewrite.buffer_begin(), e = rewrite.buffer_end(); i != e; ++i) {
			auto filename = sm.getFileEntryForID(i->first)->getName();

			if (i != rewrite.buffer_begin())
				llvm::outs() << output_delimiter << '\n';
			llvm::outs() << filename << "\n";

			i->second.write(llvm::outs());
			if (overwrite && no_confirmation) {
				write_to_file(filename, i->second);
			}
		}
		if (overwrite && no_confirmation)
			return;
	}

	if (!overwrite)
		return;

	if (!no_confirmation) {
		llvm::outs() << "WARNING!\n"
		                "You've specified 'overwrite' flag - press 'Enter' to confirm above changes in files.\n"
		                "If unsure, stop execution.\n";
		std::cin.get();
	}

	for (auto i = rewrite.buffer_begin(), e = rewrite.buffer_end(); i != e; ++i) {
		write_to_file(sm.getFileEntryForID(i->first)->getName(), i->second);
	}
}
