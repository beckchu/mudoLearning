#include"mutex.h"

void someFunctionMayCallExit() {
	exit(1);
}

class GlobalObject {
public:
	void doit() {
		mudo::MutexLockGuard lock(mutex_);//lock1
		someFunctionMayCallExit();
	}

	~GlobalObject() {
		printf("GlobalObject:~GlobalObject\n");
		mudo::MutexLockGuard g(mutex_);//lock2 ,,dead lock
		printf("GlobalObject:~GlobalObject\n");
	}

private:
	mudo::MutexLock mutex_;
};

namespace {
GlobalObject g_obj;

int main() {
	g_obj.doit();
}
}
