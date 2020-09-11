#pragma once

#include"thread.h"
#include<boost/noncopyable.hpp>
#include<mutex>
#include<assert.h>

namespace mudo {
	class MutexLock :boost::noncopyable {
	public:
		MutexLock() :holder_(0) {
			pthread_mutex_init(&mutex_, NULL);
		}
		~MutexLock() {
			assert(holder_ == 0);
			pthread_mutex_destroy(&mutex_);
		}

		bool isLockByThisThread() {
			return holder_ == CurrentThread::tid();
		}

		void assertLock() {
			assert(isLockByThisThread());
		}

		void lock() {
			holder_ = CurrentThread::tid();
			pthread_mutex_lock(&mutex_);
		}

		void unlock() {
			holder_ = 0;
			pthread_mutex_unlock(&mutex_);
		}

		pthread_mutex_t* getPthreadMutex() {//is local value
			return &mutex_;
		}

	private:
		pthread_mutex_t mutex_;
		pid_t holder_;
	};

	class MutexLockGuard :boost::noncopyable{
	public:
		explicit MutexLockGuard(MutexLock& mutex):mutex_(mutex) {
			mutex_.lock();
		}

		~MutexLockGuard() {
			mutex_.unlock();
		}
	private:
		MutexLock& mutex_;
	};
	
//
#define MutexLockGuard(x) "miss lock guard name..."

}


