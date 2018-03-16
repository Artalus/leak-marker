#include "marker_record_handler.h"

#include <sstream>
#include <algorithm>

using std::string;
using std::stringstream;

using namespace clang;
using namespace clang::ast_matchers;
using namespace clang::tooling;

namespace {
struct leak_mark {
	const std::string
	unifier,
	var_name,
	text,
	replacement;
	leak_mark(const RecordDecl &rec)
	        : unifier(rec.getNameAsString())
	        , var_name("leakmark_" + unifier)
	        , text("LEAKMARK: " + rec.getQualifiedNameAsString())
	        , replacement(format_replacement(var_name, text))
	{}

private:
	static std::string format_replacement(const std::string &var_name, const std::string &text) {
		stringstream ss;
		ss << "\n/*>>>> >>>>*/  const char " << var_name << "[" << text.size()+1 << "] = \"" << text << "\";\n";
		return ss.str();
	}
};

}



MarkerRecordHandler::MarkerRecordHandler(std::map<std::string, Replacements> &R)
        : replacements_map(R)
{}

void MarkerRecordHandler::run(const MatchFinder::MatchResult &result) {
	if (const RecordDecl* rec = result.Nodes.getNodeAs<RecordDecl>(bind_name)) {
		if (!rec->isCompleteDefinition() || rec->isUnion())
			return;

		auto *sm = result.SourceManager;

		SourceLocation insert_pos = rec->getLocEnd().getLocWithOffset(-1);

		if (sm->isInExternCSystemHeader(insert_pos) || sm->isInSystemHeader(insert_pos))
			return;


		leak_mark mark(*rec);

		// this ignores inserting marker into already edited records
		if (std::find_if(rec->field_begin(), rec->field_end(),
		                 [&mark](FieldDecl *f) { return f->getNameAsString() == mark.var_name; })
		    != rec->field_end()) {
			llvm::errs() << mark.text << " is already present\n";
			return;
		}

		Replacement rep(*sm, insert_pos, 0, mark.replacement);

		auto &r = replacements_map[sm->getFilename(insert_pos)];

		// this ignores already made replacements in headers included many times
		if ( find(r.begin(), r.end(), rep) == r.end() ) {
			if (auto e = r.add(rep)) {
				throw e;
			}
		}
	}
}
