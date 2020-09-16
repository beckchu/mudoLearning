#include"mutex.h"
#include<boost/enable_shared_from_this.hpp>
#include<vector>
#include<boost/weak_ptr.hpp>
#include<boost/shared_ptr.hpp>

class Observable;

class Observer :public boost::enable_shared_from_this<Observer>{
public:
	virtual ~Observer() {}
	virtual void update() = 0;

	void observer(Observable* s);

protected:
	Observable* subject_;//这里没有用到智能指针，也算说的过去
};

class Observable {
public:
	void register_(boost::weak_ptr<Observer> x);
	void unregister_(boost::weak_ptr<Observer> x);

	void notifyObservers() {
		mudo::MutexLockGuard lock(mutex_);
		auto it = observers_.begin();
		while(it != observers_.end()) {
			boost::shared_ptr<Observer> obj(it->lock());
			if (obj) {
				obj->update();
				++it;
			}
			else {
				printf("notifyObservers() ereas\n");
				it=observers_.erase(it);//返回的是更新过后的迭代器,避免了迭代器失效
			}
		}
	}

private:
	mudo::MutexLock mutex_;
	std::vector<boost::weak_ptr<Observer>> observers_;
};

void Observer::observer(Observable* s) {
	s->register_(shared_from_this());//shared_ptr
	subject_ = s;
}

void Observable::register_(boost::weak_ptr<Observer> x) {//weak_ptr中有参数为shared_ptr&的拷贝构造函数，
	mudo::MutexLockGuard lock(mutex_);
	observers_.push_back(x);
}

class Foo :public Observer {
public:
	Foo(){
		
	}
	virtual void update() {
		printf("Foo::update() %p\n",this);
	}
};
namespace {
int main() {
	Observable subject;
	{
		boost::shared_ptr<Foo> p(new Foo);
		p->observer(&subject);
		subject.notifyObservers();
	}//离开作用域p就消失了
	subject.notifyObservers();
}

}

