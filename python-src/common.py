import csv
import sys
import collections
import numpy as np
import common as com

def getHeaders(infile, hasTarget=1, targetRow=0):
	#fast patch
	if(hasTarget==0):
		targetRow = 10000000
	#end fast patch
	headerByNumber = {}
	headerByName = {}
	counter = 0
	currentRow = 0
	with open(infile) as f:
		reader = csv.reader(f)
		for row in reader:
			if(currentRow != targetRow):
				#headerByNumber.append(row[0])
				headerByNumber[counter] = row[0]
				headerByName[row[0]] = counter
				counter = counter+1
			currentRow = currentRow+1
		return headerByNumber, headerByName
def getHeadersData(rawData, hasTarget=1, targetRow=0):
	#fast patch
	if(hasTarget==0):
		targetRow = 10000000
	#end fast patch
	headerByNumber = {}
	headerByName = {}
	counter = 0
	currentRow = 0
	for row in rawData:
		if(currentRow != targetRow):
			#headerByNumber.append(row[0])
			headerByNumber[counter] = row[0]
			headerByName[row[0]] = counter
			counter = counter+1
		currentRow = currentRow+1
	return headerByNumber, headerByName

def writeCSV(rawData,filepath,addcommaFlag=0):
	myfile = open(filepath,'w')
	for row in rawData:
		stringRow=''
		for c in row:
			if(addcommaFlag):
				stringRow+=str(c)+","
			else:
				stringRow+=str(c)
		stringRow = stringRow.translate(None,"\'")
		stringRow = stringRow.translate(None,"]")
		stringRow = stringRow.translate(None,"[")
		stringRow = stringRow.translate(None," ")
		if(addcommaFlag): #removing extra comma
			stringRow=stringRow[:-1]
		myfile.write(stringRow+"\n")
	myfile.close()	
def getRawData(infile,separateHeaderFlag=1):
	rawData = []
	headers = []
	with open(infile) as f:
		reader = csv.reader(f)
		for row in reader:
			if(separateHeaderFlag):
				rawData.append(row[1:])
				headers.append(row[0])
			else:
				rawData.append(row)
	if(separateHeaderFlag):
		return headers,rawData
	else:
		return rawData
def getRowData(rawData, rowNumber, deleteHeaderFlag=0):
	return rawData[rowNumber][deleteHeaderFlag:]	
def getColumnData(rawData, columnNumber,deleteHeaderFlag=0):
		data = []
		for row in rawData:
			data.append(row[columnNumber+deleteHeaderFlag])
		return data	
def getRowFile(infile, rowNumber, deleteNameFlag=0):
	rowCounter = 0
	with open(infile) as f:
		reader = csv.reader(f)
		for row in reader:
			if(rowCounter==rowNumber):
				data = row[deleteNameFlag:]
				break
			rowCounter = rowCounter+1
	return data	
def getColumnFile(infile, columnNumber,deleteNameFlag=0):
	with open(infile) as f:
		reader = csv.reader(f)
		data = []
		for row in reader:
			data.append(row[columnNumber+deleteNameFlag])
		return data	
def randomizeList(listData):
	np.random.seed(0)
	newList = []
	indices = np.random.permutation(len(listData))
	#print indices
	for i in indices:
		newList.append(listData[i])
	return newList

def transposeData(rawData):
	return map(list, zip(*rawData))

def removeColumns(matrix,columns):
	newMatrix = []
	listAll = range(0,len(matrix[0]))
	listNo = columns
	listYes = list(set(listAll) - set(listNo))
	for row in matrix:
		tempList = []
		for element in listYes:
			tempList.append(row[element])
		newMatrix.append(tempList)
	return newMatrix

