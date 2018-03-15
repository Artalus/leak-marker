#include "marker_record_handler.h"

#include <sstream>
#include <algorithm>

using std::string;
using std::stringstream;

using namespace clang;
using namespace clang::ast_matchers;
using namespace clang::tooling;

namespace {

std::string format_leak_mark(const RecordDecl &rec) {
	const string mark_id = rec.getNameAsString();
	const string leak_mark = "LEAKMARK: " + rec.getQualifiedNameAsString();
	stringstream ss;
	ss << "\n/*>>>> >>>>*/  const char leakmark_" << mark_id << "[" << leak_mark.size()+1 << "] = \"" << leak_mark << "\";\n";
	return ss.str();
}

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

		auto f = sm->getFilename(insert_pos);

		auto &r = replacements_map[f];

		Replacement rep(*sm, insert_pos, 0, format_leak_mark(*rec));
		if ( find(r.begin(), r.end(), rep) == r.end() ) {
			if (auto e = r.add(rep)) {
				throw e;
			}
		}
	}
}
