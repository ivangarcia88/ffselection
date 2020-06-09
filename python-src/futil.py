import sys
import os
import pandas as pd

#Function to finish execution (normal or catched error)
def finish(option=True):
	if(option):
		try:
			#removeFolder("tmp")
			sys.exit()
		except:
			print("Error finishing execution")

#Creating folder
def createFolder(path,terminate=True):
	try:
		if(not(os.path.exists(path))):
			os.mkdir(path)
	except:
		print("Error creating folder",path)
		finish(terminate)

#Remove folder
def removeFolder(path):
	try:
		if(not(os.path.exists(path))):
			os.rmdir(path)
	except:
		print("Error removing folder",path)
			
#Creating and writing file content
def createFile(filepath,content,terminate=True):
	try:
		f = open(filepath, "w")
		f.write(content)
		f.close()
	except:
		print("File",filepath, "could not be created")
		finish(terminate)

#Get file name
def getFileName(path):
	head, tail = os.path.split(path)
	return tail

#Transpose dataset
def transposeDataset(inputPath, outPath):
    df = pd.read_csv(inputPath, header=None)
    df2 = df.T
    df2.to_csv(outPath, header=None, index=False)