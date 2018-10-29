#include <filesystem>
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
	auto isNew = std::filesystem::exists(filename);

	if (!fopen_s(&_f, filename.c_str(), "w+")) {
		if (!isNew) {
			_interpretFile();
		}
	} else {
		throw std::runtime_error((filename + ": open failed").c_str());
	}
	_isLoaded = true;
}

void	Loader::_interpretFile() {
	FileHeader	head;

	if (std::fread(&head, sizeof(head), 1, _f) != sizeof(head)
		|| head.id != FILEID
		|| head.magic != MAGIC)
		throw std::runtime_error("File header not valid!");
	
}

}