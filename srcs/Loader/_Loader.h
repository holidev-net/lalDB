#if !defined(_LALDB_LOADER_INTERNAL)
#define _LALDB_LOADER_INTERNAL

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _MSC_VER  // MSVC
# define DECORATED_STRUCT(name, body) struct name body
# pragma warning(disable : 4200)
#else // GCC's compilers
# define DECORATED_STRUCT(name, body) struct name body __attribute__((ms_struct))
#endif

#pragma pack(push, 4) //MSVC compatibility :$

#define FILEID 0x4C4C4442 //0x4C4C4442 -> 'L' + 'L' + 'D' + 'B'
#define MAGIC 0x42444C4C //0x42444C4C

DECORATED_STRUCT(__Name, {
	uint64_t	size;
	char		data[];
});

DECORATED_STRUCT(LaldbFileHeader, {
	uint32_t	id; // = FILEID
	uint32_t	magic; // = MAGIC
	uint16_t	version;
	uint64_t	dataStoreOff;
	uint64_t	updateOff;
});

DECORATED_STRUCT(LaldbDatastoreHeader, {
	uint64_t	size;
	uint64_t	nextOff; // == 0 if is last
	__Name		name;
	// ... Data
});

DECORATED_STRUCT(LaldbUpdateHeader, {
	uint64_t	size;
	uint64_t	nextOff; // == 0 if is last
	__Name		datastoreName;
	// ... Data
});

#pragma pack(pop)

#ifdef __cplusplus
}
namespace laldb {
	using FileHeader = LaldbFileHeader;
	using DatastoreHeader = LaldbDatastoreHeader;
	using UpdateHeader = LaldbUpdateHeader;
}
#endif

#endif // _LALDB_LOADER_INTERNAL