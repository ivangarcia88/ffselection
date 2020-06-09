#!/bin/bash
 
pref=ds 
# Iterate the string array using for loop
for val in {1..12}
do
   #echo $pref$val".csv"
   echo Running $pref$val".csv":
   #Running feature selection, generate dataset-output
   python MICSelect.py -i datasets-input/$pref$val".csv" -y target -t 16 -P -R -x 100
   #Transpose fs data to be used on machine learning
   python transposeCSV.py -i datasets-output/$pref$val".csv"
   #Transpose full data to be used on machine learning
   python transposeCSV.py -i datasets-input/$pref$val".csv" -o datasets-test/full/$pref$val".csv"
   #Copying fs data to evaluation directory
   cp datasets-output/$pref$val".csv" datasets-test/fs/$pref$val".csv"
   echo ===================
done
#python result.py