#include"threadPool.h"
#include"Exception.h"

#include<boost/bind.hpp>

using namespace mudo;

ThreadPool::ThreadPool(const std::string& name)
	: mutex_(),
	cond_(mutex_),
	name_(name),
	running_(false)
{
}

ThreadPool::~ThreadPool() {
	if (running_) {//这里怎么知道其他线程已经运行完了，会有提前退出的情况
		stop();
	}
}

void ThreadPool::start(int numThreads) {
	assert(threads_.empty());
	running_ = true;
	threads_.reserve(numThreads);
	for (int i = 0; i < numThreads; i++) {
		char id[32];
		snprintf(id, sizeof(id), "%d", i);
		threads_.push_back(new mudo::Thread(boost::bind(&ThreadPool::runInThread, this), name_ + id));
		threads_[i].start();
	}
}

void ThreadPool::stop() {
	running_ = false;
	cond_.notifyAll();//join和notify的先后顺序没有关系，task会判空
	for_each(threads_.begin(), threads_.end(),
		boost::bind(&mudo::Thread::join, _1)); //可参照bind class member function manual
}

void ThreadPool::run(const Task& task) {
	if (threads_.empty()) {
		task();
	}
	else {
		mudo::MutexLockGuard lock(mutex_);
		queue_.push_back(task);
		cond_.notify();
	}
}

ThreadPool::Task ThreadPool::take() {
	mudo::MutexLockGuard lock(mutex_);
	while (queue_.empty() && running_) {
		cond_.wait();
	}
	Task task;
	if (!queue_.empty()) {
		task = queue_.front();
		queue_.pop_front();
	}
	return task;
}

void ThreadPool::runInThread() {
	try {
		while (running_) {
			Task task(take());
			if (task) {
				task();
			}
		}
	}
	catch(const mudo::Exception& ex){
		fprintf(stderr, "exeption caught in ThreadPool %s\n", name_.c_str());
		fprintf(stderr, "reason:%s\n", ex.what());
		fprintf(stderr, "stack trace:%s\n", ex.stackTrace());
		abort();
	}
	catch (const std::exception& ex) {
		fprintf(stderr, "exception caught in ThreadPool %s\n", name_.c_str());
		fprintf(stderr, "reason:%s\n", ex.what());
		abort();
	}
	catch (...) {
		fprintf(stderr, "unknown exception caught in ThreadPool %s\n", name_.c_str());
		abort();
	}
}
