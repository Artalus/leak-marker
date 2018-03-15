#include "some.h"
#include <iostream>

namespace {
struct ns_thing {
	struct instruct_thing {
		char z;
	} t;
	uint8_t r[4];
};

std::vector<std::unique_ptr<cpp_thing>> rur;
}

struct cpp_thing {
	struct instruct_thing {
		double z;
	} t;
	int x = 0;
	int y = 1;
	ns_thing z;
};

header_thing::header_thing()
{
	std::cout << "creating some\n";
	static int zaz = 0;
	if (++zaz % 13 == 0) {
		std::cout << "adding whatever\n";
		rur.emplace_back(std::make_unique<cpp_thing>());
	}
}
