cmake_minimum_required(VERSION 3.5)

option(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)

project(test-project LANGUAGES CXX)
add_executable(${PROJECT_NAME}
	"main.cpp"

	"sep/diff_records.cpp"
	"sep/w_std.cpp"
	"sep/in_anon_ns.cpp"
	"sep/in_named_ns.cpp"
	"sep/in_multi_ns.cpp"
	"sep/in_header.cpp"
	"sep/in_multiinc_header1.cpp"
	"sep/in_multiinc_header2.cpp"
	"sep/in_struct.cpp"
	"sep/in_multi_struct.cpp"
	"sep/in_func.cpp"

	"sep/diff_braces/next_line.cpp"
	"sep/diff_braces/next_line_empty.cpp"
	"sep/diff_braces/next_line_empty_w_newline.cpp"
	"sep/diff_braces/next_line_w_double_indent.cpp"
	"sep/diff_braces/next_line_w_indent.cpp"
	"sep/diff_braces/one_line.cpp"
	"sep/diff_braces/one_line_empty.cpp"
	"sep/diff_braces/one_line_empty_w_space.cpp"
	"sep/diff_braces/one_line_newline.cpp"
	"sep/diff_braces/same_line.cpp"
	"sep/diff_braces/same_line_empty.cpp"
	"sep/diff_braces/same_line_empty_w_newline.cpp"
	"sep/diff_braces/same_line_wo_spaces.cpp"
	"sep/diff_braces/same_line_w_spaces.cpp"
)
