#include"mutex.h"
#include<set>

class Request;

class Inventory {
public:
	void add(Request* req) {
		mudo::MutexLockGuard lock(mutex_);
		requests_.insert(req);
	}

	void remove(Request* req) {
		mudo::MutexLockGuard lock(mutex_);
		requests_.erase(req);
	}

	void printAll() const;

private:
	mutable mudo::MutexLock mutex_;
	std::set<Request*> requests_;
};

Inventory g_inventory;

class Request {
public:
	Request() :mutex_() {

	}

	void process(){
		mudo::MutexLockGuard lock(mutex_);//dead lock
		g_inventory.add(this);//this pointer none const 
	}

	~Request() {
		mudo::MutexLockGuard lock(mutex_);//lock1 here
		sleep(1);
		g_inventory.remove(this);//lock2 here
	}

	void print() const {
		mudo::MutexLockGuard lock(mutex_);//const can not change value in class ,lock1 here

	}
private:
	mutable mudo::MutexLock mutex_;//mutable is must
};


void Inventory::printAll() const {
	mudo::MutexLockGuard lock(mutex_);//lock2 here
	sleep(1);
	for (auto s : requests_) {
		s->print();
	}

	printf("Inventory::printAll() unlocked\n");
}

namespace {
void threadFunc() {
	Request* req = new Request;
	req->process();
	delete req;
}

//lock1 and lock2 dead lock
//./exe
//gdb 
//(gdb) attach pid
//(gdb) thread apply all bt 打印出所有线程栈信息
//查看backtrace，倒序
int main() {
	mudo::Thread thread(threadFunc);
	thread.start();
	usleep(500 * 1000);
	g_inventory.printAll();
	thread.join();
}
}

