#include <iostream>
#include <vector>
#include <algorithm>

struct my_struct {
	int i;
	double d;

	bool operator<(const my_struct &) {
		return true;
	}
};

std::vector<my_struct> v;

struct wrapper {
	std::vector<decltype(v)> b;

	void f() {
		std::sort(v.begin(), v.end());
	}
};


int f() {
	std::sort(v.begin(), v.end());
	return 1;
}
