import common
import pymictools
import featureSelectorUtils as fsu
import featureCutMethods as fc
from operator import itemgetter
from numpy import genfromtxt

rawData=headerByNumber=headerByName=samples=0
outputDatasetPath = 'datasets-output/'
outputCorrelationPath = 'correlation-output/'
#Feature Selection Methods (Selecting relevant features)
def selectingRelevantFeatures(datasetPath, targetName, cutMethod, cutAmmount, MICStringParameters, correlationAlgorithms, removeRedundantFlag, writeCorrelationResults, useFile,verbose=0):
    #Getting Dataset Information
    gettingDataInformation(datasetPath)
    correlationAlgorithmsString = fsu.algorithmToString(correlationAlgorithms)
    
    if(useFile!=1):
    #Running MIC 
        MICCommand = "mictools -i "+datasetPath+" "+MICStringParameters+correlationAlgorithmsString
        MICCommand +=" -g "+targetName
        print (MICCommand)
        relevantF = pymictools.Run(MICCommand)
        targetCorrelation = fsu.selectingTargetCorrelation(relevantF,targetName,correlationAlgorithms) # choose target-feature correlation and order the list descending
    
        if(writeCorrelationResults==1):
            common.writeCSV(relevantF,outputCorrelationPath+"relevance-"+datasetPath.split("/")[-1],addcommaFlag=1)
    else: #Readomg Correlation File (option -z)
        try:
            relevantF = fsu.readCorrelation((outputCorrelationPath)+"relevance-"+(datasetPath.split("/")[-1]), targetName)
        except:
            print ((outputCorrelationPath)+"relevance-"+(datasetPath.split("/")[-1])+" does not exist")
            return False
        #print relevantF
        targetCorrelation = fsu.selectingTargetCorrelation(relevantF,targetName,correlationAlgorithms) # choose target-feature correlation and order the list descending
    
    #Selecting relevant features
    targetCorrelationNumber = []
    for t in targetCorrelation:
        targetCorrelationNumber.append([headerByName[t[0]],t[1]])

    relevantFeatures = runCut(targetCorrelationNumber,datasetPath, cutMethod, cutAmmount)
    relevantFeaturesName = []
    if(verbose):
        print ("relevantFeatures",relevantFeatures)
    for f in relevantFeatures:
        relevantFeaturesName.append(headerByNumber[f])

    #Removing redundant features
    if(removeRedundantFlag):
        badCandidates = removeRedundant(datasetPath,targetCorrelation,relevantFeaturesName,MICStringParameters,correlationAlgorithmsString,writeCorrelationResults)
        if(verbose):
            print (badCandidates)
        print (badCandidates)
        #print relevantFeaturesName
        for badCandidate in badCandidates:
            print (badCandidate + " from: ", relevantFeaturesName)
            relevantFeaturesName.remove(badCandidate)
            relevantFeatures.remove(headerByName[badCandidate])
    if(verbose):
        print ("relevantFeatures",relevantFeatures)
    #Writing Results
    relevantFeatures.insert(0,headerByName[targetName])
    relevantFeaturesName.insert(0,targetName)
    fsu.writeFilterDataByNumber(datasetPath, outputDatasetPath+datasetPath.split('/')[-1], relevantFeatures)
    return relevantFeatures[1:], relevantFeaturesName[1:]

#cut method (0 top,1 distance,2 distance val, 3 binary val, 4 all val)
def runCut(targetCorrelation, datasetPath="",option=1,cutNumber=0):
    if(cutNumber==0):
        cutNumber = int(len(targetCorrelation)*0.10)
    
    if (option>=1): #validation required
        global samples
        rawData = genfromtxt(datasetPath, delimiter=',', skip_header=0, usecols=range(1,samples+1))
        correlatedFeatures,correlatedValues = fsu.splitColumns(targetCorrelation)
    #print "option",option
    if(option==0):
        return fc.cutTopFeatures(targetCorrelation, cutNumber)
    elif(option==1):
        return fc.cutDistanceFeatures(correlatedValues,correlatedFeatures,cutPerc=0,verbose=0)
    elif(option==2):
        return fc.cutDistanceFeaturesCrossV(rawData,samples,correlatedValues,correlatedFeatures,10,[0,0,1,0,1,0,1,0,0,0,0,0],0)
    elif(option==3):
        return fc.cutBinaryV (rawData,samples,correlatedFeatures,classifierListToExecute=[0,0,1,0,1,0,1,0,0,0,0,0],verbose=0)
    elif(option==4):
        return fc.optimalCutV(rawData,samples,correlatedFeatures,consecutiveDecreaseCut=20,classifierListToExecute=[0,0,1,0,1,0,1,0,0,0,0,0],verbose=0)
    #print "SALI"
