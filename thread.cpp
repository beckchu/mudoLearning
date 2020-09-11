#include "thread.h"

#include<sys/syscall.h>

namespace mudo {
namespace CurrentThread {
	__thread const char* t_threadName = "unknown";//不能修饰局部变量，只能修饰全局变量，初始化只能在编译期
}
}

namespace {
	__thread pid_t t_cachedTid=0;//只能修饰POD数据类型

	pid_t gettid() {
		return static_cast<pid_t>(::syscall(SYS_gettid));
	}

	void afterFork() {
		t_cachedTid = gettid();
		mudo::CurrentThread::t_threadName = "child";
	}

	class ThreadNameInitializer {
	public:
		ThreadNameInitializer() {
			mudo::CurrentThread::t_threadName = "parent";
			pthread_atfork(NULL, NULL, &afterFork);//prepare  parent  child
		}
	};

	ThreadNameInitializer init;
}

using namespace mudo;

pid_t CurrentThread::tid() {
	if (t_cachedTid == 0) {
		t_cachedTid = gettid();
	}
	return t_cachedTid;
}

const char* CurrentThread::name() {
	return t_threadName;
}

bool CurrentThread::isMainThread() {
	return tid() == ::getpid();
}

AtomicInt32 Thread::numCreated_;

Thread::Thread(const ThreadFunc& func, const std::string& n)
	:started_(false),
	pthreadId_(0),
	tid_(0),
	func_(func),
	name_(n)
{
	numCreated_.increment();
}

Thread::~Thread() {

}

void Thread::start() {
	assert(!started_);
	started_ = true;
	pthread_create(&pthreadId_, NULL, &startThread, this);//函数定义
}

void Thread::join() {
	assert(started_);
	pthread_join(pthreadId_, NULL);
}

void* Thread::startThread(void* obj) {
	Thread* thread = static_cast<Thread*>(obj);
	thread->runInThread();
	return NULL;
}

void Thread::runInThread() {
	tid_ = CurrentThread::tid();
	mudo::CurrentThread::t_threadName = name_.c_str();
	std::cout << mudo::CurrentThread::t_threadName << std::endl;
	func_();
	mudo::CurrentThread::t_threadName = "finished";
	std::cout << mudo::CurrentThread::t_threadName << std::endl;
}


