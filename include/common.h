#ifndef _COMMON_H_
#define _COMMON_H_

#include <functional>
#include <vector>

using uint = unsigned int;

constexpr size_t ROWS = 100000;
constexpr size_t COLUMNS = 1000;
constexpr size_t MAX_THREAD_COUNT = 4;
constexpr size_t MIN_BLOCK_SIZE = 100;
constexpr bool PRINT_RESULT = false;

struct Pos
{
	size_t i;
	size_t j;
};

void fillArray(uint* arr, size_t rows, size_t columns);
uint sumOfDigits(uint num);

long long doWithCppTimer(std::function<void()> f);
size_t getThreadCount(size_t prefThreadCount, size_t rowsCount);

void printMatrix(const uint* arr, size_t rows, size_t columns);
void printIndices(const uint* arr, size_t rows, size_t columns, const std::vector<Pos>& indices);

#endif // _COMMON_H_
