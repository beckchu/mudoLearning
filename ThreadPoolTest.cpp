
#include"threadPool.h"
#include"countDownLatch.h"
#include<boost/bind.hpp>
namespace {

void print() {
	printf("tid=%d \n", mudo::CurrentThread::tid());
}

void printString(const std::string& str) {
	printf("tid=%d, str=%s\n", mudo::CurrentThread::tid(), str.c_str());
}

int main() {
	mudo::ThreadPool pool("MainThreadPool");
	pool.start(5);

	pool.run(print);
	pool.run(print);
	for (int i = 0; i < 100; i++) {
		char buf[32];
		snprintf(buf, sizeof(buf), "task %d", i);
		pool.run(boost::bind(printString, std::string(buf)));
	}

	mudo::CountDownLatch latch(1);
	pool.run(boost::bind(&mudo::CountDownLatch::countDown, &latch));
	latch.wait();
	pool.stop();
}
}
