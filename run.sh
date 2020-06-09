#!/bin/bash
 
pref=ds;
rm -rf datasets-test;
mkdir datasets-test;
mkdir datasets-test/fss;
mkdir datasets-test/fsx;

# Iterate the string array using for loop
for val in {1..12}
do
   #echo $pref$val".csv"
   echo Running $pref$val".csv";
   #Running feature selection, with static cut 10
   python MICSelect.py -i datasets-input/$pref$val".csv" -y target -s 5 -t 16 -P -R;
   cp datasets-output/$pref$val".csv" datasets-test/fss/$pref$val".csv";
   #Running feature selection, with step wise 
   python MICSelect.py -i datasets-input/$pref$val".csv" -y target -x 20 -t 16 -P -R;
   cp datasets-output/$pref$val".csv" datasets-test/fsx/$pref$val".csv";
   echo ----------------------------------------------------------------------------;
done
python result.py