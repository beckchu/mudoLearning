#include"Exception.h"


class Bar {
public:
	void test() {
		throw mudo::Exception("oops");
	}
};

void foo() {
	Bar b;
	b.test();
}

int main() {
	try {
		foo();
	}
	catch (const mudo::Exception& ex) {
		printf("reason:%s\n", ex.what());
		printf("stack trace:%s\n", ex.stackTrace());
	}
}


