#include "Datastore/Datastore.hpp"

#include <iostream>
namespace laldb {

bool cmpData(DataRepresentation const &data, DataRepresentation const &other) {
	if (other.isFunction())
		return other(data);
	return data == other;
}


//TODO: Store iterator for a faster remove;
void	Datastore::insert(
DataRepresentation const &query,
std::function<void(Error error, DataRepresentation const newDoc)> callback) {
	if (query.isObject()) {
		_datas.push_back(query);
		callback(false, query);
	} else if (query.isArray()) {
		for (auto &i: query.value<::laldb::Array>()) {
			if (i.isObject()) {
				_datas.push_back(i);
				callback(false, i);
			} else {
				callback(true, ::laldb::makeObject {
					{ "message", "query need to be an Object" },
					{ "query", i }
				});
			}
		}
	}
}

void	Datastore::count(
DataRepresentation const &query,
std::function<void(Error error, long count)> callback) const {
	if (query.isObject() == false) {
		callback(true, -1);
		return;
	}
	long count = 0;
	const auto &qMap = query.value<::laldb::Object>();

	for (auto &i: _datas) {
		const auto &iMap = i.value<::laldb::Object>();
		bool valid = true;

		for (auto &qField: qMap) {
			auto it = iMap.find(qField.first);
			if (!(it != iMap.end() && cmpData(it->second, qField.second))) {
				valid = false;
				break;
			}
		}
		if (valid) {
			count += 1;
		}
	}
	callback(false, count);
}

//TODO: make a global find function (or class)
void	Datastore::find(
DataRepresentation const &query,
std::function<void(Error error, DataRepresentation const docs)> callback) const {
	if (query.isObject() == false) {
		callback(true, "query need to be an Object");
		return;
	}
	auto result = DataRepresentation::newArray();
	const auto &qMap = query.value<::laldb::Object>();

	for (auto &i: _datas) {
		const auto &iMap = i.value<::laldb::Object>();
		bool valid = true;

		for (auto &qField: qMap) {
			auto it = iMap.find(qField.first);
			if (!(it != iMap.end() && cmpData(it->second, qField.second))) {
				valid = false;
				break;
			}
		}
		if (valid) {
			result.push(i);
		}
	}
	callback(false, result);
}

DataQuery	Datastore::find(
DataRepresentation const &query) const {
	if (query.isObject() == false)
		return DataQuery(*this, false);
	return DataQuery{*this, query};
}

void	Datastore::findOne(
DataRepresentation const &query,
std::function<void(Error error, DataRepresentation const doc)> callback) const {
	if (query.isObject() == false) {
		callback(true, "query need to be an Object");
		return;
	}
	const auto &qMap = query.value<::laldb::Object>();

	for (auto &i: _datas) {
		const auto &iMap = i.value<::laldb::Object>();
		bool valid = true;

		for (auto &qField: qMap) {
			auto it = iMap.find(qField.first);
			if (!(it != iMap.end() && cmpData(it->second, qField.second))) {
				valid = false;
				break;
			}
		}
		if (valid) {
			callback(false, i);
			return;
		}
	}
}

void	Datastore::update(
DataRepresentation const &query,
DataRepresentation const &newData,
UpdateOptions const &options,
std::function<void(Error error, DataRepresentation const updatedDocs)> callback) {
	if (query.isObject() == false) {
		callback(true, "query need to be an Object");
		return;
	}
	if (newData.isObject() == false) {
		callback(true, "new data need to be an Object");
		return;
	}
	auto result = DataRepresentation::newArray();
	const auto &qMap = query.value<::laldb::Object>();

	for (auto &i: _datas) {
		const auto &iMap = i.value<::laldb::Object>();
		bool valid = true;

		for (auto &qField: qMap) {
			auto it = iMap.find(qField.first);
			if (!(it != iMap.end() && cmpData(it->second, qField.second))) {
				valid = false;
				break;
			}
		}
		if (valid) {
			result.push(i);
			if (options.multi == false)
				break;
		}
	}
	if (result.value<::laldb::Array>().size() == 0) {
		_datas.push_back(query);
		result.push(query);
	} else {
		for (auto &i: result.value<::laldb::Array>())
			for (auto &q: newData.value<::laldb::Object>())
				i[q.first] = q.second;
	}
	callback(false, result);
}

DataQuery::DataQuery(Datastore const &store, bool valid):
_store{&store},
_valid{valid}
{}

DataQuery::DataQuery(Datastore const &store, DataRepresentation const &query):
_store{&store},
_valid(query.isObject()),
_queries{query}
{}

DataQuery	DataQuery::where(DataRepresentation const &query) const
{
	if (valid() == false || query.isObject() == false)
		return DataQuery{*_store, false};
	DataQuery dataQuery{*this};
	dataQuery._queries.push_back(query);
	return dataQuery;
}

DataQuery	DataQuery::sort(DataRepresentation const &query) const
{
	if (valid() == false || query.isObject() == false)
		return DataQuery{*_store, false};
	DataQuery dataQuery{*this};
	dataQuery._sort = query;
	return dataQuery;
}

DataQuery	DataQuery::limit(long n) const
{
	if (valid() == false || n < -1)
		return DataQuery{*_store, false};
	DataQuery dataQuery{*this};
	dataQuery._limit = n;
	return dataQuery;
}

DataQuery	DataQuery::skip(long n) const
{
	if (valid() == false || n < -1)
		return DataQuery{*_store, false};
	DataQuery dataQuery{*this};
	dataQuery._skip = n;
	return dataQuery;
}

void	DataQuery::launch(std::function<void(Datastore::Error error, DataRepresentation const updatedDocs)> callback) const
{
	if (valid() == false) {
		callback(true, "query is not valid");
		return;
	}
	auto result = DataRepresentation::newArray();
	auto skip = _skip;
	auto limit = _limit;
	for (auto &i: _store->getAllData()) {
		const auto &iMap = i.value<::laldb::Object>();
		bool valid = true;

		for (auto &query : _queries) {
			for (auto &qField: query.value<::laldb::Object>()) {
				auto it = iMap.find(qField.first);
				if (!(it != iMap.end() && cmpData(it->second, qField.second))) {
					valid = false;
					break;
				}
			}
			if (valid == false)
				break;
		}
		if (valid) {
			if (skip > 0) {
				--skip;
			} else if (limit != 0) {
				result.push(i);
				if (limit > 0)
					--limit;
			}
			if (limit == 0)
				break;
		}
	}
	std::cerr << "sizeof result = " << result.value<Array>().size() << std::endl;
	callback(false, result);
}

}