***Feature selection tools for supervised learning***

-To install requirements and compile in a debian based platform execute the script "setup.sh"

-To install in other linux distribution install the following packages:
	-g++
	-libboost-python-dev
	-python-dev
	-python-numpy
	-python-pandas
	-python-sklearn
	-python-matplotlib

-Then execute in the terminal:
	-make
	-make wrapper

***How to reproduce the results***

-This experiments runs on python 3.6 or greater

1. Compile mictools
2. Create the folder "datasets-test" in the root folder of this proyect
3. Create the folder "s10" inside the folder "datasets-test"
4. Create the folder "x20" inside the folder "datasets-test"
5. Run MICSelect with every dataset inside ("datasets-input") with the parameters (-y target -s 10)
6. Move every output ("datasets-output") to folder "datasets-test/s10"
7. Run MICSelect with every dataset inside ("datasets-input") with the parameters (-y target -x 20)
8. Move every output ("datasets-output") to folder "datasets-test/x20"
9. Runs result.py

-Alternative after compiling mictools, run the script "run.sh" (tested on linux ubuntu 18.04 with python 3.6)