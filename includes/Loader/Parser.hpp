#include "DataRepresentation/DataRepresentation.hpp"

#ifdef TARGET_WINDOWS
 #define TOLOWER ::tolower
 #define ISPRINT ::isprint
#else
 #define TOLOWER std::tolower
 #define ISPRINT std::isprint
#endif

namespace laldb
{

class Parser
{
public:
	explicit Parser(std::istream &stm);
	~Parser(void);
	void		parse(void);
	DataRepresentation	get(void);

	static DataRepresentation fromString(std::string const &json);
	static DataRepresentation fromFile(std::string const &file);
	static DataRepresentation fromStream(std::istream &stm);
private:
	using Pmember = DataRepresentation	(Parser::*)(void);

	Pmember	_getUsefulMember(void) const;
	DataRepresentation	_getNumber(void);
	DataRepresentation	_getObj(void);
	DataRepresentation	_getArr(void);
	DataRepresentation	_getStr(void);
	DataRepresentation	_getBool(void);
	DataRepresentation	_getNull(void);

	std::string	__extractStr(void);
	inline char	__peek() const;
private:
	struct Counter {
		std::size_t	line = 1;
		std::size_t	lineOffset = 0;
		std::size_t	offset = 0;
	}	_count;

	DataRepresentation	_obj;
	std::istream	&_stm;
	std::ios_base::fmtflags
			_ff;
public:
	struct ParserException : public std::exception
	{
	public:
		struct Pos {
			std::size_t	line = 0;
			std::size_t	col = 0;
		};
	public:
		ParserException(Parser::Counter const &count, std::istream &stm);
		ParserException(std::string const &custom);

		const char	*what() const throw();
		Pos		getErrorPos() const;
	private:
		std::string	_msg;
		Pos		_pos;
	};
};
std::istream	&operator>>(std::istream &from, DataRepresentation &me);
}