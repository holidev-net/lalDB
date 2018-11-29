#include <fstream>
#include "Loader/Parser.hpp"

namespace laldb {

inline char Parser::__peek() const {
	char	c;

	while ((c = _stm.peek()) && c && 
		(c == '\n' || c == ' ' || c == '\t')) {
		const_cast<Counter&>(_count).offset++;
		if (c == '\n') {
			const_cast<Counter&>(_count).line++;
			const_cast<Counter&>(_count).lineOffset = const_cast<Counter&>(_count).offset;
		}
		_stm >> c;
	}
	const_cast<Counter&>(_count).offset++;		
	return c;
}

std::istream	&operator>>(std::istream &from, DataRepresentation &me)
{
	me = Parser::fromStream(from);
	return (from);
}

Parser::Parser(std::istream &stm):
	_stm(stm),
	_ff(_stm.flags())
	
{
	_stm.unsetf(std::ios::skipws);
}

Parser::~Parser(void)
{
	_stm.flags(_ff);
}

DataRepresentation Parser::fromString(std::string const &json)
{
	std::istringstream	stm(json);

	return Parser::fromStream(stm);
}

DataRepresentation Parser::fromFile(std::string const &file)
{
	std::ifstream	stm(file);

	if (!stm.is_open())
		throw ParserException("'" + file + "': Unable to open file");
	return Parser::fromStream(stm);
}

DataRepresentation Parser::fromStream(std::istream &stm)
{
	Parser	parser(stm);

	parser.parse();
	return parser.get();
}

Parser::Pmember	Parser::_getUsefulMember(void) const
{
	char	c;
	struct MemberIdent{
		char const	*ident;
		DataRepresentation	(Parser::*member)(void);
	};
	static std::vector<struct MemberIdent> tab = {
		{.ident = "\"'", .member = &Parser::_getStr},
		{.ident = "{", .member = &Parser::_getObj},
		{.ident = "[", .member = &Parser::_getArr},
		{.ident = "tfTF", .member = &Parser::_getBool},
		{.ident = "0123456789-.", .member = &Parser::_getNumber},
		{.ident = "nN", .member = &Parser::_getNull}
	};

	while ((c = __peek()) && c && c == ',')
		_stm >> c;
	if ((c = __peek()) && c) {
		for (auto &a : tab)
			if (std::strchr(a.ident, c)) {
				return a.member;
			}
	}
	if (c)
		throw ParserException(_count, _stm);
	return nullptr;
}

void	Parser::parse(void)
{
	Pmember	parseMember = _getUsefulMember();
	
	_obj = (this->*parseMember)();
}

DataRepresentation Parser::_getNumber(void)
{
	double	value;

	_stm.gcount();
	_stm >> value;
	_count.offset += _stm.gcount();
	return DataRepresentation(value);
}

DataRepresentation Parser::_getObj(void)
{
	char	c;
	DataRepresentation	res(DataRepresentation::OBJ);

	auto extractKey = [this, &c] (std::string &key) -> bool {
		if (c == '"' || c == '\'') {
			key = __extractStr();
			return true;
		}
		return false;
	};

	_stm >> c;
	while ((c = __peek()) && c && c != '}') {
		std::string	key;

		if (extractKey(key) == false)
			throw ParserException(_count, _stm);

		c = __peek();
		_stm >> c;
		if (c != ':')
			throw ParserException(_count, _stm);
		res[key] = (this->*(_getUsefulMember()))();
		while ((c == __peek()) && c && c == ',')
			_stm >> c;
		_stm >> c;
		if (c == '}')
			break;
		
	}
	return res;
}

DataRepresentation Parser::_getArr(void)
{
	char	c;
	DataRepresentation	res(DataRepresentation::ARR);

	_stm >> c;
	while ((c = __peek()) && c && c != ']') {
		auto fct = _getUsefulMember();
		
		res.push((this->*(fct))());
	}
	_stm >> c;		
	return (res);
}

DataRepresentation Parser::_getStr(void)
{
	return (DataRepresentation(__extractStr()));
}

DataRepresentation Parser::_getBool(void)
{
	std::ostringstream	sValue;
	char			c;
	int			max = sizeof("false");

	while (_stm >> c && c && max) {
		sValue << c;

		_count.offset++;
		std::string toLower = sValue.str();
		for (std::size_t i = 0; i < toLower.size(); i++)
			toLower[i] = TOLOWER(toLower[i]);
		if (toLower == "true" ||
			toLower == "false")
			break;
		max--;
	}
	if (!max)
		throw ParserException(_count, _stm);
	return DataRepresentation(sValue.str() == "true");
}

DataRepresentation Parser::_getNull(void)
{
	std::ostringstream	sValue;
	char			c;
	int			max = sizeof("null");

	while (_stm >> c && c && max) {
		sValue << c;
		_count.offset++;
		std::string toLower = sValue.str();
		for (std::size_t i = 0; i < toLower.size(); i++)
			toLower[i] = TOLOWER(toLower[i]);
		if (toLower == "null")
			break;
		max--;
	}
	if (!max)
		throw ParserException(_count, _stm);
	return DataRepresentation(DataRepresentation::NUL);
}

std::string	Parser::__extractStr(void)
{
	std::ostringstream	sValue;
	char			c;
	char			endKey;
	bool			escape = false;

	_stm >> endKey;			
	while (_stm >> c && (c != endKey || escape)) {
		_count.offset++;
		if (c == '\n') {
			_count.line++;
			_count.lineOffset = _count.offset;
		}
		if (escape)
			escape = !escape;
		else if (c == '\\') {
			escape = !escape;
			continue;
		}
		sValue << c;
	}
	return sValue.str();
}

DataRepresentation	Parser::get(void)
{
	return _obj;
}

Parser::ParserException::ParserException(std::string const &custom):
	_msg("Parser: Error: " + custom)
{}

Parser::ParserException::ParserException(Parser::Counter const &count, std::istream &stm)
{
	std::ostringstream	_sstm;
	unsigned char		c =  stm.peek();

	_pos = {
		.line = count.line,
		.col = count.offset - count.lineOffset
	};
	_sstm << "Parser: Error: "
		<< "(Ln " << _pos.line << ": Col "
		<< _pos.col << "): "
		<< "Unexpected charater: '";
	if (ISPRINT(c)) {
		_sstm << c;
	} else if ((char) c == -1) {
		_sstm << "EOF";
	} else {
		_sstm << "0x" << std::hex << +c;
	}
	_sstm << '\'' << std::endl;
	_msg = _sstm.str();
}

const char	*Parser::ParserException::what() const throw()
{
	return _msg.c_str();
}

}