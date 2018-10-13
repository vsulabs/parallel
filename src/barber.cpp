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

#if 0
BinarySemaphore barberSleep;
BinarySemaphore customersWait;
#endif
BinarySemaphore barberSleep;
CountingSemaphore customersWait(CHAIRS_NUM);
BinarySemaphore accessSeats;


void randSleep(int time)
{
    sleep(rand() % time);
}

#define C(x) std::cout << "[CUST] " << #x << std::endl
#define B(x) std::cout << "[BARB] " << #x << std::endl

void customer(int value, std::queue<int>& queue)
{
    accessSeats.wait();
    std::cout << "Cusomer come: " << value << std::endl;
    if (queue.size() == CHAIRS_NUM) {
        // skip if no free chairs
        accessSeats.post();
        return;
    }

    C(push);
    queue.push(value);
    accessSeats.post();

    C(barberSleep.post());
    barberSleep.post();
    C(customersWait.wait());
    customersWait.wait();

    accessSeats.wait();
    C(pop);
    queue.pop();
    accessSeats.post();
}

void barber(std::queue<int>& queue, int count)
{
    while (queue.size() != 0 || count > 0) {
        B(barberSleep.wait());
        barberSleep.wait();

        accessSeats.wait();
        int val = queue.front();
        accessSeats.post();

        std::cout << "Barber starts: " << val << std::endl;
        randSleep(BARBER_TIME);

        B(customersWait.post());
        customersWait.post();
        count--;
    }
}

int countIntLines(const std::string& filename)
{
    std::ifstream f(filename);
    int val;
    int count = 0;
    while (f >> val)
        count++;

    return count;
}

int main(int argc, char *argv[])
{
    std::queue<int> queue;
    const std::string filename = "in.txt";
    const int count = countIntLines(filename);
    std::cout << "count = " << count << std::endl;

    barberSleep.wait();
    for (int i = 0; i < CHAIRS_NUM; ++i)
        customersWait.wait();

    std::thread barberThread(barber, std::ref(queue), count);

    std::ifstream f(filename);
    std::vector<std::thread> customersThread;
    int val;
    while (f >> val) {
        randSleep(COME_TIME);
        customersThread.push_back(std::thread(customer, val, std::ref(queue)));
    }

    for (auto& thread : customersThread) {
        thread.join();
    }

    barberThread.join();
}
