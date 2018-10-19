#define _REENTRANT

#include <iostream>
#include <fstream>
#include <queue>
#include <thread>
#include <vector>
#include <mutex>

#include <unistd.h>
#include "Semaphore.h"

constexpr int CHAIRS_NUM = 25;
constexpr int COME_TIME = 1;
constexpr int BARBER_TIME = 1;

SemaphoreImpl barberSleep(1);
SemaphoreImpl customersWait(CHAIRS_NUM);
SemaphoreImpl accessSeats(1);

SemaphoreImpl printSem(1);


void randSleep(int time)
{
    sleep(rand() % time);
}

#define DEBUG(name, x) do { \
    printSem.wait(); \
    std::cout << "[" name "] " << x << std::endl; \
    printSem.post(); \
} while(0)

#define C(x) DEBUG("CUST", #x)
#define B(x) DEBUG("BURB", #x)

void fillQueue(const std::string& filename, std::queue<int>& queue, std::atomic<size_t>& count)
{
    std::ifstream f(filename);
    int val;
    while (f >> val) {
        DEBUG("", "");
        randSleep(COME_TIME);

        accessSeats.wait();
        DEBUG("Cusomer come", val);
        if (queue.size() == CHAIRS_NUM) {
            // skip if no free chairs
            accessSeats.post();
            continue;
        }

        C(push(val));
        queue.push(val);
        accessSeats.post();

        C(barberSleep.post());
        barberSleep.post();
        C(customersWait.wait());
        customersWait.wait();

        accessSeats.wait();
        C(pop);
        queue.pop();
        if (queue.size() == 0)
            count--;
        accessSeats.post();
    }

    C(finish);
}

void barber(std::queue<int>& queue, const std::atomic<size_t>& count)
{
    while (queue.size() != 0 || count > 0) {
        DEBUG("", "");
        accessSeats.wait();
        if (queue.size() == 0 && count == 0) {
            accessSeats.post();
            return;
        }

        accessSeats.post();
        DEBUG("queue.size", queue.size());
        DEBUG("count", count);

        B(barberSleep.wait());
        barberSleep.wait();

        accessSeats.wait();
        int val = queue.front();
        accessSeats.post();

        DEBUG("Barber starts", val);
        randSleep(BARBER_TIME);

        B(customersWait.post());
        customersWait.post();
    }
}

int main(int argc, char *argv[])
{
    std::vector<std::string> files = { "1.txt", "2.txt" };
    std::atomic<size_t> count{files.size()};
    std::queue<int> queue;

    barberSleep.wait();
    for (int i = 0; i < CHAIRS_NUM; ++i)
        customersWait.wait();

    std::thread barberThread(barber, std::ref(queue), std::ref(count));

    std::vector<std::thread> customersThread;
    for (auto& filename : files) {
        randSleep(COME_TIME);
        customersThread.push_back(std::thread(fillQueue, std::ref(filename),
                    std::ref(queue), std::ref(count)));
    }

    for (auto& thread : customersThread) {
        thread.join();
    }

    barberThread.join();
}
