#include <iostream>
#include <thread>
#include <chrono>

#include "some.h"

using namespace std;

int count() {
	if (true) return 20000;
	else return 10000;
}

std::vector<header_thing> header_thing_container;

void start_leak() {

	for (int i = 0; i < count(); ++i)
		header_thing_container.push_back({});
}

int main()
{
	struct in_func{} s;

	cout << "Hello World!" << endl;

	start_leak();

	std::this_thread::sleep_for(std::chrono::minutes(2));
	return 0;
}
