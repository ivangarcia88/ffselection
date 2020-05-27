"""
This module serves as an interface to simplify the use of mictools.so.
"""
import sys
sys.path.append('/mic/')
sys.path.append('../mic/')
import mictools


# Computes the given mictools command using mictools.so,
# then parses the resulting list into a more convenient
# list of lists structure following the next format:
#
# [[result1], [result2], ... ,[resultN]]
#
# where every [result] follows is defined as follows:
#
# result [var1, var2, pearson, rapidmic, sgmic]
def Run(command):
    results = []
    try:
        argv = command.split()
        argc = len(argv)

        raw_results = mictools.Run(argc, argv)
        raw_results.reverse()

        while raw_results:
            result = []  # single result placeholder

            result.append(raw_results.pop())  # var 1
            result.append(raw_results.pop())  # var 2
            result.append(raw_results.pop())  # pearson
            result.append(raw_results.pop())  # rapidmic
            result.append(raw_results.pop())  # sgmic

            results.append(result)  # append result to results list

    except Exception as e:
        print ("Invalid command or error in mictools execution!")

    return results
