#include"weakCallBack.h"
#include<boost/noncopyable.hpp>

class string {
public:
	string(const char* str) {
		printf("actor ....\n");
	}

	string(const string& other) {
		printf("copy actor...\n");
	}

	string(const string&& other) {
		printf("move copy actor....\n");
	}
};

class Foo :boost::noncopyable {
public:
	void zero() {
		printf("Foo::zero()\n");
	}
	void zeroc() const {
		printf("Foo::zeroc()\n");
	}
	void one(int) {
		printf("Foo::one(int)\n");
	}
	void onec(int) const {
		printf("Foo::onec(int)\n");
	}
	void oner(int&) const {
		printf("Foo::oner(int&)\n");
	}
	void onestring(const string& str)const {
		printf("Foo::onestring(const string& str)\n");
	}
	void onestringRR(string&& str)const {
		printf("Foo::onestringRR(string&& str)\n");
	}
};
string getstring() {
	return string("zxy");
}

namespace {
int main() {
#if 0
//test move
	string s("xx");
	Foo f;
	f.onestring(s);
	//left value  temp value and const value
	f.onestring(string("zxy"));
	f.onestring(string("zxy"));
	f.onestring(getstring());
	f.onestring(getstring());
#endif // 0

	//test weakcallback
	printf("===test weakcallback\n");
	std::shared_ptr<Foo> foo(new Foo);
	mudo::WeakCallback<Foo> cb0 = mudo::makeWeakCallback(foo, &Foo::zero);
	mudo::WeakCallback<Foo> cb0c = mudo::makeWeakCallback(foo, &Foo::zeroc);
	cb0();
	cb0c();

	mudo::WeakCallback<Foo, int> cb1 = mudo::makeWeakCallback(foo, &Foo::one);
	mudo::WeakCallback<Foo, int> cb1c = mudo::makeWeakCallback(foo, &Foo::one);

	cb1(0);
	cb1c(0);

	mudo::WeakCallback<Foo, const string&> cb2 = mudo::makeWeakCallback(foo, &Foo::onestring);
	mudo::WeakCallback<Foo, string&&> cb2r = mudo::makeWeakCallback(foo, &Foo::onestringRR);
	printf("_Z%s\n", typeid(cb2).name());
	printf("_Z%s\n", typeid(cb2r).name());

	mudo::WeakCallback<Foo> cb3(foo, std::bind(&Foo::onestring,std::placeholders::_1,"zzz"));
	cb3();
	return 0;
}

}
