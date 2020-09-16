#include"mutex.h"
#include<string>
#include<map>
#include<boost/noncopyable.hpp>
#include<boost/enable_shared_from_this.hpp>
#include<boost/shared_ptr.hpp>
#include<boost/weak_ptr.hpp>
#include<boost/bind.hpp>

class Stock :boost::noncopyable{
public:
	Stock(std::string name) :name_(name) {
		printf("Stock[%p], %s\n", this, name_.c_str());
	}

	~Stock() {
		printf("~Stock[%p], %s\n", this, name_.c_str());
	}

	const std::string& key() const{ return name_; }

private:
	std::string name_;
};

namespace version1 {
class StockFactory :boost::noncopyable {
public:
	boost::shared_ptr<Stock> get(const std::string& key) {
		mudo::MutexLockGuard lock(mutex_);
		boost::shared_ptr<Stock>& pStock = stocks_[key];//返回second的默认构造函数
		if (!pStock) {
			pStock.reset(new Stock(key));
		}
		return pStock;
	}
	
private:
	mudo::MutexLock mutex_;
	std::map<std::string, boost::shared_ptr<Stock>> stocks_;//随着stock的增加，map会逐渐变大，且对应的stock不被delete
};
}


namespace version2 {
	class StockFactory :boost::noncopyable {
	public:
		boost::shared_ptr<Stock> get(const std::string& key) {
			boost::shared_ptr<Stock> pStock;
			mudo::MutexLockGuard lock(mutex_);
			boost::weak_ptr<Stock>& wkStock = stocks_[key];
			pStock = wkStock.lock();
			if (!pStock) {
				pStock.reset(new Stock(key));
				wkStock = pStock;//wkStock是reference
			}
			return pStock;//返回的是copy
		}

	private:
		mudo::MutexLock mutex_;
		std::map<std::string, boost::weak_ptr<Stock>> stocks_;//对应的stock在没有share_ptr时会被delete，但是map中的weak_ptr不会被处理
	};
}

namespace version3 {
	class StockFactory :boost::noncopyable {
	public:
		boost::shared_ptr<Stock> get(const std::string& key) {
			boost::shared_ptr<Stock> pStock;
			mudo::MutexLockGuard lock(mutex_);
			boost::weak_ptr<Stock>& wkStock = stocks_[key];
			pStock = wkStock.lock();
			if (!pStock) {
				pStock.reset(new Stock(key),
					boost::bind(&StockFactory::deleteStock, this, _1));//这个用法很奇怪
				wkStock = pStock;//wkStock是reference
			}
			return pStock;//返回的是copy
		}

	private:
		void deleteStock(Stock* stock) {//有了delete函数，就要手动delete，这里存在一个问题StockFactory可能提前dead
			printf("deleteStock[%p]\n",stock);
			if (stock) {
				mudo::MutexLockGuard lock(mutex_);
				stocks_.erase(stock->key());
			}
			delete stock;
		}

	private:
		mudo::MutexLock mutex_;
		std::map<std::string, boost::weak_ptr<Stock>> stocks_;//对应的stock在没有share_ptr时会被delete，但是map中的weak_ptr不会被处理
	};
}

//这个版本基本上是可用的了
namespace version4 {
	class StockFactory :public boost::enable_shared_from_this<StockFactory>,
						boost::noncopyable {
	public:
		boost::shared_ptr<Stock> get(const std::string& key) {//thread2
			boost::shared_ptr<Stock> pStock;
			mudo::MutexLockGuard lock(mutex_);
			boost::weak_ptr<Stock>& wkStock = stocks_[key];
			pStock = wkStock.lock();
			if (!pStock) {
				pStock.reset(new Stock(key),
					boost::bind(&StockFactory::deleteStock, shared_from_this(), _1));//这个用法很奇怪,改变了this指针StockFactory就可以被维护
				wkStock = pStock;//wkStock是reference
			}
			return pStock;//返回的是copy
		}

	private:
		void deleteStock(Stock* stock) {
			//收藏博客有具体分析
			printf("deleteStock[%p]\n", stock);//多线程下会有问题
			if (stocks_[stock->key()].expired()){//判空
			//if (stock) {
				mudo::MutexLockGuard lock(mutex_);
				stocks_.erase(stock->key());
			}
			delete stock;
		}

	private:
		mudo::MutexLock mutex_;
		std::map<std::string, boost::weak_ptr<Stock>> stocks_;//对应的stock在没有share_ptr时会被delete，但是map中的weak_ptr不会被处理
	};
}

