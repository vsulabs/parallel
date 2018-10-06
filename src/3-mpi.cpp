#include "common.h"

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <mutex>

#include <mpi.h>

#include <stdint.h>
#include <limits.h>

#if SIZE_MAX == ULONG_MAX
# define MPI_SIZE_T MPI_UNSIGNED_LONG
#else
# error "Can't detect size_t size"
#endif

enum Tags
{
	tRowCount,
	tColumnCount,
	tData,
	tResultSize,
	tResult
};

constexpr int MASTER_ID = 0;

int main(int argc, char* argv[])
{
	const size_t prefThreadCount = argc == 2 ? atoi(argv[1]) : MAX_THREAD_COUNT;
	std::srand(std::time(nullptr));
	uint* array;
	uint* arrayPtr;

	if (MPI_Init(&argc, &argv) != 0) {
		std::cerr << "Can't init MPI";
		return 1;
	}

	int procNum;
	int currentId;
	MPI_Comm_rank(MPI_COMM_WORLD, &currentId);
	MPI_Comm_size(MPI_COMM_WORLD, &procNum);

	std::cerr << "Proc num:" << procNum << std::endl;
	std::cerr << "Proc id:" << currentId << std::endl;
	size_t columnCount = COLUMNS;
	const size_t rowCount = ROWS;
	const size_t threadCount = getThreadCount(prefThreadCount, rowCount, procNum);
	size_t rowsPerThread = rowCount / threadCount;

	double startwtime = 0.0;
	if (currentId == MASTER_ID) {
		array = new uint[columnCount * rowCount];
		arrayPtr = array;
		fillArray(array, columnCount, rowCount);
		startwtime = MPI_Wtime();

		for (size_t i = 1; i < threadCount; ++i) {
			const size_t size = columnCount * rowsPerThread;
			MPI_Send(&rowsPerThread, 1,    MPI_SIZE_T, i, tRowCount,    MPI_COMM_WORLD);
			MPI_Send(&columnCount,   1,    MPI_SIZE_T, i, tColumnCount, MPI_COMM_WORLD);
			MPI_Send(arrayPtr,       size, MPI_INT,    i, tData,        MPI_COMM_WORLD);
			arrayPtr += size;
		}

		rowsPerThread = rowCount - (arrayPtr - array) / columnCount;
	} else {
		MPI_Recv(&rowsPerThread, 1, MPI_SIZE_T, MASTER_ID, tRowCount,    MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		MPI_Recv(&columnCount,   1, MPI_SIZE_T, MASTER_ID, tColumnCount, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		const size_t size = columnCount * rowsPerThread;
		arrayPtr = new uint[rowsPerThread * columnCount];
		MPI_Recv(arrayPtr, size, MPI_INT, MASTER_ID, tData, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	}

	std::vector<Pos> positions;
	for (size_t row = 0; row < rowsPerThread; ++row) {
		for (size_t col = 0; col < columnCount; ++col) {
			const uint* num = arrayPtr + row * columnCount + col;
			const int sum = sumOfDigits(*num);

			if (sum % 7 == 0)
				positions.push_back({ row, col });
		}
	}

	if (currentId == MASTER_ID) {
		size_t doneCount = 0;
		while (doneCount < procNum - 1) {
			size_t size;
			MPI_Status status;
			MPI_Recv(&size, 1, MPI_SIZE_T, MPI_ANY_SOURCE, tResultSize, MPI_COMM_WORLD, &status);
			const size_t lastIndex = std::max<size_t>(0, positions.size() - 1);
			positions.resize(positions.size() + size);
			char* lastPos = reinterpret_cast<char*>(positions.data() + lastIndex);
			MPI_Recv(lastPos, size * sizeof(Pos), MPI_BYTE, status.MPI_SOURCE, tResult, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			doneCount++;
		}

		std::cerr << "Time: " << MPI_Wtime() - startwtime << "s\n";
		delete array;
	} else {
		const size_t size = positions.size();
		const size_t send_size = size * sizeof(Pos);
		MPI_Send(&size, 1, MPI_SIZE_T, MASTER_ID, tResultSize, MPI_COMM_WORLD);
		const char* data = reinterpret_cast<char*>(positions.data());
		MPI_Send(data, send_size, MPI_BYTE, MASTER_ID, tResult, MPI_COMM_WORLD);
		delete arrayPtr;
	}

	MPI_Finalize();
#ifdef _WIN32
	system("pause");
#endif
	return 0;
}
