#include"mutex.h"

#include<string>
#include<vector>
#include<unordered_map>
#include<boost/noncopyable.hpp>
#include<boost/shared_ptr.hpp>

namespace{


class CustomerData:boost::noncopyable {
public:
	CustomerData():data_(new mp) {
	}

	int query(const std::string& customer, const std::string& stock) const;


private:
	typedef std::pair<std::string, int> Entry;
	typedef std::vector<Entry> EntryList;
	typedef std::unordered_map<std::string, EntryList> mp;
	typedef boost::shared_ptr<mp> mapPtr;

private:
	void update(const std::string& customer, const EntryList& entries);
	void update(const std::string& message);

	static int findEntry(const EntryList& entries, const std::string& stock);
	static mapPtr parseData(const std::string& message);

	mapPtr getData() const {
		mudo::MutexLockGuard lock(mutex_);
		return data_;//shared_ptr count++
	}

	mutable mudo::MutexLock mutex_;
	mapPtr data_;
};

int CustomerData::findEntry(const EntryList& entries, const std::string& stock) {

}
boost::shared_ptr<std::unordered_map<std::string, std::vector<std::pair<std::string, int>>>> CustomerData::parseData(const std::string& message) {

}

int CustomerData::query(const std::string& customer, const std::string& stock) const {
	mapPtr data = getData();
	mp::const_iterator entries = data->find(customer);
	if (entries != data->end()) {
		return findEntry(entries->second, stock);
	}
	else {
		return -1;
	}
}

//copy on write
void CustomerData::update(const std::string& customer, const EntryList& entries) {
	mudo::MutexLockGuard lock(mutex_);
	if (!data_.unique()) {
		mapPtr newData(new mp(*data_));
		//data_.reset(new mp(*data_));
		data_.swap(newData);
	}
	assert(data_.unique());
	(*data_)[customer] = entries;//智能指针和普通指针的用法其实差别不大
}

void CustomerData::update(const std::string& message) {
	mapPtr newData = parseData(message);
	if (newData) {
		mudo::MutexLockGuard lock(mutex_);
		data_.swap(newData);
	}
}


int main() {
	CustomerData data;
}

}
