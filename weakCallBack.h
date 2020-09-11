#pragma once
#include<memory>
#include<functional>
//这个模板很有代表意义
namespace mudo {
template<typename CLASS,typename ... ARGS>
class WeakCallback {
public:
	//参数都是对象，智能指针其实就是对象             CLASS* or CLASS&主要调用时的传參
	WeakCallback(const std::weak_ptr<CLASS> ptr, 
				  const std::function<void (CLASS*, ARGS...)>& function)
						:object_(ptr),function_(function) {

	}

	void operator()(ARGS&&... args) const{
		std::shared_ptr<CLASS> ptr(object_.lock());
		if (ptr) {
			function_(ptr.get(), std::forward<ARGS>(args)...);//避免了rv and lv 传输过程的丢失
		}
	}

private:
	std::weak_ptr<CLASS> object_;
	std::function<void (CLASS*, ARGS...)> function_;
};

template<typename CLASS,typename... ARGS>
WeakCallback<CLASS, ARGS...> makeWeakCallback(const std::shared_ptr<CLASS>& object,
												void (CLASS::* function)(ARGS...) const) {//直接参考cplusplus：std::function
	return WeakCallback<CLASS, ARGS...>(object, function);
}

template<typename CLASS, typename... ARGS>
WeakCallback<CLASS, ARGS...> makeWeakCallback(const std::shared_ptr<CLASS>& object,//from shared_ptr to weak_ptr,create a temp value,
	void (CLASS::* function)(ARGS...)) {
	return WeakCallback<CLASS, ARGS...>(object, function);
}

}

