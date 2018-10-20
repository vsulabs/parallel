#define _REENTRANT

#include <iostream>
#include <fstream>
#include <queue>
#include <thread>
#include <vector>
#include <mutex>

#include <unistd.h>
#include "Semaphore.h"

constexpr int CHAIRS_NUM = 5;
constexpr int COME_TIME = 3;
constexpr int BARBER_TIME = 5;

SemaphoreImpl barberSleep(1);
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

void fillQueue(const std::string& filename, std::queue<std::string>& queue)
{
    std::ifstream f(filename);
    std::string val;
    while (f >> val) {
        randSleep(COME_TIME);

        accessSeats.wait();
        DEBUG("Cusomer come", val);
        if (queue.size() == CHAIRS_NUM) {
            // skip if no free chairs
            DEBUG("Cusomer gone", val);
            accessSeats.post();
            continue;
        }

        DEBUG("Cusomer took a turn", val);
        queue.push(val);
        accessSeats.post();

        barberSleep.post();
    }
}

void barber(std::queue<std::string>& queue)
{
    while (true) {
        barberSleep.wait();

        accessSeats.wait();
        const std::string val = queue.front();
        queue.pop();
        accessSeats.post();

        DEBUG("Barber started", val);
        randSleep(BARBER_TIME);
        DEBUG("Barber finished", val);
    }
}

int main(int argc, char *argv[])
{
    std::vector<std::string> files = { "1.txt", "2.txt", "3.txt" };
    std::queue<std::string> queue;

    barberSleep.wait();
    std::thread barberThread(barber, std::ref(queue));

    std::vector<std::thread> customersThread;
    for (auto& filename : files) {
        randSleep(COME_TIME);
        customersThread.push_back(std::thread(fillQueue, std::ref(filename),
                    std::ref(queue)));
    }

    for (auto& thread : customersThread) {
        thread.join();
    }

    barberThread.join();
}
