#include"thread.h"
#include<stdlib.h>
#include<boost/bind.hpp>


void threadFunc1() {
	printf("threadFunc1 tid=%d\n", mudo::CurrentThread::tid());
}

void threadFunc2(int x) {
	printf("threadFunc2 tid=%d , x=%d\n", mudo::CurrentThread::tid(), x);
}

class Foo {
public:
	explicit Foo(double x) :x_(x) {

	}

	void memeberFunc() {
		printf("memeberFunc tid=%d , x=%f\n", mudo::CurrentThread::tid(), x_);
	}

	void memeberFunc2(const std::string text) {
		printf("memeberFunc2 tid=%d , x=%f , text=%s\n", mudo::CurrentThread::tid(), x_, text.c_str());
	}

private:
	double x_;
};

namespace {
	int main()
	{
		printf("pid=%d, tid=%d\n", ::getpid(), mudo::CurrentThread::tid());

		mudo::Thread t1(threadFunc1, "t1");
		t1.start();
		t1.join();

		mudo::Thread t2(boost::bind(threadFunc2, 42), "t2");
		t2.start();
		t2.join();

		Foo foo(36.500);
		mudo::Thread t3(boost::bind(&Foo::memeberFunc, &foo), "t3");
		t3.start();
		t3.join();

		mudo::Thread t4(boost::bind(&Foo::memeberFunc2, boost::ref(foo), std::string("Foo::memeberFunc2")), "t4");
		t4.start();
		t4.join();

		std::cout << "main end now...\n" << std::endl;
		return 0;
	}
}
