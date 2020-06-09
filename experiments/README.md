***How to reproduce the results of the publication:***

-This experiments runs on python 3.6 or greater

1. Compile mictools
2. Create the folder "datasets-results" in the root folder of this proyect
3. Create the folder "s10" inside the folder "datasets-results"
4. Create the folder "x20" inside the folder "datasets-results"
5. Run MICSelect with every dataset inside ("datasets-input") with the parameters (-y target -s 10)
6. Move every output ("datasets-output") to folder "datasets-results/s10"
7. Run MICSelect with every dataset inside ("datasets-input") with the parameters (-y target -x 20)
8. Move every output ("datasets-output") to folder "datasets-results/s10"
9. Runs result.py

-Alternative after compiling mictools the script run.sh could be run (tested on linux ubuntu 18.04 with python 3.6)