#Removing redundant features
def removeRedundant(datasetPath,targetCorrelation,relevantFeautresName,MICStringParameters,correlationAlgorithmsString,writeCorrelationResults=0):
    common.writeCSV(relevantFeautresName,"keysTemp.txt")
    MICCommand = "mictools -i "+datasetPath+" "+MICStringParameters+correlationAlgorithmsString
    MICCommand += " -f keysTemp.txt"
    print (MICCommand)
    redundantF = pymictools.Run(MICCommand)
    redundantF = fsu.selectingPairCorrelation(redundantF,[1,1,0],0.9) # 0.9 those who have 0.9 or higher correlation    
    
    #writing results
    
    #Separating groups and removing redundant features
    if(len(redundantF)>0): #if there is at least 1 redundant element
        groups = findGroups(redundantF)
        if(writeCorrelationResults==1):
            common.writeCSV(groups,outputCorrelationPath+"redundance-"+datasetPath.split("/")[-1],addcommaFlag=1)
    
        return gettingBestOfGroup(groups,targetCorrelation)        
    else:
        return []
def findGroups(featuresRelations):
    groups = {}
    for f in featuresRelations:
        try:
            groups[f[0]].append(f[1])
            groups[f[0]].sort()
        except KeyError:
            groups[f[0]] = [f[1]]
    for i in range(1,100):
        numberofGrupsBefore = len(groups.keys())
        groups = mergeGroups(groups)
        numberofGrupsAfter = len(groups.keys())
        if(numberofGrupsBefore==numberofGrupsAfter): # no more group to merge
            break
    #converting group to list
    newGroup = []
    for key in groups.keys():
        tmpGroup=[]
        tmpGroup.append(key) # the key is also a feature
        for feature in groups[key]:
            tmpGroup.append(feature)
        newGroup.append(tmpGroup)
    return newGroup
def mergeGroups(groups,verbose=0):
    keysToDelete=[]
    for groupoActual in groups.keys():
        for featureActual in groups[groupoActual]:
            try:
                for FR in groups[featureActual]: #feature relacionada
                    groups[groupoActual].append(FR)
                keysToDelete.append(featureActual)
            except KeyError:
                if(verbose):
                    print ("group", featureActual, "not found")
            groups[groupoActual] = sorted(set(groups[groupoActual]))
    keysToDelete = sorted(set(keysToDelete))
    for k in keysToDelete:
        if(verbose):
            print ("actual groups", groups)
            print ("removing",k)
        del groups[k]
    return groups
def gettingBestOfGroup(groups,targetCorrelation):
    goodCandidates = []
    badCandidates = []
    badCandidatesNumber = []
    dictTargetCorrelation = fsu.transformListToDict(targetCorrelation)
    for group in groups:
        bestFeatureinGroupTargetCorrelation = 0
        bestFeatureinGroup = ""
        for feature in group:
            currentFeatureTargetCorrelation = dictTargetCorrelation[feature]
            if(currentFeatureTargetCorrelation>bestFeatureinGroupTargetCorrelation):
                bestFeatureinGroupTargetCorrelation = currentFeatureTargetCorrelation
                bestFeatureinGroup = feature
        goodCandidates.append(bestFeatureinGroup)
        group.remove(bestFeatureinGroup)
        for badFeature in group:
            badCandidates.append(badFeature)
            #badCandidatesNumber.append(headerByName[badFeature])

        badCandidates = list(set(badCandidates)) # removing duplicates
    return badCandidates #badCandidatesNumber

def gettingDataInformation(datasetPath,samplesParam=0):
    global rawData, headerByName, headerByNumber, samples
    global samples
    global outputDatasetPath 
    global outputCorrelationPath
    if(samplesParam>0):
        rawData = genfromtxt(datasetPath, delimiter=',', skip_header=0, usecols=range(1,samplesParam+1))
        headerByNumber, headerByName = common.getHeaders(rawData, hasTarget=1, targetRow=0)
        #print headerByName
    else:
        rawData = common.getRawData(datasetPath,separateHeaderFlag=0)
        #print rawData
        headerByNumber, headerByName = common.getHeadersData(rawData, hasTarget=0, targetRow=0)
        samples = len(rawData[0])-1

#gettingDataInformation("datasets-input/test.csv")