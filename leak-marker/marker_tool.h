#pragma once

#include <string>
#include <vector>

#include <clang/Tooling/Refactoring.h>


class MarkerTool {
public:
	static constexpr const char output_delimiter[] = "--8<--";
	MarkerTool(std::string db_path, std::vector<std::string> input_filenames,
	           bool show_output, bool overwrite, bool no_confirmation);

	void rewrite();

private:
	void apply_rewrite();

	std::unique_ptr<clang::tooling::CompilationDatabase> db;
	std::vector<std::string> source_files;
	std::unique_ptr<clang::tooling::RefactoringTool> tool;

	bool show_output,
	no_confirmation,
	overwrite;
};
