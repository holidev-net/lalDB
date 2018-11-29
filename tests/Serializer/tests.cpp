#include <iostream>
#include <functional>
#include <tuple>
#include <chrono>
#include "laldb.hpp"
#include "../../srcs/Loader/Serializer/Serializer.hpp"
#include "../ATest.hpp"

void printObj(laldb::DataRepresentation &obj, int decal = 0)
{
	if (decal == 0)
		std::cout << "\nprinted obj :\n";
	for (int i = 0; i < decal; ++i)
		std::cout << '\t';
	switch (obj.getType()) {
		case laldb::DataRepresentation::Type::NBR:
			std::cout << obj.value<laldb::Number>() << std::endl;
			break;
		case laldb::DataRepresentation::Type::STR:
			std::cout << obj.value<laldb::String>() << std::endl;
			break;
		case laldb::DataRepresentation::Type::BUF:
			break;
		case laldb::DataRepresentation::Type::ARR:
			std::cout << "Array : " << std::endl;
			for (auto &e : obj)
				printObj(e, decal + 1);
			break;
		case laldb::DataRepresentation::Type::OBJ:
			std::cout << "Object : " << std::endl;
			for (auto &e : obj.value<laldb::Object>()) {
				std::cout << e.first << " : ";
				printObj(e.second, decal + 1);
			}
			break;
		case laldb::DataRepresentation::Type::BOL:
			
			break;
		case laldb::DataRepresentation::Type::NUL:
			
			break;
		default:
			std::cout << "fail to print" << std::endl;break;
	}
}

static std::list<std::function<std::tuple<bool, std::string>()>> _fcts {
	[] () -> std::tuple<bool, std::string> {
		{
			laldb::BaseSerializer ser("./test.db");
			laldb::DataRepresentation obj = laldb::makeObject({
				{"ok", "bonjour"},
				{"ah", laldb::makeArray{
					"je suis dans un tableau qui est dans un objet",
					laldb::makeObject{{"ah", "on part sur de la recursivité !"}}
				}},
				{"non", 42}
				});
			ser.serialize(obj);
		}
		{
			laldb::BaseSerializer deser("./test.db");
			laldb::DataRepresentation obj;
			deser.parse(obj);
			printObj(obj);
		}

		return std::make_tuple(true, "");
	}
};

class Serializer_Tests : public ATest {
public:
	Serializer_Tests(): ATest() {}

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

static Serializer_Tests __ptr;