#pragma once

#include <memory>
#include <vector>
struct cpp_thing;
class header_thing
{
	std::vector<int> x = std::vector<int>(20, 123);
public:
	header_thing();
};
