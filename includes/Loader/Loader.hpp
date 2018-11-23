
#if !defined(LALDB_LOADER)
#define LALDB_LOADER

#include <string>
#include <cstdio>
#include <memory>
#include <unordered_map>
#include "Datastore/Datastore.hpp"

namespace laldb {

class Loader {
private: // Private Vars
	using	_Map = std::unordered_map<std::string, std::unique_ptr<Datastore>>;
	
	bool		_isLoaded = false;
	FILE		*_f = 0;
	std::string	_filename;
	_Map		_datastores;
public: // Public Members
	Loader() = default;
	Loader(std::string const &filename);

	void		load(std::string const &filename);
	Datastore	&operator[](std::string &&name) {
		auto it = _datastores.find(name);
		if (it != _datastores.end())
			return *(it->second);
		else
			return *(_datastores[name] = std::make_unique<Datastore>());
	}
	void		removeDatastore(std::string &&name) { _datastores.erase(_datastores.find(name)); }
private: // Private Members
	void	_interpretFile();
	void	_saveFile();
	void	_writeDsSections(decltype(_datastores)::iterator cur, decltype(_datastores)::iterator end);
};

}

#endif // LALDB_LOADER
