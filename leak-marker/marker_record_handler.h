#pragma once
#include <string>
#include <map>

#include <clang/ASTMatchers/ASTMatchers.h>
#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/Tooling/Refactoring.h>


class MarkerRecordHandler : public clang::ast_matchers::MatchFinder::MatchCallback {
public:
	static constexpr char bind_name[] = "rec";

	MarkerRecordHandler(std::map<std::string, clang::tooling::Replacements> &R);

	void run(const clang::ast_matchers::MatchFinder::MatchResult &result) override;

private:
	std::map<std::string, clang::tooling::Replacements> &replacements_map;
};
