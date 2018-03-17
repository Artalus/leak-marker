#pragma once

#include <string>

#include <clang/Tooling/Refactoring.h>

#include <llvm/ADT/ArrayRef.h>


class MarkerTool {
public:
	static constexpr const char output_delimiter[] = "--8<--";
	MarkerTool(std::string db_path, llvm::ArrayRef<std::string> input_filenames,
	           bool show_output, bool overwrite, bool no_confirmation);

	void rewrite();

private:
	void apply_rewrite();

	std::unique_ptr<clang::tooling::CompilationDatabase> db;
	llvm::ArrayRef<std::string> source_files;
	std::unique_ptr<clang::tooling::RefactoringTool> tool;

	bool show_output,
	no_confirmation,
	overwrite;
};
