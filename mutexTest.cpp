#include"mutex.h"
#include<vector>

using namespace mudo;

class Foo {
public:
	void doit() const;
};

MutexLock mutex;
std::vector<Foo> foos;

void post(const Foo& foo) {
	MutexLockGuard lock(mutex);
	foos.push_back(foo);
}

void Foo::doit() const {
	Foo f;
	post(f);
}

void travel() {
	//迭代器有可能失效的
	for (auto v : foos) {
		v.doit();
	}
}
namespace {
int main() {
	Foo f;
	post(f);
	travel();
}
}
