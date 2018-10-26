#include <iostream>
#include <functional>
#include <tuple>
#include "Datastore/Datastore.hpp"
#include "../ATest.hpp"

std::list<std::function<std::tuple<bool, std::string>()>> _fcts {
	[] () -> std::tuple<bool, std::string> {
		laldb::Datastore	ds;
		bool res = true;
		std::string message;

		ds.insert(laldb::makeObject {
			{ "name", "salut" },
			{ "age", 16 } 
		}, [&] (laldb::Datastore::Error err, laldb::DataRepresentation doc) {
			res = !err;
			if (err) {
				message.append("insert return an error");
			}
		});
		if (res == false)
			return std::make_tuple(res, message);
		
		ds.find(laldb::makeObject {
			{ "name", "salut" }
		}, [&] (laldb::Datastore::Error err, laldb::DataRepresentation docs) {
			if (docs[0]["age"] != 16) {
				res = false;
				message.append("Unexpected value at [0][\"key\"]");
			}
		});

		if (res == false)
			return std::make_tuple(res, message);
	}
};

class DS_Tests : public ATest {
	DS_Tests(): ATest() {}

	virtual void	launchTests() {
		std::cout << "Testing Datastore module:" << std::endl;

		for (auto &fct: _fcts) {
			bool		res;
			std::string	message;
			
			std::tie(res, message) = fct();
			if (!res) {
				std::cout << "Fail: " << message << std::endl;
			} else {
				std::cout << "Test passed" << std::endl;
			}
		}
	}
};


DS_Tests	__g__ds_test();