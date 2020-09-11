#pragma once
#include<pthread.h>

template<typename T>
class singleton {
public:
	static T& instance() {
		pthread_once(&ponce,&init);
		return *value_;
	}

	static void init() {
		value_ = new T();
		::atexit(destory);//程序正常终止时退出
	}

	static void destory() {
		delete value_;
	}

	
private:
	singleton() {}
	~singleton() {}
private:
	static pthread_once_t ponce_;
	static T* value_;
};

template<typename T> pthread_once_t singleton<T>::ponce_= PTHREAD_ONCE_INIT;
template<typename T> T* singleton<T>::value_ = nullptr;
