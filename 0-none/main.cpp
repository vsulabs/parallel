#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <functional>
#include <iostream>
#include <vector>

using uint = unsigned int;

constexpr size_t ROWS = 100000;
constexpr size_t COLUMNS = 1000;
constexpr size_t MAX_THREAD_COUNT = 4;
constexpr size_t MIN_BLOCK_SIZE = 100;
constexpr bool PRINT_RESULT = false;


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

void checkSerial(const uint* arr, size_t rows, size_t columns)
{
	std::vector<Pos> indices;

	const auto timeElapsed = doWithTimer([=, &indices]() {
		serial_findIndices(arr, rows, columns, indices);
	});

	if (PRINT_RESULT) {
		std::cout << "First:\n";
		printIndices(arr, rows, columns, indices);
	}

	std::cout << "Time elaped: " << timeElapsed << '\n';
}

int main(int argc, char* argv[])
{
	const size_t prefThreadCount = argc == 2 ? atoi(argv[1]) : MAX_THREAD_COUNT;
	std::srand(std::time(nullptr));
	uint* arr = new uint[ROWS * COLUMNS];
	fillArray(arr, ROWS, COLUMNS);

	checkSerial(arr, ROWS, COLUMNS);

	std::cout << std::endl;
#ifdef _WIN32
	system("pause");
#endif
	return 0;
}
