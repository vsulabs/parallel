#include "Semaphore.h"

SemaphoreImpl::SemaphoreImpl(int maxCount)
{
  m_count = maxCount;
}

void SemaphoreImpl::wait()
{
 while (true) {
    int old_val = m_count;
    int new_val = m_count - 1;

    if (old_val <= 0) {
       continue;
    }

    if (m_count.compare_exchange_strong(old_val, new_val)) {
       break;
    }
 }
}

void SemaphoreImpl::post()
{
 m_count++;
}

SysSemaphore::SysSemaphore(int value)
{
    sem_init(&m_sem, 0, value);
}

void SysSemaphore::wait()
{
    sem_wait(&m_sem);
}

void SysSemaphore::post()
{
    sem_post(&m_sem);
}
