#include "Semaphore.h"

#include <iostream>
#include <thread>
#include <vector>

void threadFunc(Semaphore *semaphore, int val) {
    for (int q = 0; q < 15; ++q) {
        semaphore->wait();
        for (int i = 0; i < 10; ++i) {
            std::cout << val << " ";
        }
        std::cout << "\n";
        semaphore->post();
    }

}

int main(int argc, char *argv[]) {
	Semaphore *semaphore = new SemaphoreImpl(1);
    std::vector<std::thread> threads;

    for (int i = 0; i < 10; i++)
        threads.push_back(std::thread(threadFunc, semaphore, i));

    for (auto& t : threads)
        t.join();

	delete semaphore;

#ifdef _WIN32
	system("PAUSE");
#endif
	return 0;
}
