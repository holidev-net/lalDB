
#if !defined(LALDB_LOADER)
#define LALDB_LOADER

#include <string>
#include <cstdio>

namespace laldb {

class Loader {
private: // Private Vars
	bool	_isLoaded = false;
	FILE	*_f = 0;
public: // Public Members
	Loader() = default;
	Loader(std::string const &filename);

	void	load(std::string const &filename);
private: // Private Members
	void	_interpretFile();
};

}

#endif // LALDB_LOADER
