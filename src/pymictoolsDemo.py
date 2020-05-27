"""
Usage example of  pymictools
"""
import pymictools

# Set a valid mictools command
command = "build/mictools -i ../datasets/Spellman_tiny.csv -P -t 4"

# Run the command using pymictools and retrieve the computed results
results = pymictools.Run(command)

# Print the results
print "Command results (format [var1, var2, pearson, parallelmic, samic]):"
for result in results:
    print result
