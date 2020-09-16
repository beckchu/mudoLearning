#include<vector>
#include<boost/shared_ptr.hpp>
#include"mutex.h"

using namespace mudo;

namespace {
class Foo {
public:
	void doit() const;
};

typedef std::vector<Foo> FooList;
typedef boost::shared_ptr<FooList> FooListPtr;


FooListPtr g_foos;
MutexLock mutex;
void post(const Foo& f) {
	printf("post\n");
	MutexLockGuard lock(mutex);
	if (!g_foos.unique()) {//使用不唯一
		g_foos.reset(new FooList(*g_foos));//重新new了一个vector，之前的有其他的shared_ptr来维护
		printf("copy the whole list\n");
	}
	assert(g_foos.unique());
	g_foos->push_back(f);
}

void traverse() {
	FooListPtr foos;
	{
		MutexLockGuard lock(mutex);
		foos = g_foos;//增加了计数
		assert(!g_foos.unique());
	}
	assert(!foos.unique());
	for (std::vector<Foo>::const_iterator it = foos->begin(); it != foos->end();it++) {
		it->doit();
	}
}

void Foo::doit() const {
	Foo f;
	post(f);
}


int main() {
	g_foos.reset(new FooList);//初始化
	Foo f;
	post(f);
	traverse();
	return 0;
}

}
