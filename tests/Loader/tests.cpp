#include <iostream>
#include <functional>
#include <tuple>
#include <chrono>
#include "laldb.hpp"
#include "../ATest.hpp"

static std::list<std::function<std::tuple<bool, std::string>()>> _fcts {
	[] () -> std::tuple<bool, std::string> {
		laldb::Loader		l;

		l.load("Salut.db");

		auto	&ds__ = l["users"];

		auto	&ds = l["users"];
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
	}
};

class Loader_Tests : public ATest {
public:
	Loader_Tests(): ATest() {}

	virtual void	launchTests() {
		std::cout << "Testing Loader module:" << std::endl;

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
				std::cout << "Test passed (" << elapsed << " ns)" << std::endl;
			}
		}
	}
};

static Loader_Tests __ptr;