#pragma once

#include "atomic.h"

#include<pthread.h>
#include<iostream>
#include<boost/noncopyable.hpp>
#include<boost/function.hpp>

namespace mudo {
	class Thread :boost::noncopyable {
	public:
		typedef boost::function<void()> ThreadFunc;//指向任意函数，和函数对象

		Thread(const ThreadFunc& func, const std::string& name = std::string());
		~Thread();

		void start();
		void join();

		bool started() const { return started_; }
		pid_t tid() const { return tid_; }
		const std::string& name() const { return name_; }
	private:
		static void* startThread(void* thread);
		void runInThread();

		std::string name_;
		bool started_;
		pthread_t pthreadId_;
		pid_t tid_;
		ThreadFunc func_;

		static AtomicInt32 numCreated_;
	};

	namespace CurrentThread {
		pid_t tid();
		const char* name();
		bool isMainThread();
	}
}
