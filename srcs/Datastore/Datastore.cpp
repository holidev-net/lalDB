#include "Datastore/Datastore.hpp"

namespace laldb {

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

//TODO: make a global find function (or class)
void	Datastore::find(
DataRepresentation const &query,
std::function<void(Error error, DataRepresentation const docs)> callback) {
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
			if (!(it != iMap.end() && it->second == qField.second)) {
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

void	Datastore::findOne(
DataRepresentation const &query,
std::function<void(Error error, DataRepresentation const doc)> callback) {
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
			if (!(it != iMap.end() && it->second == qField.second)) {
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
			if (!(it != iMap.end() && it->second == qField.second)) {
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

}