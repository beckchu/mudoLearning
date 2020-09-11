#include "thread.h"
#include"threadLocal.h"


class Test :boost::noncopyable {
public:
	Test() {
		printf("tid=%d, constructing %p\n", mudo::CurrentThread::tid(), this);
	}
	~Test() {
		printf("tid=%d, destructing %p %s\n", mudo::CurrentThread::tid(), this,name_.c_str());
	}
	const std::string name() const { return name_; }
	void setName(const std::string& n) {
		name_ = n;
	}
private:
	std::string name_;
};

mudo::ThreadLocal<Test> obj1;
mudo::ThreadLocal<Test> obj2;

namespace {
//其他文件中也有同样的function
void print() {
	printf("tid=%d, obj1 %p name=%s\n",mudo::CurrentThread::tid(),&obj1.value(),obj1.value().name().c_str());
	printf("tid=%d, obj2 %p name=%s\n", mudo::CurrentThread::tid(), &obj2.value(), obj2.value().name().c_str());
}

void threadFunc() {
	print();
	obj1.value().setName("obj1");
	obj2.value().setName("obj2");
	print();
}


int main() {//直接看调试信息非常完整
	obj1.value().setName("main one");
	print();
	mudo::Thread t1(threadFunc,"t1");
	t1.start();
	t1.join();
	obj2.value().setName("main two");
	print();

	pthread_exit(0);
}
}


