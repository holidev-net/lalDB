
#if !defined(LALDB_DATAREPRESENTATION)
#define LALDB_DATAREPRESENTATION

#include <unordered_map>
#include <vector>
#include <ostream>
#include <istream>
#include <sstream>
#include <iomanip>
#include <memory>
#include <functional>

namespace laldb
{

class AbstractData;
class DataRepresentation;
using ObjEntry = std::pair<const std::string, DataRepresentation>;
using makeObject = std::initializer_list<ObjEntry>;
using ArrayEntry = DataRepresentation;
using makeArray = std::initializer_list<ArrayEntry>;

class DataRepresentation
{
public:
	enum CloneOption {
		LAZY,
		DEEP
	};
	enum Type {
		NBR,
		STR,
		BUF,
		ARR,
		OBJ,
		BOL,
		NUL
	};
public:
	DataRepresentation(DataRepresentation const &) = default;
	DataRepresentation(Type type = NUL);
	DataRepresentation(double nbr);
	DataRepresentation(long nbr);
	DataRepresentation(int nbr);
	DataRepresentation(bool bol);
	DataRepresentation(std::string const &str);
	DataRepresentation(const char *s);
	DataRepresentation(const char *s, std::size_t l);
	DataRepresentation(std::initializer_list<DataRepresentation> list);
	DataRepresentation(std::initializer_list<ObjEntry> list);

	DataRepresentation	&operator=(DataRepresentation const &other) = default;
	bool	operator==(DataRepresentation const &other) const;
	bool	operator!=(DataRepresentation const &other) const;

	DataRepresentation	&operator[](std::string const &key);
	DataRepresentation	&operator[](unsigned idx);
	DataRepresentation	&push(DataRepresentation const &obj);
	
	DataRepresentation	clone(CloneOption attr = CloneOption::DEEP) const;

	bool	isObject(void);
	bool	isArray(void);
	bool	isNumber(void);
	bool	isString(void);
	bool	isBool(void);
	bool	isNull(void);
	
	template<typename T>
	T	&getData();

	template<typename T>
	const T	&getData() const;

	template<typename T>
	auto	value();

	template<typename T>
	auto	value() const;

	static inline DataRepresentation	newObject()
	{
		return DataRepresentation(OBJ);
	}

	static inline DataRepresentation	newArray()
	{
		return DataRepresentation(ARR);
	}
private:
	DataRepresentation(std::shared_ptr<laldb::AbstractData> &data);
private:
	std::shared_ptr<AbstractData>	_data;
};
std::ostream	&operator<<(std::ostream &to, DataRepresentation const &me);


class AbstractData
{
public:
	virtual bool		operator==(AbstractData const &) const = 0;
	virtual DataRepresentation::Type	getType() const = 0;
	virtual std::shared_ptr<AbstractData>
	clone(DataRepresentation::CloneOption attr) const = 0;
};

class Number final : public AbstractData
{
public:
	Number(double nbr);
	
	virtual bool		operator==(AbstractData const &) const final;
	virtual DataRepresentation::Type	getType() const final;
	virtual std::shared_ptr<AbstractData>
	clone(DataRepresentation::CloneOption attr) const final;
	double	get(void) const;
	void	set(double val);
private:
	double	_value;
};

class Buffer final : public AbstractData
{
public:
	Buffer();
	Buffer(void const *s, std::size_t len);

	virtual bool		operator==(AbstractData const &) const final;
	virtual DataRepresentation::Type	getType() const final;
	virtual std::shared_ptr<AbstractData>
	clone(DataRepresentation::CloneOption attr) const final;
	void		*get(void);
	const void	*get(void) const;
	std::size_t	size(void) const;
private:
	void		*_data;
	std::size_t	_len;
};

class Null final : public AbstractData
{
public:
	Null();
	
	virtual bool		operator==(AbstractData const &) const final;
	virtual DataRepresentation::Type	getType() const final;
	virtual std::shared_ptr<AbstractData>
	clone(DataRepresentation::CloneOption attr) const final;
	void	*get(void) const;
};

class String final : public AbstractData, private std::string
{
public:
	String(std::string const &str);

	virtual bool		operator==(AbstractData const &) const final;
	virtual DataRepresentation::Type	getType() const final;
	virtual std::shared_ptr<AbstractData>
	clone(DataRepresentation::CloneOption attr) const final;
	const std::string
			&get(void) const;
	void		set(std::string const &str);
};

class Bool final : public AbstractData
{
public:
	Bool(bool val);
	
	virtual bool		operator==(AbstractData const &) const final;
	virtual DataRepresentation::Type	getType() const final;
	virtual std::shared_ptr<AbstractData>
	clone(DataRepresentation::CloneOption attr) const final;
	bool	get(void) const;
	void	set(bool bol);
private:
	bool	_value;
};

class Object final : public AbstractData, private std::unordered_map<std::string, DataRepresentation>
{
public:
	Object();
	Object(std::initializer_list<ObjEntry> list);

	virtual bool		operator==(AbstractData const &) const final;
	virtual DataRepresentation::Type	getType() const final;
	virtual std::shared_ptr<AbstractData>
	clone(DataRepresentation::CloneOption attr) const final;

	std::unordered_map<std::string, DataRepresentation> &get(void);
};

class Array final : public AbstractData, private std::vector<DataRepresentation>
{
public:
	Array();
	Array(std::initializer_list<ArrayEntry> list);

	virtual bool		operator==(AbstractData const &) const final;
	virtual DataRepresentation::Type	getType(void) const final;
	virtual std::shared_ptr<AbstractData>
	clone(DataRepresentation::CloneOption attr) const final;
	std::vector<DataRepresentation>		&get(void);
};

}

#include "DataRepresentation.tpp"

#endif // LALDB_DATAREPRESENTATION