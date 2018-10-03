#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <functional>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

using uint = unsigned int;

constexpr size_t ROWS = 100000;
constexpr size_t COLUMNS = 1000;
constexpr size_t MAX_THREAD_COUNT = 4;
constexpr size_t MIN_BLOCK_SIZE = 100;


void fillArray(uint* arr, size_t rows, size_t columns)
{
	std::generate(arr, arr + rows * columns, std::rand);
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

void serial_findIndices(const uint* arr, size_t rows, size_t columns, std::vector<Pos>& positions)
{
	for (size_t i = 0; i < rows; ++i) {
		for (size_t j = 0; j < columns; ++j) {
			const uint* num = arr + i * columns + j;
			const int sum = sumOfDigits(*num);

			if (sum % 7 == 0)
				positions.push_back({ i, j });
		}
	}
}

void multithread_findIndices(const uint* arr, size_t rowCount, size_t columns,
        size_t startRowInd, std::mutex &mutex, std::vector<Pos>& indices) {
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

void printMatrix(const uint* arr, size_t rows, size_t columns)
{
	for (size_t i = 0; i < rows; ++i) {
		for (size_t j = 0; j < columns; ++j) {
			const uint* num = arr + i * columns + j;
			std::cout << *num << "\t";
		}

		std::cout << "\n";
	}
}

void printIndices(const uint* arr, size_t rows, size_t columns, const std::vector<Pos>& indices)
{
	for (auto pos : indices) {
		const uint* num = arr + pos.i * columns + pos.j;
		std::cout << pos.i << " " << pos.j << " " << *num << "\n";
	}
}

long long doWithTimer(std::function<void()> f)
{
	const auto startTime = std::chrono::high_resolution_clock::now();
	f();
	const auto endTime = std::chrono::high_resolution_clock::now();
	return std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
}

size_t getThreadCount(size_t prefThreadCount, size_t rowsCount)
{
	if (prefThreadCount < 0)
		return 1;

	if (rowsCount < MIN_BLOCK_SIZE)
		return 1;

	const size_t maxThreadCountForData = rowsCount / MIN_BLOCK_SIZE;
	const size_t maxThreadCount = std::min(maxThreadCountForData, MAX_THREAD_COUNT);
	return std::min(prefThreadCount, maxThreadCount);
}

void checkSerial(const uint* arr, size_t rows, size_t columns)
{
	std::vector<Pos> indices;

	const auto timeElapsed = doWithTimer([=, &indices]() {
		serial_findIndices(arr, rows, columns, indices);
	});

	/*
	std::cout << "First:\n";
	printIndices(arr, rows, columns, indices);
	*/

	std::cout << "Time elaped: " << timeElapsed << '\n';
}

void checkMultithread(const uint* arr, size_t rows, size_t columns, size_t prefThreadCount)
{
	std::vector<Pos> indices;
	const auto timeElapsed = doWithTimer([=, &indices]() {
		std::mutex mutex;
		std::vector<std::thread> threads;
		const size_t threadCount = getThreadCount(prefThreadCount, rows);
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

	std::cout << "Time elaped: " << timeElapsed << '\n';
}

int main(int argc, char* argv[])
{
	const size_t prefThreadCount = argc == 2 ? atoi(argv[1]) : MAX_THREAD_COUNT;
	std::srand(std::time(nullptr));
	uint* arr = new uint[ROWS * COLUMNS];
	fillArray(arr, ROWS, COLUMNS);

	//printMatrix(arr, ROWS, COLUMNS);

	checkSerial(arr, ROWS, COLUMNS);
	checkMultithread(arr, ROWS, COLUMNS, prefThreadCount);

	std::cout << std::endl;
#ifdef _WIN32
	system("pause");
#endif
	return 0;
}
