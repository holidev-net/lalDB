#if !defined(_LALDB_LOADER_INTERNAL)
#define _LALDB_LOADER_INTERNAL

#include <stdint.h>
#include "_internal.h"

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(push, 4) //MSVC compatibility :$

#define FILEID 0x4C4C4442 //0x4C4C4442 -> 'L' + 'L' + 'D' + 'B'
#define MAGIC 0x42444C4C //0x42444C4C

enum UpdateType {
	ADD_DATASTORE = 0,
	UPDATE_DATASTORE = 1,
	RENAME_DATASTORE = 2,
	ADD_VALUE = 3,
	UPDATE_VALUE = 4,
	REMOVE_VALUE = 5
};

DECORATED_STRUCT(__dyn, {
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
	__dyn		name;
	// ... Data
});

DECORATED_STRUCT(LaldbUpdateHeader, {
	uint64_t	size;
	UpdateType	type;
	uint64_t	nextOff; // == 0 if is last
	__dyn		datastoreName;
	// ... Data
});

#pragma pack(pop)

#ifdef __cplusplus
}
namespace laldb {
	using FileHeader = LaldbFileHeader;
	using DatastoreHeader = LaldbDatastoreHeader;
	using UpdateHeader = LaldbUpdateHeader;

	template <typename T, typename U>
	static inline T *getData(U *ptr) {
		auto len = (reinterpret_cast<__dyn*>(reinterpret_cast<void*>(ptr) + sizeof(U) - sizeof(__dyn)))->size;

		return reinterpret_cast<T*>(reinterpret_cast<void*>(ptr) + sizeof(U) + len);
	}
}
#endif

#endif // _LALDB_LOADER_INTERNAL