
#if !defined(LALDB_DATASTORE)
#define LALDB_DATASTORE

#include <list>
#include "DataRepresentation/DataRepresentation.hpp"

namespace laldb {

class Datastore {
private:
	std::list<DataRepresentation>	_datas;
public:
	Datastore() {};
	struct UpdateOptions {
		bool create = true;
		bool multi = true;
	};
	using Error = bool;

	void	insert(
		DataRepresentation const &query,
		std::function<void(Error error, DataRepresentation const newDoc)> callback);
	void	find(
		DataRepresentation const &query,
		std::function<void(Error error, DataRepresentation const docs)> callback);
	void	findOne(
		DataRepresentation const &query,
		std::function<void(Error error, DataRepresentation const doc)> callback);
	void	update(
		DataRepresentation const &query,
		DataRepresentation const &newData,
		UpdateOptions const &options,
		std::function<void(Error error, DataRepresentation const updatedDocs)> callback);
};

}

#endif // LALDB_DATASTORE
