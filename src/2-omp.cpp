#include "common.h"

#include <cstdlib>
#include <ctime>
#include <iostream>

#include <omp.h>


void checkMultithread(const uint* arr, size_t rowCount, size_t columnsCount, size_t prefThreadCount)
{
	std::vector<Pos> indices;

	omp_set_num_threads(getThreadCount(prefThreadCount, rowCount, MAX_THREAD_COUNT));
	const auto startTime = omp_get_wtime();
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

#pragma omp critical
		{
			indices.insert(indices.cend(), positions.cbegin(), positions.cend());
		}
	}
	const auto endTime = omp_get_wtime( );
	const auto timeElapsed = endTime - startTime;

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
