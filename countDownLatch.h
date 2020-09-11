#pragma once
#include"mutex.h"
#include"condition.h"
#include<boost/noncopyable.hpp>

namespace mudo {
	class CountDownLatch:boost::noncopyable {
	public:
		explicit CountDownLatch(int count) :
			count_(count), 
			mutex_(),//actor
			condition_(mutex_){

		}

		void wait() {
			MutexLockGuard lock(mutex_);
			if (count_ > 0) {
				condition_.wait();
			}
		}

		void countDown() {
			MutexLockGuard lock(mutex_);
			count_--;
			if (count_ == 0) {
				condition_.notifyAll();
			}
		}

		int gerCount() {
			MutexLockGuard lock(mutex_);
			return count_;
		}

	private:
		int count_;
		Condition condition_;
		MutexLock mutex_;
	};

}



