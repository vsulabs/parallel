#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <functional>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

#include <omp.h>

using uint = unsigned int;

constexpr size_t ROWS = 100000;
constexpr size_t COLUMNS = 1000;
constexpr size_t MAX_THREAD_COUNT = 4;
constexpr size_t MIN_BLOCK_SIZE = 100;
constexpr bool PRINT_RESULT = false;


void fillArray(uint* arr, size_t rowCount, size_t columnsCount)
{
	std::generate(arr, arr + rowCount * columnsCount, std::rand);
}

uint sumOfDigits(uint num)
{
	uint sum = 0;
	while (num > 0) {
		sum += num % 10;
		num /= 10;
	}

	return sum;
}

struct Pos
{
	size_t i;
	size_t j;
};

void printMatrix(const uint* arr, size_t rowCount, size_t columnsCount)
{
	for (size_t i = 0; i < rowCount; ++i) {
		for (size_t j = 0; j < columnsCount; ++j) {
			const uint* num = arr + i * columnsCount + j;
			std::cout << *num << "\t";
		}

		std::cout << "\n";
	}
}

void printIndices(const uint* arr, size_t rowCount, size_t columnsCount, const std::vector<Pos>& indices)
{
	for (auto pos : indices) {
		const uint* num = arr + pos.i * columnsCount + pos.j;
		std::cout << pos.i << " " << pos.j << " " << *num << "\n";
	}
}

size_t getThreadCount(size_t prefThreadCount, size_t rowCountCount)
{
	if (prefThreadCount < 0)
		return 1;

	if (rowCountCount < MIN_BLOCK_SIZE)
		return 1;

	const size_t maxThreadCountForData = rowCountCount / MIN_BLOCK_SIZE;
	const size_t maxThreadCount = std::min(maxThreadCountForData, MAX_THREAD_COUNT);
	return std::min(prefThreadCount, maxThreadCount);
}

void checkMultithread(const uint* arr, size_t rowCount, size_t columnsCount, size_t prefThreadCount)
{
	std::mutex mutex;
	std::vector<Pos> indices;

	omp_set_num_threads(getThreadCount(prefThreadCount, rowCount));
	const auto startTime = std::chrono::high_resolution_clock::now();
#pragma omp parallel
	{
		//std::cout << "Thread number: " << omp_get_thread_num() << std::endl;
		std::vector<Pos> positions;
#pragma omp for
		for (size_t i = 0; i < rowCount; ++i) {
			for (size_t j = 0; j < columnsCount; ++j) {
				const uint* num = arr + i * columnsCount + j;
				const int sum = sumOfDigits(*num);

				if (sum % 7 == 0)
					positions.push_back({ i, j });
			}
		}

		std::lock_guard<std::mutex> lock(mutex);
		indices.insert(indices.cend(), positions.cbegin(), positions.cend());
	}
	const auto endTime = std::chrono::high_resolution_clock::now();
	const auto timeElapsed = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();

	if (PRINT_RESULT) {
		std::cout << "First:\n";
		printIndices(arr, rowCount, columnsCount, indices);
	}

	std::cout << "Time elaped: " << timeElapsed << '\n';
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
