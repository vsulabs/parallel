#include <semaphore.h>
#include <atomic>


class Semaphore
{
public:
	virtual void wait() = 0;
	virtual void post() = 0;
};

class SemaphoreImpl : public Semaphore
{
public:
  SemaphoreImpl(int maxCount);
  void wait() override;
  void post() override;

private:
  std::atomic<int> m_count;
};


class SysSemaphore : public Semaphore
{
public:
	SysSemaphore(int value);

	void wait() override ;
	void post() override;

private:
	sem_t m_sem;
};
