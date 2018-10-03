#include "common.h"

#include <cstdlib>
#include <ctime>
#include <iostream>

void serial_findIndices(const uint* arr, size_t rowsCoutn, size_t columnsCount, std::vector<Pos>& positions)
{
	for (size_t i = 0; i < rowsCoutn; ++i) {
		for (size_t j = 0; j < columnsCount; ++j) {
			const uint* num = arr + i * columnsCount + j;
			const int sum = sumOfDigits(*num);

			if (sum % 7 == 0)
				positions.push_back({ i, j });
		}
	}
}

int main(int argc, char* argv[])
{
	std::srand(std::time(nullptr));
	uint* arr = new uint[ROWS * COLUMNS];
	fillArray(arr, ROWS, COLUMNS);

	std::vector<Pos> indices;
	const auto timeElapsed = doWithCppTimer([=, &indices]() {
		serial_findIndices(arr, ROWS, COLUMNS, indices);
	});

	if (PRINT_RESULT) {
		std::cout << "First:\n";
		printIndices(arr, ROWS, COLUMNS, indices);
	}

	std::cout << "Time elaped: " << timeElapsed << '\n';

	std::cout << std::endl;
#ifdef _WIN32
	system("pause");
#endif
	return 0;
}
