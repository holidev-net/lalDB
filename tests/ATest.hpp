#if !defined(ITEST)
#define ITEST

#include <list>

class ATest {
public:
	static auto &singleton() {
		static std::list<ATest*> ah;
		return ah;
	}

	ATest() { ::ATest::singleton().push_back(this); }

	virtual void launchTests() = 0;
};

#endif // ITEST