//画蛇添足，实现弱引用
namespace  {
	class StockFactory :public boost::enable_shared_from_this<StockFactory>,
		boost::noncopyable {
	public:
		boost::shared_ptr<Stock> get(const std::string& key) {
			boost::shared_ptr<Stock> pStock;
			mudo::MutexLockGuard lock(mutex_);
			boost::weak_ptr<Stock>& wkStock = stocks_[key];
			pStock = wkStock.lock();
			if (!pStock) {
				pStock.reset(new Stock(key),
					boost::bind(&StockFactory::deleteStock, shared_from_this(), _1));//这个用法很奇怪,改变了this指针StockFactory就可以被维护
				wkStock = pStock;//wkStock是reference
			}
			return pStock;//返回的是copy
		}

	private:
		static void deleteStock(const boost::weak_ptr<StockFactory>& wkFactory,Stock* stock) {//must be static
			printf("deleteStock[%p]\n", stock);
			boost::shared_ptr<StockFactory> factory(wkFactory.lock());
			if (factory) {
				factory->removeStock(stock);
			}
			else {
				printf("factory died.\n");
			}
			delete stock;
		}

		void removeStock(Stock* stock) {
			if (stock) {
				mudo::MutexLockGuard lock(mutex_);
				stocks_.erase(stock->key());
			}
	}

	private:
		mudo::MutexLock mutex_;
		std::map<std::string, boost::weak_ptr<Stock>> stocks_;//对应的stock在没有share_ptr时会被delete，但是map中的weak_ptr不会被处理
	};
}

void testShortLifeFactory() {
	boost::shared_ptr<Stock> stock;
	{
		boost::shared_ptr<StockFactory> factory(new StockFactory);//so deleteStock must static
		stock = factory->get("NYSE:IBM");
		boost::shared_ptr<Stock> stock2 = factory->get("NYSE:IBM");
		assert(stock == stock2);
	}//factory destructs here 这里用的是version5的版本如果用version4 stock he stockfactory强绑定,
	//stock destructs here ;so deleteStock must static
}

void testLongLifeFactory() {
	boost::shared_ptr<StockFactory> factory(new StockFactory);
	{
		boost::shared_ptr<Stock> stock = factory->get("NYSE:IBM");
		boost::shared_ptr<Stock> stock2 = factory->get("NYSE:IBM");
		assert(stock == stock2);
	}//stock destructs here
	//factory destructs here
}

int main() {
	version1::StockFactory sf1;
	version2::StockFactory sf2;
	version3::StockFactory sf3;//以上版本，没有share_from_this,即没有使用this指针
	boost::shared_ptr<version4::StockFactory> sf4(new version4::StockFactory);
	boost::shared_ptr<StockFactory> sf5(new StockFactory);

	{
		boost::shared_ptr<Stock> s1 = sf1.get("stock1");//life with factory
	}

	{
		boost::shared_ptr<Stock> s2 = sf2.get("stock2");//life with factory
	}

	{
		boost::shared_ptr<Stock> s3 = sf3.get("stock3");
	}

	{
		boost::shared_ptr<Stock> s4 = sf4->get("stock4");
	}

	{
		boost::shared_ptr<Stock> s5 = sf5->get("stock5");
	}

	testLongLifeFactory();
	testShortLifeFactory();
}
