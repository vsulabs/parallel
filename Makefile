all: bin bin/0-none bin/1-mutex bin/2-omp bin/3-mpi bin/barber

bin/common.o: src/common.cpp
	g++ -c src/common.cpp -Iinclude -o bin/common.o

bin/0-none: bin/common.o src/0-none.cpp
	g++ src/0-none.cpp bin/common.o -Iinclude -o bin/0-none

bin/1-mutex: bin/common.o src/1-mutex.cpp
	g++ src/1-mutex.cpp bin/common.o -Iinclude -o bin/1-mutex -lpthread

bin/2-omp: bin/common.o src/2-omp.cpp
	g++ src/2-omp.cpp bin/common.o -Iinclude -o bin/2-omp -fopenmp

bin/3-mpi: bin/common.o src/3-mpi.cpp
	mpic++ src/3-mpi.cpp bin/common.o -Iinclude -o bin/3-mpi

bin/semaphore.o: src/Semaphore.cpp
	g++ -c src/Semaphore.cpp -Iinclude -o bin/semaphore.o

bin/semaphore: bin/semaphore.o src/semaphore_test.cpp
	g++ bin/semaphore.o src/semaphore_test.cpp -Iinclude -lpthread -o bin/semaphore

bin/barber: bin/semaphore.o src/barber.cpp
	g++ bin/semaphore.o src/barber.cpp -lrt -lpthread -Iinclude -o bin/barber

run-mpi: bin/3-mpi
	mpirun --hostfile hostfile -np `nproc` bin/3-mpi

bin:
	mkdir bin

clean:
	rm -rf bin
