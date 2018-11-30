
#if !defined(LALDB_DATASTORE)
#define LALDB_DATASTORE

#include <list>
#include "DataRepresentation/DataRepresentation.hpp"

namespace laldb {

class DataQuery;

class Datastore {
private:
	std::list<DataRepresentation>	_datas;
public:
	Datastore() {};
	~Datastore() = default;
	struct UpdateOptions {
		bool create = true;
		bool multi = true;
	};
	using Error = bool;

	auto const &getAllData(void) const {return _datas;}

	void		insert(
		DataRepresentation const &query,
		std::function<void(Error error, DataRepresentation const newDoc)> callback);
	void		count(
		DataRepresentation const &query,
		std::function<void(Error error, long count)> callback) const;
	void		find(
		DataRepresentation const &query,
		std::function<void(Error error, DataRepresentation const docs)> callback) const;
	DataQuery	find(DataRepresentation const &query) const;
	void		findOne(
		DataRepresentation const &query,
		std::function<void(Error error, DataRepresentation const doc)> callback) const;
	void		update(
		DataRepresentation const &query,
		DataRepresentation const &newData,
		UpdateOptions const &options,
		std::function<void(Error error, DataRepresentation const updatedDocs)> callback);
};

class DataQuery {
public:
	DataQuery(Datastore const &store, bool valid = true);
	DataQuery(Datastore const &store, DataRepresentation const &query);

public:
	bool		valid(void) const {return _valid;};
	DataQuery	where(DataRepresentation const &query) const;
	DataQuery	sort(DataRepresentation const &query) const;
	DataQuery	limit(std::size_t n) const;
	DataQuery	skip(std::size_t n) const;

	void		launch(std::function<void(Datastore::Error error, DataRepresentation const updatedDocs)> callback) const;

private:
	Datastore const		*_store;
	bool				_valid = true;
	std::vector<DataRepresentation>	_queries;
	DataRepresentation	_sort;
	std::size_t			_skip = -1;
	std::size_t			_limit = -1;
};

}

#endif // LALDB_DATASTORE
