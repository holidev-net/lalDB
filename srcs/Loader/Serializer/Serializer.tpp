#if !defined(_LALDB_SERILIZER_INTERNAL_TPP)
#define _LALDB_SERILIZER_INTERNAL_TPP

#include "Serializer.hpp"
#include "../_internal.h"
#include "../_Loader.h"

namespace laldb {

#pragma pack(push, 4)

DECORATED_STRUCT(BaseSaveStruct, {
	DataRepresentation::Type	type;
	std::uint64_t				length;
}); 

#pragma pack(pop)

/* STRING */

std::uint64_t	Serializer<::laldb::String>::serialize(std::FILE *stream, ::laldb::String const &data) {
	BaseSaveStruct s {
		DataRepresentation::Type::STR,
		data.get().size() + 1
	};

	std::fwrite(&s, sizeof(s), 1, stream);
	std::fwrite(data.get().c_str(), static_cast<std::size_t>(s.length), 1, stream);
	return getSize(data) + 1;
}

void		*Serializer<::laldb::String>::parse(void *data, ::laldb::String &res) {
	auto	&str = res.get();
	auto	*s = reinterpret_cast<BaseSaveStruct*>(data);

	str.replace(0, str.size(), reinterpret_cast<char*>(s + 1), static_cast<std::size_t>(s->length));
	return reinterpret_cast<void*>(
		reinterpret_cast<std::uintptr_t>(data) +
		Serializer<typename std::remove_reference<decltype(res)>::type>::getSize(res));
}

std::uint64_t	Serializer<::laldb::String>::getSize(::laldb::String const &target) {
	return (sizeof(BaseSaveStruct) + target.get().size());
}

/* OBJECT */

std::uint64_t	Serializer<::laldb::Object>::serialize(std::FILE *stream, ::laldb::Object const &data) {
	auto pos = std::ftell(stream);
	BaseSaveStruct s {
		DataRepresentation::Type::OBJ,
		sizeof(std::uint64_t)
	};
	std::fwrite(&s, sizeof(s), 1, stream);
	std::uint64_t nbElem = data.get().size();
	std::fwrite(&nbElem, sizeof(nbElem), 1, stream);
	for (auto &obj : data.get()) {
		s.length += serializeChamp(stream, obj.first);
		switch (obj.second.getType()) {
				case DataRepresentation::Type::NBR:
					s.length += Serializer<Number>::serialize(stream, obj.second.getData<Number>());
					break;
				case DataRepresentation::Type::STR:
					s.length += Serializer<String>::serialize(stream, obj.second.getData<String>());
					break;
				case DataRepresentation::Type::BUF:
					s.length += Serializer<Buffer>::serialize(stream, obj.second.getData<Buffer>());
					break;
				case DataRepresentation::Type::ARR:
					s.length += Serializer<Array>::serialize(stream, obj.second.getData<Array>());
					break;
				case DataRepresentation::Type::OBJ:
					s.length += Serializer<Object>::serialize(stream, obj.second.getData<Object>());
					break;
				case DataRepresentation::Type::BOL:
					s.length += Serializer<Bool>::serialize(stream, obj.second.getData<Bool>());
					break;
				case DataRepresentation::Type::NUL:
					s.length += Serializer<Null>::serialize(stream, obj.second.getData<Null>());
					break;
				default:
					throw std::runtime_error{"error : fail to serialize value."};
			}
	}
	/* rewrite the header with real size */
	if (std::fseek(stream, pos, SEEK_SET) == -1 ||
		std::fwrite(&s, sizeof(s), 1, stream) == -1 ||
		std::fseek(stream, 0, SEEK_END) == -1) abort();
	return (sizeof(BaseSaveStruct) + s.length);
}

void		*Serializer<::laldb::Object>::parse(void *data, ::laldb::Object &res) {
	auto	&obj = res.get();
	auto	*s = reinterpret_cast<BaseSaveStruct*>(data);
	auto	nbElem = *reinterpret_cast<std::uint64_t*>(s + 1);

	data = reinterpret_cast<double*>(s + 1) + 1;
	for (auto i = 0; i < nbElem; ++i) {
		DataRepresentation key("");
		data = Serializer<String>::parse(data, key.getData<String>());
		auto	dataPtr = reinterpret_cast<DataRepresentation::Type*>(data);
		DataRepresentation	value(*dataPtr);
		switch (*dataPtr) {
				case DataRepresentation::Type::NBR:
					data = Serializer<Number>::parse(dataPtr, value.getData<Number>());
					break;
				case DataRepresentation::Type::STR:
					data = Serializer<String>::parse(dataPtr, value.getData<String>());
					break;
				case DataRepresentation::Type::BUF:
					data = Serializer<Buffer>::parse(dataPtr, value.getData<Buffer>());
					break;
				case DataRepresentation::Type::ARR:
					data = Serializer<Array>::parse(dataPtr, value.getData<Array>());
					break;
				case DataRepresentation::Type::OBJ:
					data = Serializer<Object>::parse(dataPtr, value.getData<Object>());
					break;
				case DataRepresentation::Type::BOL:
					data = Serializer<Bool>::parse(dataPtr, value.getData<Bool>());
					break;
				case DataRepresentation::Type::NUL:
					data = Serializer<Null>::parse(dataPtr, value.getData<Null>());
					break;
				default:
					throw std::runtime_error{"error : fail to parse value."};
			}
		obj[key.value<laldb::String>()] = value;
	}
	return data;
}

std::uint64_t	Serializer<::laldb::Object>::serializeChamp(std::FILE *stream, std::string const &data)
{
	DataRepresentation obj(data);

	return Serializer<::laldb::String>::serialize(stream, obj.value<laldb::String>());
}

/* ARRAY */

std::uint64_t	Serializer<::laldb::Array>::serialize(std::FILE *stream, ::laldb::Array const &data) {
	auto pos = std::ftell(stream);
	BaseSaveStruct s {
		DataRepresentation::Type::ARR,
		sizeof(std::uint64_t)
	};
	std::fwrite(&s, sizeof(s), 1, stream);
	std::uint64_t nbElem = data.get().size();
	std::fwrite(&nbElem, sizeof(nbElem), 1, stream);
	for (auto &obj : data.get()) {
		switch (obj.getType()) {
				case DataRepresentation::Type::NBR:
					s.length += Serializer<Number>::serialize(stream, obj.getData<Number>());
					break;
				case DataRepresentation::Type::STR:
					s.length += Serializer<String>::serialize(stream, obj.getData<String>());
					break;
				case DataRepresentation::Type::BUF:
					s.length += Serializer<Buffer>::serialize(stream, obj.getData<Buffer>());
					break;
				case DataRepresentation::Type::ARR:
					s.length += Serializer<Array>::serialize(stream, obj.getData<Array>());
					break;
				case DataRepresentation::Type::OBJ:
					s.length += Serializer<Object>::serialize(stream, obj.getData<Object>());
					break;
				case DataRepresentation::Type::BOL:
					s.length += Serializer<Bool>::serialize(stream, obj.getData<Bool>());
					break;
				case DataRepresentation::Type::NUL:
					s.length += Serializer<Null>::serialize(stream, obj.getData<Null>());
					break;
				default:
					throw std::runtime_error{"error : fail to serialize value."};
			}
	}
	/* rewrite the header with real size */
	if (std::fseek(stream, pos, SEEK_SET) == -1 ||
		std::fwrite(&s, sizeof(s), 1, stream) == -1 ||
		std::fseek(stream, 0, SEEK_END) == -1) abort();
	return (sizeof(BaseSaveStruct) + s.length);
}

void	*Serializer<::laldb::Array>::parse(void *data, ::laldb::Array &res) {
	auto	&arr = res.get();
	auto	*s = reinterpret_cast<BaseSaveStruct*>(data);
	auto	nbElem = *reinterpret_cast<std::uint64_t*>(s + 1);

	data = reinterpret_cast<double*>(s + 1) + 1;
	for (auto i = 0; i < nbElem; ++i) {
		auto	dataPtr = reinterpret_cast<DataRepresentation::Type*>(data);
		DataRepresentation	obj(*dataPtr);
		switch (*dataPtr) {
				case DataRepresentation::Type::NBR:
					data = Serializer<Number>::parse(dataPtr, obj.getData<Number>());
					break;
				case DataRepresentation::Type::STR:
					data = Serializer<String>::parse(dataPtr, obj.getData<String>());
					break;
				case DataRepresentation::Type::BUF:
					data = Serializer<Buffer>::parse(dataPtr, obj.getData<Buffer>());
					break;
				case DataRepresentation::Type::ARR:
					data = Serializer<Array>::parse(dataPtr, obj.getData<Array>());
					break;
				case DataRepresentation::Type::OBJ:
					data = Serializer<Object>::parse(dataPtr, obj.getData<Object>());
					break;
				case DataRepresentation::Type::BOL:
					data = Serializer<Bool>::parse(dataPtr, obj.getData<Bool>());
					break;
				case DataRepresentation::Type::NUL:
					data = Serializer<Null>::parse(dataPtr, obj.getData<Null>());
					break;
				default:
					throw std::runtime_error{"error : fail to parse value."};
			}
		arr.push_back(obj);
	}
	return data;
}

/* BOOL */

std::uint64_t	Serializer<::laldb::Bool>::serialize(std::FILE *stream, ::laldb::Bool const &data) {
	auto value = data.get();
	BaseSaveStruct s {
		DataRepresentation::Type::BOL,
		sizeof(value)
	};

	std::fwrite(&s, sizeof(s), 1, stream);
	std::fwrite(&value, sizeof(value), 1, stream);
	return getSize(data);
}

void	*Serializer<::laldb::Bool>::parse(void *data,::laldb::Bool &res) {
	auto	*v = reinterpret_cast<decltype(res.get())*>(
		reinterpret_cast<std::uintptr_t>(data) +
		sizeof(BaseSaveStruct));

	res.set(*v);
	return reinterpret_cast<void*>(
		reinterpret_cast<std::uintptr_t>(data) +
		Serializer<typename std::remove_reference<decltype(res)>::type>::getSize(res));
}

std::uint64_t	Serializer<::laldb::Bool>::getSize(::laldb::Bool const &target) {
	return (sizeof(BaseSaveStruct) + sizeof(target.get()));
}

/* BUFFER */

std::uint64_t	Serializer<::laldb::Buffer>::serialize(std::FILE *stream, ::laldb::Buffer const &data) {
	BaseSaveStruct s {
		DataRepresentation::Type::BUF,
		data.size()
	};

	std::fwrite(&s, sizeof(s), 1, stream);
	std::fwrite(data.get(), static_cast<std::size_t>(data.size()), 1, stream);
	return getSize(data);
}

void		*Serializer<::laldb::Buffer>::parse(void *data, ::laldb::Buffer &res) {
	auto	*s = reinterpret_cast<BaseSaveStruct*>(data);

	res.set(reinterpret_cast<char*>(
		reinterpret_cast<std::uintptr_t>(data) +
		sizeof(*s)), static_cast<std::size_t>(s->length));

	return reinterpret_cast<void*>(
		reinterpret_cast<std::uintptr_t>(data) +
		Serializer<typename std::remove_reference<decltype(res)>::type>::getSize(res));
}

std::uint64_t	Serializer<::laldb::Buffer>::getSize(::laldb::Buffer const &target) {
	
}

/* NUMBER */

std::uint64_t	Serializer<::laldb::Number>::serialize(std::FILE *stream, ::laldb::Number const &data) {
	auto value = data.get();
	BaseSaveStruct s {
		DataRepresentation::Type::NBR,
		sizeof(value)
	};

	std::fwrite(&s, sizeof(s), 1, stream);
	std::fwrite(&value, sizeof(value), 1, stream);
	return getSize(data);
}

void		*Serializer<::laldb::Number>::parse(void *data, ::laldb::Number &res) {
	res.set(*reinterpret_cast<double*>(reinterpret_cast<BaseSaveStruct*>(data) + 1));
	return reinterpret_cast<void*>(
		reinterpret_cast<std::uintptr_t>(data) +
		Serializer<typename std::remove_reference<decltype(res)>::type>::getSize(res));
}

std::uint64_t	Serializer<::laldb::Number>::getSize(::laldb::Number const &target) {
	return (sizeof(BaseSaveStruct) + sizeof(double));
}

/* SERIALIZE */

std::uint64_t	Serializer<::laldb::Null>::serialize(std::FILE *stream, ::laldb::Null const &data) {
	BaseSaveStruct s {
		DataRepresentation::Type::NUL,
		0
	};

	std::fwrite(&s, sizeof(s), 1, stream);
	return getSize(data);
}

void		*Serializer<::laldb::Null>::parse(void *data, ::laldb::Null &res) {
	return reinterpret_cast<void*>(
		reinterpret_cast<std::uintptr_t>(data) +
		Serializer<typename std::remove_reference<decltype(res)>::type>::getSize(res));
}

std::uint64_t	Serializer<::laldb::Null>::getSize(::laldb::Null const &target) {
	return (sizeof(BaseSaveStruct));
}

/* BASE SERIALIZER */

BaseSerializer::BaseSerializer(std::string const &filename):
_filename{filename}
{
}

BaseSerializer::~BaseSerializer()
{
	std::fclose(_stream);
}

void BaseSerializer::verifMode(std::string const &mode)
{
	if (_mode.empty() && (mode == "r" || mode == "w")) {
		_mode = mode;
		_stream = std::fopen(_filename.c_str(), _mode.c_str());
		if (_stream == nullptr)
			throw std::runtime_error{"error : fail to open " + _filename};
		if (mode == "w") {
			LaldbFileHeader header{FILEID, MAGIC, 1, 0, 0};
			std::fwrite(&header, sizeof(header), 1, _stream);
		} else {
			LaldbFileHeader header;
			if (std::fread(&header, sizeof(header), 1, _stream) != 1
				|| header.id != FILEID || header.magic != MAGIC)
				throw std::runtime_error{"error : " + _filename + " is not a laldb file."};
		}
	} else if (_mode != mode) {
		throw std::runtime_error{"error : fail to use serializer."};
	}
}

void	BaseSerializer::serialize(DataRepresentation const &data) {
	verifMode("w");

	switch (data.getType()) {
		case DataRepresentation::Type::NBR:
			laldb::Serializer<laldb::Number>::serialize(_stream, data.getData<Number>());
			break;
		case DataRepresentation::Type::STR:
			laldb::Serializer<laldb::String>::serialize(_stream, data.getData<String>());
			break;
		case DataRepresentation::Type::BUF:
			laldb::Serializer<laldb::Buffer>::serialize(_stream, data.getData<Buffer>());
			break;
		case DataRepresentation::Type::ARR:
			laldb::Serializer<laldb::Array>::serialize(_stream, data.getData<Array>());
			break;
		case DataRepresentation::Type::OBJ:
			laldb::Serializer<laldb::Object>::serialize(_stream, data.getData<Object>());
			break;
		case DataRepresentation::Type::BOL:
			laldb::Serializer<laldb::Bool>::serialize(_stream, data.getData<Bool>());
			break;
		case DataRepresentation::Type::NUL:
			laldb::Serializer<laldb::Null>::serialize(_stream, data.getData<Null>());
			break;
		default:
			throw std::runtime_error{"error : fail to parse file."};
	}
}

void	BaseSerializer::parse(DataRepresentation &data) {
	verifMode("r");
	BaseSaveStruct st[1];

	/* parse header file */
	data = getNextData();
/*		if (data.isString())
	laldb::Serializer<laldb::String>::parse(file, data.getData<String>());
	else if (data.isNumber())
	laldb::Serializer<laldb::Number>::parse(file, data.getData<Number>());*/
}

DataRepresentation BaseSerializer::getNextData()
{
	DataRepresentation obj;
	BaseSaveStruct st[1];

	if (std::fread(st, sizeof(st), 1, _stream) != 1)
		throw std::runtime_error{"error : file is truncated (base struct error)."};

	std::uint8_t data[sizeof(st) + st->length];

	std::memcpy(data, reinterpret_cast<std::uint8_t*>(st), sizeof(st));
	auto a = std::fread(data + sizeof(st), sizeof(std::uint8_t), st->length, _stream);
	if (a != st->length)
		throw std::runtime_error{"error : file is truncated."};
	switch (st->type) {
		case DataRepresentation::Type::NBR:
			obj = DataRepresentation(st->type);
			Serializer<Number>::parse(data, obj.getData<Number>());
			break;
		case DataRepresentation::Type::STR:
			obj = DataRepresentation(st->type);
			Serializer<String>::parse(data, obj.getData<String>());
			break;
		case DataRepresentation::Type::BUF:
			obj = DataRepresentation(st->type);
			Serializer<Buffer>::parse(data, obj.getData<Buffer>());
			break;
		case DataRepresentation::Type::ARR:
			obj = DataRepresentation(st->type);
			Serializer<Array>::parse(data, obj.getData<Array>());
			break;
		case DataRepresentation::Type::OBJ:
			obj = DataRepresentation(st->type);
			Serializer<Object>::parse(data, obj.getData<Object>());
			break;
		case DataRepresentation::Type::BOL:
			obj = DataRepresentation(st->type);
			Serializer<Bool>::parse(data, obj.getData<Bool>());
			break;
		case DataRepresentation::Type::NUL:
			obj = DataRepresentation(st->type);
			Serializer<Null>::parse(data, obj.getData<Null>());
			break;
		default:
			throw std::runtime_error{"error : fail to parse file."};
	}
	return obj;
}

}

#endif // _LALDB_SERILIZER_INTERNAL_TPP