
#if !defined(_LALDB_SERILIZER_INTERNAL)
#define _LALDB_SERILIZER_INTERNAL

#include <cstdio>
#include "DataRepresentation/DataRepresentation.hpp"

namespace laldb {

class BaseSerializer {
	static 
}

template <typename DataType>
class Serializer {
public:
	static void				serialize(std::FILE *stream, DataType const &data);
	static void				*parse(void *data, DataType &res);
	static std::uint64_t	getSize(DataType const &);
};

template <>
class Serializer<::laldb::String>;

template <>
class Serializer<::laldb::Object>;

template <>
class Serializer<::laldb::Array>;

template <>
class Serializer<::laldb::Bool>;

template <>
class Serializer<::laldb::Buffer>;

template <>
class Serializer<::laldb::Number>;

template <>
class Serializer<::laldb::Null>;


}

#endif // _LALDB_SERILIZER_INTERNAL
