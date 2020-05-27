import random
import math
#import matplotlib.pyplot as mpl
import learningParallelized as learnP
import numpy as np
from numpy import genfromtxt

globalValue = 0
globalPosition = 0
globalTotalLen = 0

def prepareData(dataset, cols, targetRow, transpose=0, normalize=1):
    target = dataset[targetRow]
    for i in range(0,len(target)):
        if(target[i]<=0):
            target[i] = 0
        else:
             target[i] = 1
    #patch remove target
    #dataset = dataset[1:]
    #end patch
    data = dataset[cols]
    if(transpose):
        data = data.transpose()
    
    if(normalize):
        row_sums = data.max(axis=1)
        data = data / row_sums[:, np.newaxis]
        #dataNormalized = dataNormalized.transpose()*10
    return [data,target]
def separeDataAndTrainning(data, target, indices, samples, testpercentage):
    samplesper = int((-samples*testpercentage))
    dataTrain = data[indices[:samplesper]]
    targetTrain = target[indices[:samplesper]]
    dataTest  = data[indices[samplesper:]]
    targetTest  = target[indices[samplesper:]]
    return [dataTrain,targetTrain,dataTest,targetTest]
def processRawDataToClassify (samples, testpercentage, rawData, cols, targetRow):
    np.random.seed(0)
    indices = np.random.permutation(samples)
    [data,target] = prepareData(rawData, cols, targetRow , 1, 0) #max 0 32
    [dataTrain,targetTrain,dataTest,targetTest] = separeDataAndTrainning (data, target, indices, samples, testpercentage)
    return [dataTrain,targetTrain,dataTest,targetTest]
def validateFeatures(rawData,samples,correlationList,cutPosition,classifierListToExecute=[0,0,1,0,1,0,1,0,0,0,0,0]):
    cols = correlationList[0:cutPosition+1]
    dataTrain,targetTrain,dataTest,targetTest = processRawDataToClassify(samples, 0.5, rawData, cols, 0)
    return sum(learnP.executeClassifiersParallelized(dataTrain,targetTrain,dataTest,targetTest, classifierListToExecute,0))
def maxDistanceBetweenPoints(xlist, first=0):
    maxDistance = 0
    position = 0
    last = len(xlist)
    for i in range(first+1,last):
        lastX = xlist[i-1]
        currentX = xlist[i]
        difX = lastX-currentX
        #print "lx",lastX
        #print "cx",currentX
        #print "dx",difX
        if(difX > maxDistance):
            maxDistance = difX
            position = i             
    return round(maxDistance,3), position
def cutTopFeatures(targetCorrelation, cutNumber):
    features = []
    for f in targetCorrelation:
        features.append(f[0])
    return features[0:cutNumber]
def cutDistanceFeatures(correlatedValues,correlatedFeatures,cutPerc=0.05,verbose=0):
    sumL=sum(correlatedValues)
    minCuteAllowed=sumL*cutPerc
    md=maxDistanceBetweenPoints(correlatedValues)
    positionDistance=0
    for i in range(0,1000):
        md,position=maxDistanceBetweenPoints(correlatedValues,positionDistance)
        currentCut=sum(correlatedValues[0:position])
        if(currentCut >= minCuteAllowed ):
            break
        else:
            positionDistance=position    
    if(verbose):
        print ("Total",sumL)
        print ("Position",position)
        print ("minCuteAllowed",minCuteAllowed)
        print ("currentCut",currentCut)
    return correlatedFeatures[0:position+1]

def cutDistanceFeaturesCrossV(rawData,samples,correlatedValues,correlatedFeatures,maxCuts=20,classifierListToExecute=[0,0,1,0,1,0,1,0,0,0,0,0],verbose=0):
    maxValue = 0
    maxFeatures = []
    lastCut = 0
    allcurrentFeatures = []
    for i in range(0,maxCuts):
        #features = cutDistanceFeatures(correlatedValues[lastCut:],correlatedFeatures[lastCut:],0) # is used with verbose (instead of the line below) to validate how value is decreasing when you extract important features to MIC-Feature-Selection-Algorithm
        features = cutDistanceFeatures(correlatedValues[lastCut:],correlatedFeatures[lastCut:],0)
        for f in features:
            allcurrentFeatures.append(f)
        lastCut = len(allcurrentFeatures)
        valValue = validateFeatures(rawData,samples,allcurrentFeatures,len(allcurrentFeatures)+1,classifierListToExecute)
        #verbose=1
        if(verbose):
            print (str(lastCut)+"/"+str(len(correlatedValues))+","+"["+str(valValue)+" vs "+str(maxValue)+"]",len(maxFeatures))#,":",allcurrentFeatures        
        if(valValue > maxValue):
            maxValue = valValue
            maxFeatures = []
            for x in allcurrentFeatures:
                maxFeatures.append(x)
        if(lastCut>=len(correlatedValues)):
            return maxFeatures
    return maxFeatures
