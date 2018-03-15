#include <iostream>
#include "some.h"
#include <thread>
#include <chrono>

using namespace std;

int lobzyr() {
	if (true) return 20000;
	else return 10000;
}

void bobzyr() {
	std::vector<header_thing> sus;

	for (int i = 0; i < lobzyr(); ++i)
		sus.push_back({});
}

namespace a {
namespace b {
namespace c {
struct abc_empty_thing{};
}
}
}

int main()
{
	struct in_func{} s;
	cout << "Hello World!" << endl;

	bobzyr();

	std::this_thread::sleep_for(std::chrono::minutes(2));
	return 0;
}
