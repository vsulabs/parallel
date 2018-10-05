#include "common.h"
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <mutex>
#include <thread>

void multithread_findIndices(const uint* arr, size_t rowCount, size_t columns,
        size_t startRowInd, std::mutex &mutex, std::vector<Pos>& indices)
{
	std::vector<Pos> positions;
	const size_t endRowInd = startRowInd + rowCount;

	for (size_t i = startRowInd; i < endRowInd; ++i) {
		for (size_t j = 0; j < columns; j++) {
			const uint* num = arr + i * columns + j;
			const int sum = sumOfDigits(*num);

			if (sum % 7 == 0) {
				positions.push_back({ i, j });
			}
		}
	}

	std::lock_guard<std::mutex> lock(mutex);
	indices.insert(indices.cend(), positions.cbegin(), positions.cend());
}

void checkMultithread(const uint* arr, size_t rows, size_t columns, size_t prefThreadCount)
{
	std::vector<Pos> indices;
	const auto timeElapsed = doWithCppTimer([=, &indices]() {
		std::mutex mutex;
		std::vector<std::thread> threads;
		const size_t threadCount = getThreadCount(prefThreadCount, rows, MAX_THREAD_COUNT);
		const size_t rowsPerThread = rows / threadCount;
		size_t startRowInd = 0;
		for (size_t i = 0; i < threadCount - 1; ++i) {
			threads.push_back(std::thread(multithread_findIndices, arr,
			            rowsPerThread, columns, startRowInd, std::ref(mutex),
			            std::ref(indices)));
			startRowInd += rowsPerThread;
		}

		multithread_findIndices(arr, rows - startRowInd, columns, startRowInd,
		        std::ref(mutex), std::ref(indices));

		for (auto& t : threads)
			t.join();
	});

	/*
	std::cout << "Second:\n";
	printIndices(arr, rows, columns, indices);
	*/

	std::cout << "Time elaped: " << timeElapsed << "ms\n";
}

int main(int argc, char* argv[])
{
	const size_t prefThreadCount = argc == 2 ? atoi(argv[1]) : MAX_THREAD_COUNT;
	std::srand(std::time(nullptr));
	uint* arr = new uint[ROWS * COLUMNS];
	fillArray(arr, ROWS, COLUMNS);

	checkMultithread(arr, ROWS, COLUMNS, prefThreadCount);

	std::cout << std::endl;
#ifdef _WIN32
	system("pause");
#endif
	return 0;
}
