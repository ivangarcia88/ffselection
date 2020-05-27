import pymictools
import common
from operator import itemgetter

def transformListToDict(xList, keyColumn=0):
	newDict = {}
	for x in xList:
		key = x[keyColumn]
		x.remove(key)
		newDict[key] = x
	return newDict


def readCorrelation(correlationFilePath, targetName):
	results = []
	rawData = common.getRawData(correlationFilePath,separateHeaderFlag=0)
	for row in rawData:
		line = []
		elementNumber = 0
		for element in row:
			if(elementNumber>1):
				element = float(element)
			line.append(element)
			elementNumber+=1
		results.append(line)
	return results

def algorithmToString(correlationAlgorithms):
	stringToRun = ""
	if(correlationAlgorithms[0]):
		stringToRun += " -P"
	if(correlationAlgorithms[1]):
		stringToRun += " -R"
	if(correlationAlgorithms[2]):
		stringToRun += " -S"
	return stringToRun

def MICParamToString(MICParamsArray):
	a=0
	return ""

def splitColumns(matrix):
	c1 = []
	c2 = []
	for i in matrix:
		c1.append(i[0])
		c2.append(i[1])
	return c1,c2


def selectingTargetCorrelation(results,targetName,correlationAlgorithms):
	matrixResult = []
	featureColumn = 0
	correlationColumn = 0
	#Selecting feature columns and correlation Value
	#printMatrixResult(results)
	if(results[0][0]==targetName):
		featureColumn = 1
	if(correlationAlgorithms[2] and correlationAlgorithms[1]):
		correlationColumn = 1
	#Filling matrix result
	for result in results:
		if(correlationColumn==1):
			correlationValue = max(result[3],result[4])
		else:
			correlationValue = abs(result[2])
		feature = result[featureColumn]
		matrixResult.append([feature,correlationValue])
	
	matrixResult.sort(key=itemgetter(1), reverse=True)
	return matrixResult


def selectingPairCorrelation(results,correlationAlgorithms,cutValue,removeCorrelationFlag=0):
	matrixResult = []
	correlationColumn = 0
	if(correlationAlgorithms[2] or correlationAlgorithms[1]):
		correlationColumn = 1
	for result in results:
		if(correlationColumn==1):
			correlationValue = max(result[3],result[4])
		else:
			correlationValue = abs(result[2])
		if(correlationValue>=cutValue):
			feature1 = result[0]
			feature2 = result[1]
			matrixResult.append([feature1,feature2,correlationValue])
	
	#print matrixResult
	matrixResult.sort(key=itemgetter(2), reverse=True)
	if(len(matrixResult)>0):
		matrixResult = common.removeColumns(matrixResult,[2])
	return matrixResult


def printMatrixResult(results):
	print ("[var1, var2, pearson, rapidmic, sgmic]:")
	for result in results:
		print (result)

def writeFilterDataByNumber(inputFile, outputFile, features):
	rawData = common.getRawData(inputFile,separateHeaderFlag=0)
	newData = []
	for f in features:
		newData.append(rawData[f])
	#print newData
	common.writeCSV(newData,outputFile,1)

