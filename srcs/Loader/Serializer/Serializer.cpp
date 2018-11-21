#include "Serializer.hpp"
#include "_internal.h"

namespace laldb {

#pragma pack(push, 4)

DECORATED_STRUCT(BaseSaveStruct, {
	DataRepresentation::Type	type;
	std::uint64_t				length;
}); 

#pragma pack(pop)

template <>
class Serializer<::laldb::String> {
	static void		serialize(std::FILE *stream, ::laldb::String const &data) {
		BaseSaveStruct s {
			DataRepresentation::Type::STR,
			data.get().size()
		};

		fwrite(&s, sizeof(s), 1, stream);
		fwrite(data.get().c_str(), static_cast<std::size_t>(s.length), 1, stream);
	}

	static void		*parse(void *data, ::laldb::String &res) {
		auto	&str = res.get();
		auto	*s = reinterpret_cast<BaseSaveStruct*>(data);

		str.copy(reinterpret_cast<char*>(s + 1), static_cast<std::size_t>(s->length));
		return reinterpret_cast<void*>(
			reinterpret_cast<std::uintptr_t>(data) +
			Serializer<typename std::remove_reference<decltype(res)>::type>::getSize(res));
	}

	static std::uint64_t	getSize(::laldb::String const &target) {
		return (sizeof(BaseSaveStruct) + target.get().size());
	}
};

template <>
class Serializer<::laldb::Object> {
	static void		serialize(std::FILE *stream, ::laldb::Object const &data) {
		BaseSaveStruct s {
			DataRepresentation::Type::STR,
			0
		};

		for (auto &cur: data.get()) {
			
		}
	}

	static void		*parse(void *data, ::laldb::Object &res) {

	}

	static std::uint64_t	getSize(::laldb::Object const &target) {
		
	}
};

template <>
class Serializer<::laldb::Array> {
	static void		serialize(std::FILE *stream, ::laldb::Array const &data) {
		
	}

	static void		*parse(void *data, ::laldb::Array &res) {

	}

	static std::uint64_t	getSize(::laldb::Array const &target) {
		
	}
};

template <>
class Serializer<::laldb::Bool> {
	static void		serialize(std::FILE *stream, ::laldb::Bool const &data) {
		auto value = data.get();
		BaseSaveStruct s {
			DataRepresentation::Type::BOL,
			sizeof(value)
		};

		fwrite(&s, sizeof(s), 1, stream);
		fwrite(&value, sizeof(bool), 1, stream);
	}

	static void		*parse(void *data,::laldb::Bool &res) {
		auto	*v = reinterpret_cast<decltype(res.get())*>(
			reinterpret_cast<std::uintptr_t>(data) +
			sizeof(BaseSaveStruct));

		res.set(*v);
		return reinterpret_cast<void*>(
			reinterpret_cast<std::uintptr_t>(data) +
			Serializer<typename std::remove_reference<decltype(res)>::type>::getSize(res));
	}

	static std::uint64_t	getSize(::laldb::Bool const &target) {
		return (sizeof(BaseSaveStruct) + sizeof(bool));
	}
};

template <>
class Serializer<::laldb::Buffer> {
	static void		serialize(std::FILE *stream, ::laldb::Buffer const &data) {
		BaseSaveStruct s {
			DataRepresentation::Type::BUF,
			data.size()
		};

		fwrite(&s, sizeof(s), 1, stream);
		fwrite(data.get(), static_cast<std::size_t>(data.size()), 1, stream);
	}

	static void		*parse(void *data, ::laldb::Buffer &res) {
		auto	*s = reinterpret_cast<BaseSaveStruct*>(data);

		res.set(reinterpret_cast<char*>(
			reinterpret_cast<std::uintptr_t>(data) +
			sizeof(*s)), static_cast<std::size_t>(s->length));

		return reinterpret_cast<void*>(
			reinterpret_cast<std::uintptr_t>(data) +
			Serializer<typename std::remove_reference<decltype(res)>::type>::getSize(res));
	}

	static std::uint64_t	getSize(::laldb::Buffer const &target) {
		
	}
};

template <>
class Serializer<::laldb::Number> {
	static void		serialize(std::FILE *stream, ::laldb::Number const &data) {
		auto value = data.get();
		BaseSaveStruct s {
			DataRepresentation::Type::NBR,
			sizeof(value)
		};

		fwrite(&s, sizeof(s), 1, stream);
		fwrite(&value, sizeof(bool), 1, stream);
	}

	static void		*parse(void *data, ::laldb::Number &res) {
		auto	*v = reinterpret_cast<decltype(res.get())*>(
			reinterpret_cast<std::uintptr_t>(data) +
			sizeof(BaseSaveStruct));

		res.set(*v);
		return reinterpret_cast<void*>(
			reinterpret_cast<std::uintptr_t>(data) +
			Serializer<typename std::remove_reference<decltype(res)>::type>::getSize(res));
	}

	static std::uint64_t	getSize(::laldb::Number const &target) {
		return (sizeof(BaseSaveStruct) + sizeof(double));
	}
};

template <>
class Serializer<::laldb::Null> {
	static void		serialize(std::FILE *stream, ::laldb::Null const &data) {
		BaseSaveStruct s {
			DataRepresentation::Type::NUL,
			0
		};

		fwrite(&s, sizeof(s), 1, stream);
	}

	static void		*parse(void *data, ::laldb::Null &res) {
		return reinterpret_cast<void*>(
			reinterpret_cast<std::uintptr_t>(data) +
			Serializer<typename std::remove_reference<decltype(res)>::type>::getSize(res));
	}

	static std::uint64_t	getSize(::laldb::Null const &target) {
		return (sizeof(BaseSaveStruct));
	}
};

}