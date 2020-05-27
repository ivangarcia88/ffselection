import sys
sys.path.insert(0,'python-src/')
sys.path.insert(0,'mic/')
import featureSelectors as fs
import learningParallelized as lp
import parser as parser

optionMap = {'-i': 0,'-y': 1,'-w': 2,'-z': 3,'-a': 4,'-h': 5,'-t': 6,'-u': 7,'-p': 8,'-n': 9,'-c': 10,'-m': 11,'-j': 12,'-k': 13,'-e': 14,'-x': 15, '-s': 16, '-r': 17}

def run(argv):
	
	if(len(argv)<2):
		parser.help()
		return 1 # no parameters recived

	#default values
	cutMethod = 2
	cutAmmount = 0
	removeRedundantFlag=0
	writeCorrelationResults=0
	useCorrelationFile=0
	correlationAlgorithms=[1,1,0] #Pearson, ApproxMaxMI, No SMIC

	result = parser.run(argv)
	#print result	
	if(result[0]!=False): #If there is no error in parser 
		
		optionListSorted, paramListSorted, MICStringParameters, correlationAlgorithms = result
		#Mandatory Parameters
		datasetPath = paramListSorted[optionMap['-i']]
		targetName = paramListSorted[optionMap['-y']]
		
		# Select Cut Type
		if(optionListSorted[optionMap['-s']]): #static method
			cutMethod = 0
			cutAmmount = int(paramListSorted[optionMap['-s']])
		else: #dynamic method
			if(optionListSorted[optionMap['-x']]):
				cutMethod = int(paramListSorted[optionMap['-x']])
				cutAmmount = 0

		
		#Check for feature selector flags
		if(optionListSorted[optionMap['-r']]): #remove redundant flag
			removeRedundantFlag=1
		else:
			removeRedundantFlag=0
		if(optionListSorted[optionMap['-w']]): #write correlation results
			writeCorrelationResults=1
		else:
			writeCorrelationResults=0
		if(optionListSorted[optionMap['-z']]): #use previous correlation results
			writeCorrelationResults=0 #z prevents to rewrite the file
			useCorrelationFile=1
		else:
			useCorrelationFile=0
	
		relevantFeatures = fs.selectingRelevantFeatures(datasetPath, targetName, cutMethod, cutAmmount, MICStringParameters, correlationAlgorithms, removeRedundantFlag, writeCorrelationResults, useCorrelationFile, 0)
		if(relevantFeatures!=False):
			print (len(relevantFeatures[0]))
			print (relevantFeatures[0])
		else:
			return 3 #error in feature selection
	else:
		print (result)
		return 2 #error in parser

if __name__ == "__main__":
	run(sys.argv)
