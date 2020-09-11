#pragma once

#include<pthread.h>

namespace mudo {
	template<typename T>
	class ThreadLocal {//建议使用RAII来实现
	public:
		ThreadLocal() {
			pthread_key_create(&pkey_, &ThreadLocal::destruct);
		}

		~ThreadLocal() {
			pthread_key_delete(pkey_);
		}

		T& value() {
			T* preThreadValue = static_cast<T*>(pthread_getspecific(pkey_));
			if (!preThreadValue) {
				T* newobj = new T();
				pthread_setspecific(pkey_, newobj);
				preThreadValue = newobj;
			}
			return *preThreadValue;
		}
	private:
		static void destruct(void* x) {//这里一定要是静态成员函数
		//void destruct(void* x) {
			T* obj = static_cast<T*>(x);
			delete obj;
		}
	private:
		pthread_key_t pkey_;//和__thread作对比，相似
	};
}
