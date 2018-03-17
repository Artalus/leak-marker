#include <llvm/Support/Error.h>
#include <llvm/Support/CommandLine.h>

#include "marker_tool.h"

namespace {

using namespace llvm::cl;

OptionCategory marker_category{"marker options"};

//TODO: why does it cuts "output e" if the string is not a separate variable? o_O
const auto s = std::string("output edited files to stdout, separating buffers by ") + MarkerTool::output_delimiter;
opt<bool> show_output{"-show-output", desc(s),
	        cat(marker_category)};
alias show_output_a{"s", desc("alias for --show-output"), aliasopt(show_output), cat(marker_category)};

opt<bool> overwrite{"-overwrite", desc("write tool output directly to parsed files"), cat(marker_category)};
alias overwrite_a{"o", desc("alias for --overwrite"), aliasopt(overwrite), cat(marker_category)};

opt<bool> no_confirmation{"-no-confirmation", desc("do not ask for user confirmation when --overwrite is specified"),
	        cat(marker_category)};

opt<std::string> compilation_database{"p", desc("path to directory containing project compilation database (compiler_commands.json)"),
	        init("."), cat(marker_category)};
list<std::string> input_filenames{Positional, desc("[<source0> [... <sourceN>]]"), ZeroOrMore};

}

int main(int argc, const char **argv) try {
	HideUnrelatedOptions(marker_category);
	ParseCommandLineOptions(argc, argv,
	                        "\n  This tool is used to insert a const char[] leak mark to each struct in project."
	                        "\n  Use positional arguments to specify files which should be instrumented."
	                        "\n  If no positional arguments are specified, the tool is applied to each file"
	                        "\n  in compilation database.\n");

	if (!show_output && !overwrite) {
		PrintHelpMessage();
		return 0;
	}

	MarkerTool tool(compilation_database, input_filenames, show_output, overwrite, no_confirmation);

	tool.rewrite();

	return 0;
} catch (const llvm::Error &) {
	llvm::errs() << "Whoops! Error. How do I handle them in LLVM?\n";
	return 1;
} catch (const std::exception &e) {
	llvm::errs() << e.what() << '\n';
	return 1;
}