def optimalCutV(rawData,samples,correlationList,consecutiveDecreaseCut=20,classifierListToExecute=[0,0,1,0,1,0,1,0,0,0,0,0],verbose=0): #only combined method
    #rawData = genfromtxt(datasetPath, delimiter=',', skip_header=0, usecols=range(1,samples+1))
    maxResult = 0
    maxPosition = 0
    result = 0
    decreaseCount = 0
    for i in range(0,len(correlationList)):
        result = validateFeatures(rawData,samples,correlationList,i+1,classifierListToExecute)
        if(maxResult>result):
            decreaseCount += 1
            if(decreaseCount>=consecutiveDecreaseCut): #if X consecutive decrease, then cut
                return correlationList[0:maxPosition+1]
        else:
            decreaseCount = 0
        
        if(result > maxResult):
            maxResult = result
            maxPosition = i
        if(verbose):
            print (result,"vs",maxResult)
            print (maxPosition)
    return correlationList[0:maxPosition+1]
def cutBinaryV (rawData,samples,correlationList,classifierListToExecute=[0,0,1,0,1,0,1,0,0,0,0,0],verbose=0): #only combined method
    global globalValue
    global globalPosition
    global globalTotalLen
    #rawData = genfromtxt(datasetPath, delimiter=',', skip_header=0, usecols=range(1,samples+1))
    globalValue = 0
    globalPosition = 0    
    globalTotalLen = len(correlationList)
    mid = int(round(globalTotalLen/2))
    midValue = validateFeatures(rawData,samples,correlationList,mid+1,classifierListToExecute)
    makeDecision(rawData,samples,correlationList,classifierListToExecute,mid,midValue,0)
    makeDecision(rawData,samples,correlationList,classifierListToExecute,mid,midValue,globalTotalLen-1)
    return correlationList[0:globalPosition+1]
    #return globalPosition,globalValue
def makeDecision(rawData,samples,correlationList,classifierListToExecute,supPosition,supValue,infPosition): #falta depurar
    #Features
    infValue = validateFeatures(rawData,samples,correlationList,infPosition+1,classifierListToExecute)

    #Evaluation to return
    global globalPosition
    global globalValue
    if(supValue>infValue):
        if(supValue> globalValue):
            globalValue = supValue
            globalPosition = supPosition
    else:
        if(infValue> globalValue):
            globalValue = infValue
            globalPosition = infPosition
    #print "current", supPosition,infPosition
    #print "values:",supValue, infValue
    if(abs(supPosition-infPosition)<=1):
        return 1

    #Make decision
    if(supValue>infValue): #significa que el limite inferior debe ser recorrido
        newInfPosition = int(round((infPosition+supPosition)/2))
        makeDecision(rawData,samples,correlationList,classifierListToExecute,supPosition,supValue,newInfPosition)
    else: #significa que el limite superior debe ser recorrido
        newSupPosition = int(round((supPosition+infPosition)/2)) 
        #cols = correlationList[0:newSupPosition+1]
        #dataTrain,targetTrain,dataTest,targetTest = processRawDataToClassify(samples, 0.5, rawData, cols, 0)
        #supValue = sum(learnP.executeClassifiersParallelized(dataTrain,targetTrain,dataTest,targetTest, [0,0,1,1,1,0,1,1,0,0,1,1],0))
        supValue = validateFeatures(rawData,samples,correlationList,newSupPosition+1,classifierListToExecute)
        makeDecision(rawData,samples,correlationList,classifierListToExecute,newSupPosition,supValue,infPosition)

'''
#testing
samples = 1000
rawData = genfromtxt('datasets/f1-fe.csv', delimiter=',', skip_header=0, usecols=range(1,samples+1))
optimalFeatures = [14,13,0,1,2,3,4,5,6,11,12,7,8,9,10]
correlatedFeatures = [14,2,12,0,11,13,6,1,3,4,5,7,8,9,10]
correlatedValues = [0.55,0.205,0.202,0.197,0.197,0.192,0.158,0.123,0.115,0.109,0.103,0.094,0.068,0.058,0.055]
print "Global Optimal:         ", optimalCutV(rawData,samples,optimalFeatures,classifierListToExecute=[0,0,1,0,1,0,1,0,0,0,0,0],verbose=0)
print "Correlation optimal cut:", optimalCutV(rawData,samples,correlatedFeatures,classifierListToExecute=[0,0,1,0,1,0,1,0,0,0,0,0],verbose=0)
print "Binary cut:             ", cutBinaryV(rawData,samples,correlatedFeatures,classifierListToExecute=[0,0,1,0,1,0,1,0,0,0,0,0],verbose=0)
print "Fast cut:               ", cutDistanceFeatures(correlatedValues,correlatedFeatures,0.3)
print "Fast cut with Cross-V:  ", cutDistanceFeaturesCrossV(rawData,samples,correlatedValues,correlatedFeatures,[0, 0.1, 0.2, 0.33, 0.5, 0.75])
#'''