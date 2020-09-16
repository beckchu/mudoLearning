#pragma once
#include"mutex.h"
#include"condition.h"
#include"thread.h"

#include<deque>
#include<boost/noncopyable.hpp>
#include<boost/ptr_container/ptr_vector.hpp>

namespace mudo{

class ThreadPool : boost::noncopyable{
public:
	typedef boost::function<void()> Task;

	explicit ThreadPool(const std::string& name = std::string());
	~ThreadPool();

	void start(int numThreads);
	void stop();

	void run(const Task& f);
private:
	void runInThread();
	Task take();

	mudo::MutexLock mutex_;
	mudo::Condition cond_;
	std::string name_;
	boost::ptr_vector<mudo::Thread> threads_;//放进去的指针在出了作用域后直接删除
	std::deque<Task> queue_;
	bool running_;
};

}
