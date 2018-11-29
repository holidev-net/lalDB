
#if !defined(_LALDB_SERILIZER_INTERNAL)
#define _LALDB_SERILIZER_INTERNAL

#include <cstdio>
#include "DataRepresentation/DataRepresentation.hpp"

namespace laldb {

class BaseSerializer {
public:
	BaseSerializer(std::string const &);
	~BaseSerializer();

	inline void serialize(DataRepresentation const &data);
	inline void parse(DataRepresentation &data);

private:
	inline void verifMode(std::string const &);
	inline DataRepresentation getNextData(void);

private:
	std::string _filename;
	std::string _mode;
	std::FILE *_stream;
};

template <typename DataType>
class Serializer {
public:
	static std::uint64_t	serialize(std::FILE *stream, DataType const &data);
	static void				*parse(void *data, DataType &res);
	static std::uint64_t	getSize(DataType const &);
};

template <>
class Serializer<::laldb::String> {
public:
	static inline std::uint64_t	serialize(std::FILE *stream, ::laldb::String const &data);
	static inline void			*parse(void *data, ::laldb::String &res);
	static inline std::uint64_t	getSize(::laldb::String const &);
};

template <>
class Serializer<::laldb::Object> {
public:
	static inline std::uint64_t	serialize(std::FILE *stream, ::laldb::Object const &data);
	static inline void			*parse(void *data, ::laldb::Object &res);
	static inline std::uint64_t	getSize(::laldb::Object const &);
private:
	static inline std::uint64_t	serializeChamp(std::FILE *stream, std::string const &data);
};

template <>
class Serializer<::laldb::Array> {
public:
	static inline std::uint64_t	serialize(std::FILE *stream, ::laldb::Array const &data);
	static inline void			*parse(void *data, ::laldb::Array &res);
	static inline std::uint64_t	getSize(::laldb::Array const &);
};

template <>
class Serializer<::laldb::Bool> {
public:
	static inline std::uint64_t	serialize(std::FILE *stream, ::laldb::Bool const &data);
	static inline void			*parse(void *data, ::laldb::Bool &res);
	static inline std::uint64_t	getSize(::laldb::Bool const &);
};

template <>
class Serializer<::laldb::Buffer> {
public:
	static inline std::uint64_t	serialize(std::FILE *stream, ::laldb::Buffer const &data);
	static inline void			*parse(void *data, ::laldb::Buffer &res);
	static inline std::uint64_t	getSize(::laldb::Buffer const &);
};

template <>
class Serializer<::laldb::Number> {
public:
	static inline std::uint64_t	serialize(std::FILE *stream, ::laldb::Number const &data);
	static inline void			*parse(void *data, ::laldb::Number &res);
	static inline std::uint64_t	getSize(::laldb::Number const &);
};

template <>
class Serializer<::laldb::Null> {
public:
	static inline std::uint64_t	serialize(std::FILE *stream, ::laldb::Null const &data);
	static inline void			*parse(void *data, ::laldb::Null &res);
	static inline std::uint64_t	getSize(::laldb::Null const &);
};


}

#include "Serializer.tpp"

#endif // _LALDB_SERILIZER_INTERNAL
