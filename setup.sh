#Script to install requirements and compile mictools
#This scripted was tested on ubuntu 18.04
sudo apt-get install build-essential;
sudo apt-get install g++;
sudo apt-get install python-numpy;
sudo apt-get install libboost-python-dev;
sudo apt-get install python-dev;
sudo apt-get install python-sklearn;
sudo apt get install python-pandas;
sudo apt get install python-matplotlib;
make;
make wrapper;