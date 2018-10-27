#include <iostream>
#include <functional>
#include <tuple>
#include <chrono>
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
		return std::make_tuple(true, "");
	},
	[] () -> std::tuple<bool, std::string> {
		laldb::Datastore	ds;
		bool res = true;
		std::string message;

		ds.insert(laldb::makeArray { laldb::makeObject {
			{ "name", "salut" },
			{ "age", 16 }
		}, laldb::makeObject {
			{ "name", "paul" },
			{ "age", 32 }
		}, laldb::makeObject {
			{ "name", "jean" },
			{ "age", 42 }
		}}, [&] (laldb::Datastore::Error err, laldb::DataRepresentation doc) {
			res = !err;
			if (err) {
				message.append("insert return an error");
			}
		});
		if (res == false)
			return std::make_tuple(res, message);
		
		ds.find(laldb::makeObject {
			{ "name", "paul" }
		}, [&] (laldb::Datastore::Error err, laldb::DataRepresentation docs) {
			if (docs[0]["age"] != 32) {
				res = false;
				message.append("Unexpected value at [0][\"key\"]");
			}
		});

		if (res == false)
			return std::make_tuple(res, message);
		return std::make_tuple(true, "");
	}
};

class DS_Tests : public ATest {
public:
	DS_Tests(): ATest() {}

	virtual void	launchTests() {
		std::cout << "Testing Datastore module:" << std::endl;

		for (auto &fct: _fcts) {
			bool		res;
			std::string	message;
			
			std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
			start = std::chrono::high_resolution_clock::now();
			std::tie(res, message) = fct();
			end = std::chrono::high_resolution_clock::now();
			auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
			if (!res) {
				std::cout << "Fail: " << message << std::endl;
			} else {
				std::cout << "Test passed in (" << elapsed << " ns)" << std::endl;
			}
		}
	}
};

DS_Tests __ptr;