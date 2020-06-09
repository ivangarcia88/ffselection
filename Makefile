all:
	#mkdir -p correlation-output datasets-output mic 
	mkdir -p correlation-output datasets-output mic tmp 	
	g++ src/main.cpp src/mictools.cpp src/pearson.cpp src/parallelmic.cpp src/samic.cpp src/grid.cpp src/approxmaxmi.cpp src/quicksort.cpp -o mic/mictools -pthread -O3
	ln -sd mic/mictools mictools
debug:
	mkdir -p debug
	g++ -g src/main.cpp src/mictools.cpp src/pearson.cpp src/parallelmic.cpp src/samic.cpp src/grid.cpp src/approxmaxmi.cpp src/quicksort.cpp -o debug/mictools -pthread

wrapper:
	#mkdir -p correlation-output datasets-output mic 
	cp src/wrapper/* src/
	cd src/; python setup.py build
	mv src/build/lib.*/* mic/mictools.so
	rm src/pymictools.cpp
	rm src/setup.py
	rm -rf src/build

clean:
	rm -rf correlation-output datasets-output mic debug mictools tmp
