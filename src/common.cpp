#include "common.h"

#include <algorithm>
#include <iostream>
#include <chrono>

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

long long doWithCppTimer(std::function<void()> f)
{
	const auto startTime = std::chrono::high_resolution_clock::now();
	f();
	const auto endTime = std::chrono::high_resolution_clock::now();
	return std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
}

size_t getThreadCount(size_t prefThreadCount, size_t rowsCount, size_t maxCount)
{
	if (prefThreadCount < 0)
		return 1;

	if (rowsCount < MIN_BLOCK_SIZE)
		return 1;

	const size_t maxThreadCountForData = rowsCount / MIN_BLOCK_SIZE;
	const size_t maxThreadCount = std::min(maxThreadCountForData, maxCount);
	return std::min(prefThreadCount, maxThreadCount);
}

