#include "ATest.hpp"

int main(int ac, char **av) {
	for (auto *i: ATest::singleton()) {
		i->launchTests();
	}
}