#include"threadLocalSingleton.h"
#include"thread.h"

#include<boost/bind.hpp>


class Test :boost::noncopyable {
public:
	Test() {
		printf("tid=%d, constructing %p\n", mudo::CurrentThread::tid(), this);
	}
	~Test() {
		printf("tid=%d, destructing %p %s\n", mudo::CurrentThread::tid(), this, name_.c_str());
	}
	const std::string name() const { return name_; }
	void setName(const std::string& n) {
		name_ = n;
	}
private:
	std::string name_;
};

void threadFunc(const char* changeto) {
	printf("tid=%d, %p name=%s\n",
		mudo::CurrentThread::tid(),
		&mudo::ThreadLocalSingleton<Test>::instance(),
		mudo::ThreadLocalSingleton<Test>::instance().name().c_str());
	mudo::ThreadLocalSingleton<Test>::instance().setName(changeto);
	printf("tid=%d, %p name=%s\n",
		mudo::CurrentThread::tid(),
		&mudo::ThreadLocalSingleton<Test>::instance(),
		mudo::ThreadLocalSingleton<Test>::instance().name().c_str());

	mudo::ThreadLocalSingleton<Test>::destoty();
}

namespace {
int main() {
	mudo::ThreadLocalSingleton<Test>::instance().setName("main one");//创建线程单例
	mudo::Thread t1(boost::bind(threadFunc,"thread1"));
	mudo::Thread t2(boost::bind(threadFunc, "thread2"));
	t1.start();
	t2.start();

	t1.join();
	printf("tid=%d, %p name=%s\n",
		mudo::CurrentThread::tid(),
		&mudo::ThreadLocalSingleton<Test>::instance(),
		mudo::ThreadLocalSingleton<Test>::instance().name().c_str());
	t2.join();
	return 0;
}
}


