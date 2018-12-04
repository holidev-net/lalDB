
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
	enum CloneOption
	{
		LAZY,
		DEEP
	};
	enum Type : std::uint8_t
	{
		NBR,
		STR,
		BUF,
		ARR,
		OBJ,
		BOL,
		NUL,
		FNC
	};

public:
	template<typename T>
	struct patate {
		using Type = T;
	};

	inline DataRepresentation(DataRepresentation const &) = default;
	inline DataRepresentation(Type type = NUL);
	inline DataRepresentation(double nbr);
	inline DataRepresentation(long nbr);
	inline DataRepresentation(int nbr);
	inline DataRepresentation(bool bol);
	inline DataRepresentation(std::string const &str);
	inline DataRepresentation(const char *s);
	inline DataRepresentation(const char *s, std::size_t l);
	inline DataRepresentation(std::function<bool (DataRepresentation const &)> const &);
	// inline DataRepresentation(bool (*)(DataRepresentation const &));
	inline DataRepresentation(std::initializer_list<DataRepresentation> list);
	inline DataRepresentation(std::initializer_list<ObjEntry> list);

	inline DataRepresentation &operator=(DataRepresentation const &other) = default;
	inline bool operator==(DataRepresentation const &other) const;
	inline bool operator!=(DataRepresentation const &other) const;

	inline DataRepresentation &operator[](std::string const &key);
	inline DataRepresentation const &operator[](std::string const &key) const;
	inline DataRepresentation &operator[](unsigned idx);
	inline DataRepresentation const &operator[](unsigned idx) const;
	inline DataRepresentation &push(DataRepresentation const &obj);
	inline bool operator()(DataRepresentation const &data) const;

	inline DataRepresentation clone(CloneOption attr = CloneOption::DEEP) const;

	inline DataRepresentation::Type getType(void) const;
	inline bool isObject(void) const;
	inline bool isArray(void) const;
	inline bool isNumber(void) const;
	inline bool isString(void) const;
	inline bool isBool(void) const;
	inline bool isNull(void) const;
	inline bool isFunction(void) const;

	template <typename T>
	inline T &getData();

	template <typename T>
	inline const T &getData() const;

	template <typename T>
	inline auto &value();

	template <typename T>
	inline const auto &value() const;

	template <typename T, typename U>
	inline U value();

	template <typename T, typename U>
	inline U value() const;

	static inline DataRepresentation newObject()
	{
		return DataRepresentation(OBJ);
	}

	static inline DataRepresentation newArray()
	{
		return DataRepresentation(ARR);
	}

private:
	inline DataRepresentation(std::shared_ptr<laldb::AbstractData> &data);

private:
	std::shared_ptr<AbstractData>	_data;

/*  ITERATOR  */
public:
	class iterator {
	public:
		inline iterator(DataRepresentation *data = nullptr, long pos = 0);
		inline iterator(iterator const &);
		inline ~iterator();
		inline iterator &operator=(iterator const &);
		inline iterator &operator++();
		inline bool operator==(iterator const &) const;
		inline bool operator!=(iterator const &) const;
		inline DataRepresentation &operator*() const;
		inline DataRepresentation *operator->() const;
		inline friend void swap(iterator &lhs, iterator &rhs);
	private:
		DataRepresentation	*_data;
		long				_pos;
	};

	inline iterator begin();
	inline iterator end();
};
inline std::ostream &operator<<(std::ostream &to, DataRepresentation const &me);

class AbstractData {
public:
	virtual bool operator==(AbstractData const &) const = 0;
	virtual DataRepresentation::Type getType() const = 0;
	virtual std::shared_ptr<AbstractData>
	clone(DataRepresentation::CloneOption attr) const = 0;
};

class Number final : public AbstractData {
public:
	inline Number(double nbr);

	inline virtual bool operator==(AbstractData const &) const final;
	inline virtual DataRepresentation::Type getType() const final;
	inline virtual std::shared_ptr<AbstractData>
	clone(DataRepresentation::CloneOption attr) const final;
	inline double	get(void) const;
	inline double	&get(void);
	inline void	set(double val);
private:
	double	_value;
};

class Buffer final : public AbstractData {
public:
	inline Buffer();
	inline Buffer(void const *s, std::size_t len);

	inline virtual bool operator==(AbstractData const &) const final;
	inline virtual DataRepresentation::Type getType() const final;
	inline virtual std::shared_ptr<AbstractData>
	clone(DataRepresentation::CloneOption attr) const final;
	inline const void *get(void) const;
	inline void *get(void);
	inline void set(const char *buf, std::size_t len);
	inline std::size_t size(void) const;

private:
	void *_data;
	std::size_t _len;
};

class Null final : public AbstractData {
public:
	inline Null();

	inline virtual bool operator==(AbstractData const &) const final;
	inline virtual DataRepresentation::Type getType() const final;
	inline virtual std::shared_ptr<AbstractData>
	clone(DataRepresentation::CloneOption attr) const final;
	inline void *get(void) const;
};

class String final : public AbstractData, private std::string {
public:
	inline String(std::string const &str);

	inline virtual bool operator==(AbstractData const &) const final;
	inline virtual DataRepresentation::Type getType() const final;
	inline virtual std::shared_ptr<AbstractData>
	clone(DataRepresentation::CloneOption attr) const final;
	inline const std::string &get(void) const;
	inline std::string &get(void);
	inline void set(std::string const &str);
};

class Bool final : public AbstractData {
public:
	inline Bool(bool val);

	inline virtual bool operator==(AbstractData const &) const final;
	inline virtual DataRepresentation::Type getType() const final;
	inline virtual std::shared_ptr<AbstractData>
	clone(DataRepresentation::CloneOption attr) const final;
	inline bool get(void) const;
	inline void set(bool bol);

  private:
	bool _value;
};

class Object final : public AbstractData, private std::unordered_map<std::string, DataRepresentation> {
public:
	inline Object();
	inline Object(std::initializer_list<ObjEntry> list);

	inline virtual bool operator==(AbstractData const &) const final;
	inline virtual DataRepresentation::Type getType() const final;
	inline virtual std::shared_ptr<AbstractData>
	clone(DataRepresentation::CloneOption attr) const final;

	inline std::unordered_map<std::string, DataRepresentation> const &get(void) const;
	inline std::unordered_map<std::string, DataRepresentation> &get(void);
};

class Array final : public AbstractData, private std::vector<DataRepresentation> {
public:
	inline Array();
	inline Array(std::initializer_list<ArrayEntry> list);

	inline virtual bool operator==(AbstractData const &) const final;
	inline virtual DataRepresentation::Type getType(void) const final;
	inline virtual std::shared_ptr<AbstractData>
	clone(DataRepresentation::CloneOption attr) const final;
	inline std::vector<DataRepresentation> const &get(void) const;
	inline std::vector<DataRepresentation> &get(void);
};

class Function final : public AbstractData {
public:
	using Func = std::function<bool (DataRepresentation const &)>;

	inline Function(Func);

	inline virtual bool operator==(AbstractData const &) const final;
	inline virtual DataRepresentation::Type getType() const final;
	inline virtual std::shared_ptr<AbstractData>
	clone(DataRepresentation::CloneOption attr) const final;

	inline bool operator()(DataRepresentation const &) const;
	inline void		set(Func val);
private:
	Func	_value;
};

} // namespace laldb

#include "DataRepresentation.tpp"

#endif // LALDB_DATAREPRESENTATION