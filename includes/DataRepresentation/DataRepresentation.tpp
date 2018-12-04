#if !defined(LALDB_DATAREPRESENTATION_TPP)
#define LALDB_DATAREPRESENTATION_TPP

#include <cstring>
#include <algorithm>
#include "DataRepresentation.hpp"

namespace laldb {

DataRepresentation::DataRepresentation(Type type) {
	switch (type) {
		case OBJ:
			_data = std::make_shared<Object>();
			break;
		case ARR:
			_data = std::make_shared<Array>();
			break;
		case BUF:
			_data = std::make_shared<Buffer>();
			break;
		case NBR:
			_data = std::make_shared<Number>(0);
			break;
		case STR:
			_data = std::make_shared<String>("");
			break;
		case BOL:
			_data = std::make_shared<Bool>(false);
			break;
		case NUL:
			_data = std::make_shared<Null>();
		default:
			break;
	}
}

DataRepresentation::DataRepresentation(double nbr):
	_data(std::make_shared<Number>(nbr))
{}

DataRepresentation::DataRepresentation(long nbr):
	_data(std::make_shared<Number>((double) nbr))
{}

DataRepresentation::DataRepresentation(int nbr):
	_data(std::make_shared<Number>((double) nbr))
{}

DataRepresentation::DataRepresentation(bool val):
	_data(std::make_shared<Bool>(val))
{}

DataRepresentation::DataRepresentation(std::string const &str):
	_data(std::make_shared<String>(str))
{}

DataRepresentation::DataRepresentation(const char *str):
	_data(std::make_shared<String>(std::string(str)))
{}

DataRepresentation::DataRepresentation(std::function<bool (DataRepresentation const &)> const &func):
	_data(std::make_shared<Function>(func))
{}

// DataRepresentation::DataRepresentation(bool (*func)(DataRepresentation const &)):
// 	_data(std::make_shared<Function>(func))
// {}

DataRepresentation::DataRepresentation(std::initializer_list<DataRepresentation> list):
	_data(std::make_shared<Array>(list))
{}

DataRepresentation::DataRepresentation(std::initializer_list<std::pair<const std::string, DataRepresentation>> list):
	_data(std::make_shared<Object>(list))
{}

DataRepresentation::DataRepresentation(std::shared_ptr<laldb::AbstractData> &data):
	_data(data)
{}

template<typename T>
T	&DataRepresentation::getData() {
	return *(reinterpret_cast<T*>(_data.get()));
}

template<typename T>
const T	&DataRepresentation::getData() const {
	return *(reinterpret_cast<T*>(_data.get()));
}

template<typename T>
auto	&DataRepresentation::value() {
	return (reinterpret_cast<T*>(_data.get()))->get();
}

template<typename T>
const auto	&DataRepresentation::value() const {
	return (reinterpret_cast<T*>(_data.get()))->get();
}

template<typename T, typename U>
U	DataRepresentation::value() {
	return (reinterpret_cast<T*>(_data.get()))->get();
}

template<typename T, typename U>
U	DataRepresentation::value() const {
	return (reinterpret_cast<T*>(_data.get()))->get();
}

bool	DataRepresentation::operator==(DataRepresentation const &other) const {
	return ((*_data) == other.getData<AbstractData>());
}

bool	DataRepresentation::operator!=(DataRepresentation const &other) const {
	return !((*_data) == other.getData<AbstractData>());
}

DataRepresentation	&DataRepresentation::operator[](std::string const &key) {
	if (_data == nullptr || !isObject()) {
		throw std::runtime_error("laldb : Error : "
			"Cannot use this DataRepresentation");
	}
	return getData<Object>().get()[key];
}

DataRepresentation const	&DataRepresentation::operator[](std::string const &key) const {
	if (_data == nullptr || !isObject()) {
		throw std::runtime_error("laldb : Error : "
			"Cannot use this DataRepresentation");
	}
	return getData<Object>().get().at(key);
}

DataRepresentation	&DataRepresentation::operator[](unsigned idx) {
	if (_data == nullptr || !isArray()) {
		throw std::runtime_error("laldb : Error : "
			"Cannot use this DataRepresentation");
	}
	return getData<Array>().get()[idx];
}

DataRepresentation const	&DataRepresentation::operator[](unsigned idx) const {
	if (_data == nullptr || !isArray()) {
		throw std::runtime_error("laldb : Error : "
			"Cannot use this DataRepresentation");
	}
	return getData<Array>().get()[idx];
}

DataRepresentation	&DataRepresentation::push(DataRepresentation const &obj) {
	if (_data == nullptr || !isArray()) {
		throw std::runtime_error("laldb : Error : "
			"Cannot use this DataRepresentation");
	}
	auto &arr = getData<Array>().get();

	arr.emplace_back(obj);
	return arr.back();
}

bool DataRepresentation::operator()(DataRepresentation const &data) const
{
	if (_data == nullptr || !isFunction()) {
		throw std::runtime_error("laldb : Error : "
			"Cannot use this DataRepresentation");
	}
	return getData<Function>()(data);
}

DataRepresentation	DataRepresentation::clone(CloneOption attr) const {
	auto	copiedData = _data->clone(attr);

	return DataRepresentation(copiedData);
}

DataRepresentation::Type DataRepresentation::getType(void) const {
	return _data->getType();
}

bool	DataRepresentation::isObject(void) const {
	return (_data->getType() == OBJ);
}

bool	DataRepresentation::isArray(void) const {
	return (_data->getType() == ARR);
}

bool	DataRepresentation::isNumber(void) const {
	return (_data->getType() == NBR);
}

bool	DataRepresentation::isString(void) const {
	return (_data->getType() == STR);
}

bool	DataRepresentation::isBool(void) const {
	return (_data->getType() == BOL);
}

bool	DataRepresentation::isNull(void) const {
	return (_data->getType() == NUL);
}

bool	DataRepresentation::isFunction(void) const {
	return (_data->getType() == FNC);
}

DataRepresentation::iterator	DataRepresentation::begin()
{
	if (isNull())
		return end();
	if (isArray() && value<Array>().empty()) {
		return end();
	}
	return DataRepresentation::iterator{this};
}

DataRepresentation::iterator	DataRepresentation::end()
{
	return DataRepresentation::iterator{this, -1L};
}


static void printDataRepresentation(std::ostream &os, DataRepresentation const &obj, int decal = 0, bool member = false)
{
	int n = 0;
	for (int i = 0; !member && i < decal; ++i) os << "  ";
	switch (obj.getType()) {
		case laldb::DataRepresentation::Type::NBR:
			os << obj.value<laldb::Number>();
			break;
		case laldb::DataRepresentation::Type::STR:
			os << '"' << obj.value<laldb::String>() << '"';
			break;
		case laldb::DataRepresentation::Type::BUF:
			break;
		case laldb::DataRepresentation::Type::ARR:
			os << "[\n";
			for (auto &e : obj.value<Array>()) {
				if (n) os << ",\n";
				for (int i = 0; i < decal + 1; ++i) os << "  ";
				os << '[' << n << "] : ";
				printDataRepresentation(os, e, decal + 1, true);
				++n;
			}
			os << '\n';
			for (int i = 0; i < decal; ++i) os << "  ";
			os << ']';
			break;
		case laldb::DataRepresentation::Type::OBJ:
			os << "{\n";
			for (auto &e : obj.value<laldb::Object>()) {
				if (n) os << ",\n";
				for (int i = 0; i < decal + 1; ++i) os << "  ";
				os << '"' << e.first << "\" : ";
				printDataRepresentation(os, e.second, decal + 1, true);
				++n;
			}
			os << '\n';
			for (int i = 0; i < decal; ++i) os << "  ";
			os << '}';
			break;
		case laldb::DataRepresentation::Type::BOL:
			os << (obj.value<Bool, bool>() ? "true" : "false");
			break;
		case laldb::DataRepresentation::Type::NUL:
			os << "NULL";
			break;
		default:
			os << "non printable data";break;
	}
	if (!member)
		os << std::endl;
}

std::ostream &operator<<(std::ostream &os, DataRepresentation const &me)
{
	printDataRepresentation(os, me);
	return os;
}

/***********************************************
 *	ITERATOR
 ***********************************************/
DataRepresentation::iterator::iterator(DataRepresentation *data, long pos):
_data{data},
_pos{pos}
{
	if (_data == nullptr)
		_pos = -1;
	else if (pos != 0 && (!data->isArray() || pos <= (long)data->value<Array>().size()))
		_pos = -1;
}

DataRepresentation::iterator::iterator(iterator const &it):
_data{it._data},
_pos{it._pos}
{
}

DataRepresentation::iterator::~iterator()
{
}

DataRepresentation::iterator &DataRepresentation::iterator::operator=(iterator const &it)
{
	_data = it._data;
	_pos = it._pos;
	return *this;
}

DataRepresentation::iterator &DataRepresentation::iterator::operator++()
{
	if (!_data->isArray() || _pos >= (long)(_data->value<Array>().size() - 1))
		_pos = -1;
	else
		++_pos;
	return *this;
}

bool	DataRepresentation::iterator::operator==(iterator const &it) const
{
	return (_data == it._data && _pos == it._pos);
}

bool	DataRepresentation::iterator::operator!=(iterator const &it) const
{
	return (_data != it._data || _pos != it._pos);
}

DataRepresentation &DataRepresentation::iterator::operator*() const
{
	if (_pos == -1)
		throw std::runtime_error("laldb : Error : "
			"invalid iterator");
	if (_data->isArray())
		return (*_data)[_pos];
	return *_data;
}

DataRepresentation *DataRepresentation::iterator::operator->() const
{
	if (_pos == -1)
		throw std::runtime_error("laldb : Error : "
			"invalid iterator");
	if (_data->isArray())
		return &(*_data)[_pos];
	return _data;
}

void swap(DataRepresentation::iterator& lhs, DataRepresentation::iterator& rhs)
{
	std::swap(lhs._data, rhs._data);
	std::swap(lhs._pos, rhs._pos);
}

/***********************************************
 *	NUMBER
 ***********************************************/
Number::Number(double nbr) :
	_value(nbr)
{}

bool		Number::operator==(AbstractData const &other) const {
	return (other.getType() == DataRepresentation::NBR &&
		_value == static_cast<const Number&>(other).get());
}

DataRepresentation::Type	Number::getType() const {
	return DataRepresentation::NBR;
}

std::shared_ptr<AbstractData>	Number::clone(DataRepresentation::CloneOption attr) const {
	(void) attr;
	return std::make_shared<Number>(_value);
}

double	Number::get(void) const {
	return _value;
}

double	&Number::get(void) {
	return _value;
}

void	Number::set(double nbr) {
	_value = nbr;
}

/***********************************************
 *	BUFFER
 ***********************************************/
Buffer::Buffer() {}

Buffer::Buffer(void const *buffer, std::size_t len):
		_data(::operator new(len)),
		_len(len) {
	std::memmove(_data, buffer, len);
}

bool		Buffer::operator==(AbstractData const &other) const {
	if (other.getType() == DataRepresentation::BUF) {
		return (_len == static_cast<const Buffer&>(other).size() 
		&& !std::memcmp(_data, static_cast<const Buffer&>(other).get(), _len));
	}
	return false;
}

DataRepresentation::Type	Buffer::getType() const {
	return DataRepresentation::BUF;
}

std::shared_ptr<AbstractData>	Buffer::clone(DataRepresentation::CloneOption) const {
	return std::make_shared<Buffer>(_data, _len);
}

void	*Buffer::get(void) {
	return _data;
}

const void	*Buffer::get(void) const {
	return _data;
}

std::size_t Buffer::size(void) const {
	return _len;
}

void		Buffer::set(const char *buf, std::size_t len) {
	if (_data)
		::operator delete(_data);
	_data = ::operator new(len);
	_len = len;
	std::memmove(_data, buf, len);
}

/***********************************************
 *	NULL
***********************************************/
Null::Null()
{}

DataRepresentation::Type	Null::getType() const {
	return DataRepresentation::NUL;
}

bool		Null::operator==(AbstractData const &other) const {
	return (other.getType() == DataRepresentation::NUL);
}

std::shared_ptr<AbstractData>	Null::clone(DataRepresentation::CloneOption attr) const {
	(void) attr;
	return std::make_shared<Null>();
}

void	*Null::get(void) const {
	return nullptr;
}

/***********************************************
 *	STRING
***********************************************/
String::String(std::string const &str) :
	std::string(str)
{}

DataRepresentation::Type	String::getType() const {
	return DataRepresentation::STR;
}

bool		String::operator==(AbstractData const &other) const {
	return (other.getType() == DataRepresentation::STR &&
		*this == static_cast<const String&>(other).get());
}

std::shared_ptr<AbstractData>	String::clone(DataRepresentation::CloneOption) const {
	return std::make_shared<String>(*this);
}

const std::string	&String::get(void) const {
	return *this;
}

std::string	&String::get(void)
{
	return *this;
}

void		String::set(std::string const &str) {
	dynamic_cast<std::string&>(*this) = str;
}

/***********************************************
 *	Bool
***********************************************/
Bool::Bool(bool val) :
	_value(val)
{}

DataRepresentation::Type	Bool::getType() const {
	return DataRepresentation::BOL;
}

bool		Bool::operator==(AbstractData const &other) const {
	return (other.getType() == DataRepresentation::BOL &&
		_value == static_cast<const Bool&>(other).get());
}

std::shared_ptr<AbstractData>	Bool::clone(DataRepresentation::CloneOption) const {
	return std::make_shared<Bool>(_value);
}

bool	Bool::get(void) const {
	return _value;
}

void	Bool::set(bool val) {
	_value = val;
}


/***********************************************
 *	Object
***********************************************/
Object::Object()
{}

Object::Object(std::initializer_list<ObjEntry> list):
	std::unordered_map<std::string, DataRepresentation>(list)	
{}

DataRepresentation::Type	Object::getType() const {
	return DataRepresentation::OBJ;
}

bool		Object::operator==(AbstractData const &other) const {
	return (other.getType() == DataRepresentation::OBJ &&
		*this == const_cast<Object&>(static_cast<const Object&>(other)).get());
}

std::shared_ptr<AbstractData>	Object::clone(DataRepresentation::CloneOption attr) const {
	auto	newObj = std::make_shared<Object>();

	for (auto &elem : *this) {
		if (attr == DataRepresentation::CloneOption::DEEP) {
			newObj->get()[elem.first] = elem.second.clone(attr);
		} else {
			newObj->get()[elem.first] = elem.second;
		}
	}
	return newObj;
}

std::unordered_map<std::string, DataRepresentation>	&Object::get(void) {
	return *this;
}

std::unordered_map<std::string, DataRepresentation> const	&Object::get(void) const {
	return *this;
}

/***********************************************
 *	Array
***********************************************/

Array::Array()
{}

Array::Array(std::initializer_list<DataRepresentation> list):
	std::vector<DataRepresentation>(list)
{}

DataRepresentation::Type	Array::getType() const {
	return DataRepresentation::ARR;
}

bool		Array::operator==(AbstractData const &other) const {
	return (other.getType() == DataRepresentation::ARR &&
		*this == const_cast<Array&>(static_cast<const Array&>(other)).get());
}

std::shared_ptr<AbstractData>	Array::clone(DataRepresentation::CloneOption attr) const {
	auto	newArr = std::make_shared<Array>();

	for (auto &elem : *this) {
		if (attr == DataRepresentation::CloneOption::DEEP) {
			newArr->get().push_back(elem.clone(attr));
		} else {
			newArr->get().push_back(elem);
		}
	}
	return newArr;
}

std::vector<DataRepresentation>	&Array::get(void) {
	return *this;
}

std::vector<DataRepresentation>	const &Array::get(void) const {
	return *this;
}

/***********************************************
 *	Array
***********************************************/

Function::Function(Func nbr):
_value(nbr)
{}

bool		Function::operator==(AbstractData const &other) const {
	(void) other;
	return false;
}

DataRepresentation::Type	Function::getType() const {
	return DataRepresentation::FNC;
}

std::shared_ptr<AbstractData>	Function::clone(DataRepresentation::CloneOption attr) const {
	(void) attr;
	return std::make_shared<Function>(_value);
}

bool Function::operator()(DataRepresentation const &data) const
{
	return _value(data);
}

void	Function::set(Func nbr) {
	_value = nbr;
}


}


#endif // LALDB_DATAREPRESENTATION_TPP
