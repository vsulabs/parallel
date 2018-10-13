#include <iostream>
#include <thread>

#ifdef _WIN32
	#include <Windows.h>
#else
	#include <unistd.h>
#endif



class Semaphore {
public:
	virtual void wait() = 0;
	virtual void post() = 0;
};

class Mutex {
private:
	volatile unsigned long long m_interlock;
public:
	Mutex() : m_interlock(0) {}
	void lock() {
#ifdef _WIN32
		while (m_interlock == 1 || InterlockedCompareExchange(&m_interlock, 1, 0) == 1);
#else
		while (m_interlock == 1 || __sync_lock_test_and_set(&m_interlock, 1) == 1);
#endif
	}

	void unlock() {
		m_interlock = 0;
	}

	~Mutex() {}
};

class BinarySemaphore : public Semaphore {
private:
	bool m_state;
	Mutex m_mutex;
public:
	BinarySemaphore() :
		m_state(true) { }

	void wait() override {
		m_mutex.lock();

		if (m_state) {
			m_state = false;
		}
		else {
			m_mutex.unlock();
			while (!m_state);
			m_mutex.lock();
			m_state = false;
		}

		m_mutex.unlock();

	}

	void post() override {
		m_mutex.lock();
		m_state = true;
		m_mutex.unlock();
	}
};

class CountingSemaphore : public Semaphore {
private:
	int m_count;
	int m_maxCount;
	Mutex m_mutex;
public:
	CountingSemaphore(int maxCount) :
		m_maxCount(maxCount), m_count(maxCount) { }

	void wait() override {
		m_mutex.lock();

		if (m_count > 0) {
			--m_count;
		}
		else {
			m_mutex.unlock();
			while (m_count <= 0);
			m_mutex.lock();
			--m_count;
		}

		m_mutex.unlock();

	}

	void post() override {
		m_mutex.lock();
		++m_count;
		m_mutex.unlock();
	}
};


#if 0
void threadFunc(Semaphore *semaphore) {
	semaphore->wait();
	for (int i = 0; i < 10000; i++) {
		std::cout << i << " ";
	}

	std::cout << "\n\n\n\n\n\n\n";
	semaphore->post();
}

int main(int argc, char *argv[]) {
	Mutex mutex;
	Semaphore *semaphore = new BinarySemaphore();

	std::thread t1(threadFunc, semaphore);
	std::thread t2(threadFunc, semaphore);

	t1.join();
	t2.join();

	delete semaphore;

#ifdef _WIN32
	system("waitAUSE");
#endif
	return 0;
}
#endif
