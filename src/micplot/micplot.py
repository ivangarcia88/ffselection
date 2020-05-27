"""
--- Oracle MICPlot ---
MICPlot produces a scatterplot of a pair of variables from a CSV file.

Usage: python micplot.py [csv file] [mode] [var1 row] [var2 row]

Mode can be either:
  number - To select a variable by it's row number
  label  - To select a variable by it's row label
"""

import sys
import csv
import matplotlib.pyplot as plt


def getRowsByName(path, var1, var2):
    var1_list = []
    var2_list = []
    
    try:
        with open(path, 'r') as csv_file:
            csv_reader = csv.reader(csv_file, delimiter=',')

            for row in csv_reader:
                if var1 == row[0]:
                    var1_list = row[1:]
                if var2 == row[0]:
                    var2_list = row[1:]
                if var1_list and var2_list:
                    break;
    except:
        print '\n  Error: File %s not found!...\n' % path
        exit()

    return var1_list, var2_list, var1, var2


def getRowsByNum(path, var1, var2):
    var1 = int(var1)
    var2 = int(var2)
    current_row = 0
    var1_list = []
    var2_list = []
    var1_name = ''
    var2_name = ''
    
    try:
        with open(path, 'r') as csv_file:
            csv_reader = csv.reader(csv_file, delimiter=',')

            for row in csv_reader:
                if var1 == current_row:
                    var1_name = row[0]
                    var1_list = row[1:]
                if var2 == current_row:
                    var2_name = row[0]
                    var2_list = row[1:]
                if var1_list and var2_list:
                    break;
                current_row += 1
    except:
        print '\n  Error: File %s not found!...\n' % path
        exit()

    return var1_list, var2_list, var1_name, var2_name


def plot(var1_list, var2_list, var1_name, var2_name):
    plt.figure(facecolor='white')
    plt.scatter(var1_list, var2_list, s=15, c="blue", alpha=0.5)
    plt.xlabel(var1_name)
    plt.ylabel(var2_name)
    plt.title('PAIR PLOT: %s, %s' % (var1_name, var2_name))
    plt.show()


def checkPlotConfig(var1_list, var2_list):
    success = True
    if len(var1_list) == 0:
        print "  Error: Variable 1 could not be found!"
        success = False
    if len(var2_list) == 0:
        print "  Error: Variable 2 could not be found!"
        success = False
    if len(var1_list) != len(var2_list) and success:
        print "  Error: Variables 1 and 2 are not of the same size!"
        success = False
    return success

if __name__ == '__main__':
    if len(sys.argv) == 5:
        path = sys.argv[1]  # Input file path
        mode = sys.argv[2]  # Mode (label, numeric)
        var1 = sys.argv[3]  # Var 1
        var2 = sys.argv[4]  # Var 2
    else:
        print __doc__
        exit()
    
    if mode  == "label":
        var1_list, var2_list, var1_name, var2_name = getRowsByName(path, var1, var2)
    elif mode == "number":
        var1_list, var2_list, var1_name, var2_name = getRowsByNum(path, var1, var2)
    else:
        print "Unknown mode '%s'...\n" % mode

    if checkPlotConfig(var1_list, var2_list):
        plot(var1_list, var2_list, var1_name, var2_name)
