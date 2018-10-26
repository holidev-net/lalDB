#include <assert.h>
#include <iostream>
#include "../../../includes/DataRepresentation/DataRepresentation.hpp"

int main(int ac, char **av) {
	laldb::DataRepresentation d = laldb::makeObject {
		{ "key", "Value" },
		{ "nbr", 1 }
	};

	assert(d["nbr"] == 1 && d["key"] == "Value");
	std::cout << d["nbr"].value<laldb::Number>() << " " << d["key"].value<laldb::String>() << std::endl;
	return (0);
}