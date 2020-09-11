#pragma once
//相当于线程单例
#include<boost/noncopyable.hpp>

namespace mudo {
	template<typename T>
	class ThreadLocalSingleton:boost::noncopyable {
	public:
		static T& instance() {
			if (!value_) {
				value_ = new T();
			}
			return *value_;
		}
	
		static void destoty() {
			if (value_) {
				delete value_;
			}
			value_ = nullptr;
		}
	private:
		static __thread T* value_;
	};

template<typename T> __thread T* ThreadLocalSingleton<T>::value_ = nullptr;//__thread是gcc内置类型，
}


