#include <iostream>
#include <functional>
#include <tuple>
#include <chrono>
#include "Datastore/Datastore.hpp"
#include "../ATest.hpp"

static std::list<std::function<std::tuple<bool, std::string>()>> _fcts {
	[] () -> std::tuple<bool, std::string> {
		laldb::Datastore	ds;
		bool res = true;
		std::string message;

		ds.insert(laldb::makeArray {
			laldb::makeObject {{ "name", "Clement" }, { "lastName", "A" }, { "age", 16 }},
			laldb::makeObject {{ "name", "Paul" }, { "lastName", "A" }, { "age", 32 }},
			laldb::makeObject {{ "name", "Jean" }, { "lastName", "A" }, { "age", 32 }},
			laldb::makeObject {{ "name", "Clement" }, { "lastName", "B" }, { "age", 20 }},
			laldb::makeObject {{ "name", "Cyril" }, { "lastName", "A" }, { "age", 32 }},
			laldb::makeObject {{ "name", "Paul" }, { "lastName", "B" }, { "age", 42 }},
			laldb::makeObject {{ "name", "Jean" }, { "lastName", "B" }, { "age", 42 }},
			laldb::makeObject {{ "name", "Oui" }, { "lastName", "C" }, { "age", 32 }},
			laldb::makeObject {{ "name", "Non" }, { "lastName", "D" }, { "age", 32 }}
			}, [&] (laldb::Datastore::Error err, laldb::DataRepresentation doc) {
			res = !err;
			if (err) {
				message.append("insert return an error");
			}
		});
		if (res == false)
			return std::make_tuple(res, message);

		auto bla = ds.find(laldb::makeObject{{ "age", 32 }});

		bla.skip(1).limit(3).launch([&] (laldb::Datastore::Error err, laldb::DataRepresentation docs) {
			if (err == true || docs.isArray() == false) {
				std::cerr << "ERROR : " <<  docs.value<laldb::String>() << std::endl;
				return;
			}
			for (auto &doc : docs) {
				std::cout << doc["name"].value<laldb::String>() << " " << doc["lastName"].value<laldb::String>() << " " << doc["age"].value<laldb::Number>() << std::endl;
			}
		});
		return std::make_tuple(true, "");
	}
};

class DQ_Tests : public ATest {
public:
	DQ_Tests(): ATest() {}

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
				std::cout << "Test passed (" << elapsed << " ns)" << std::endl;
			}
		}
	}
};

static DQ_Tests __ptr;