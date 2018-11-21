#include "Loader/Loader.hpp"
#include "_Loader.h"

#ifndef _MSC_VER
#include <errno.h>

static inline int fopen_s(FILE **f, const char *name, const char *mode) {
    int ret = 0;

    *f = fopen(name, mode);
    if (!*f)
        ret = errno;
    return ret;
}
#endif

namespace laldb {

Loader::Loader(std::string const &filename) {
	load(filename);	
}

void	Loader::load(std::string const &filename) {
	if (!fopen_s(&_f, filename.c_str(), "r")) {
		//if (!isNew) {
		//_interpretFile();
		//}
	} else {
		throw std::runtime_error((filename + ": open failed").c_str());
	}
	_filename = filename;
	_isLoaded = true;
}

void	Loader::_interpretFile() {
	FileHeader	head;

	if (std::fread(&head, sizeof(head), 1, _f) != sizeof(head)
		|| head.id != FILEID
		|| head.magic != MAGIC)
		throw std::runtime_error("File header not valid!");
	
}

inline void Loader::_writeDsSections(decltype(_datastores)::iterator cur, decltype(_datastores)::iterator end) {
	auto	&name = cur->first;
	auto	&ds = *(cur->second);

	

	cur = cur++;
	if (cur != end)
		_writeDsSections(cur, end);
};

void	Loader::_saveFile() {
	FileHeader	head;

	fclose(_f);
	fopen_s(&_f, _filename.c_str(), "w");
	head.id = FILEID;
	head.magic = MAGIC;
	head.dataStoreOff = sizeof(head);
	head.version = 1;
	fwrite(&head, sizeof(head), 1, _f);
	_writeDsSections(_datastores.begin(), _datastores.end());
}

}