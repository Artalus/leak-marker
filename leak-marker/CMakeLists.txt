add_executable(marker main.cpp)

target_include_directories(marker
    PRIVATE
    /usr/lib/clang/5.0.1/include
)

target_link_libraries(marker
	clangFrontendTool
	clangFrontend
	clangDriver
	clangSerialization
	clangParse
	clangSema
	clangStaticAnalyzerFrontend
	clangStaticAnalyzerCheckers
	clangStaticAnalyzerCore
	clangAnalysis
	clangRewrite
	clangEdit
	clangAST
	clangASTMatchers
	clangLex
	clangBasic
	clangRewriteFrontend
	clangToolingCore
	clangTooling
	clangFormat
	LLVM
)
