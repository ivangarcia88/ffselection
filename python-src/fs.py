import os 
import sys
from functools import partial
from multiprocessing import Pool
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from sklearn.model_selection import train_test_split
from sklearn.svm import SVC
from sklearn.neighbors import KNeighborsClassifier
from sklearn.linear_model import LogisticRegression
from sklearn.naive_bayes import GaussianNB
from sklearn.datasets import make_classification
from sklearn.model_selection import StratifiedKFold
from sklearn.metrics import roc_auc_score
import warnings
warnings.filterwarnings("ignore")

############################### AUXILIAR FUNCTIONS ###############################
def sortMatrix(matrix,index):
    return(sorted(matrix, key = lambda x: abs(x[index])))     

def checkValues(lst):
    indexes = []
    for e in range(len(lst)):
        if(abs(lst[e])>0):
            indexes.append(e)
    return indexes

def indexValue(matrixMeasures): #Get column measure to be used (to identify which score use in pipeline)
    index = []
    for i in range(5):
        row = matrixMeasures[i]
        index.append(max(checkValues(row[2:])))
    return int(round(sum(index)/len(index),0))+2

#Secuential ML classifier
def classifyData(X,y):
    cv = StratifiedKFold(n_splits=3)
    gnb = GaussianNB()
    lr = LogisticRegression(random_state=0, multi_class='auto')
    knn = KNeighborsClassifier(n_neighbors=3)
    svc = SVC(random_state=42)
    classifiers = [gnb,lr,knn,svc]
    names = ["GNB","LR","KNN","SVC"]
    scores = []
    for classifier in classifiers:
        for i, (train, test) in enumerate(cv.split(X, y)):
            classifier.fit(X[train], y[train])
            y_pred = classifier.predict(X[test])
            scores.append(roc_auc_score(y[test], y_pred))
    return round(sum(scores)/len(scores),2) 

#Paralell ML classifier
def parallelMLClassifier(X,y):
    cv = StratifiedKFold(n_splits=3)
    gnb = GaussianNB()
    lr = LogisticRegression(random_state=0, multi_class='auto')
    knn = KNeighborsClassifier(n_neighbors=3)
    svc = SVC(random_state=42)
    classifiers = [gnb,lr,knn,svc]
    names = ["GNB","LR","KNN","SVC"]
    colors = ["red","blue","green","brown"]
    scores = []
    pool = Pool(9)
    parameters = []
    for classifier in classifiers:
        for i, (train, test) in enumerate(cv.split(X, y)):
            parameters.append({"classifier":classifier,"train":train,"test":test})
    func = partial(threadMLClassifier, X, y)
    scores = pool.map(func, parameters)
    pool.close()
    pool.join()
    return round(sum(scores)/len(scores),2)
    
#Thread ML Classifier
def threadMLClassifier(X,y,parameters):
    parameters["classifier"].fit(X[parameters["train"]], y[parameters["train"]])
    y_pred = parameters["classifier"].predict(X[parameters["test"]])
    return roc_auc_score(y[parameters["test"]], y_pred)

#Function to keep dataframe with selected features, can also save the result in csv
def selectFeatures(featureSelected, datasetPath, outputPath=False):
    fsl = []
    for i in range(len(featureSelected)):
        row = featureSelected[i]
        if(i==0):
            fsl.append(row[0])
        fsl.append(row[1])
    print(datasetPath)
    df = pd.read_csv(datasetPath)
    df = df[fsl]
    if(outputPath):
        df.to_csv(outputPath, index=False)
    return df

############################### FEATURE SELECTION FUNCTIONS ###############################
#Evaluating feature revelancy
def featureRanking(matrixMeasures):
    score_column = indexValue(matrixMeasures) #Using the best score in pipeline
    matrixMeasures = sortMatrix(matrixMeasures,score_column) #Sort by score
    matrixMeasures.reverse()
    fr = np.array(matrixMeasures)
    fr = fr[:,[0,1,score_column]] #Keep only feature names and selected score
    return fr

def removeRedundant(featureSelected, featureRelation,threshold=0.9):
    redundantMatrix = []
    #Keeping redundant feautres
    for e in featureRelation:
        if(e[3]>=threshold):
            redundantMatrix.append(e)
    groups = {}
    #Generating groups
    groups = {}
    fgroup = {}
    k = 0
    for e in redundantMatrix:
        #None of them in a group
        if(e[0] not in fgroup and e[1] not in fgroup):
            #print("case None", e[0],e[1])
            groups[k] = [e[0],e[1]]
            fgroup[e[0]] = k
            fgroup[e[1]] = k
            k+=1
        #Both of them in a group
        elif(e[0] in fgroup and e[1] in fgroup):
            #print("case Both", e[0],e[1])
            g0 = fgroup[e[0]] #obtain index of group e0
            g1 = fgroup[e[1]] #obtain index of group e1
            if(g0!=g1): #if they are in deferent groups
                groups[g0].extend(groups[g1]) # merge group 2 int group 1
                for e in groups[g1]: #for every element of group 2, change their group
                    fgroup[e] = g0
                del groups[g1] #del group 2
        #One of them in a group
        elif(e[0] in fgroup and e[1] not in fgroup):
            #print("case e0", e[0],e[1])
            g = fgroup[e[0]] #obtain index of group e0
            groups[g].append(e[1]) #add e1 to that group
            fgroup[e[1]] = g #reference e1 into the group
        else:
            #print("case e1", e[0],e[1])
            g = fgroup[e[1]] #obtain index of group e1
            groups[g].append(e[0]) #add e0 to that group
            fgroup[e[0]] = g #reference e0 into the group
    fsMatrix = []
    
    #Getting redundant feautres 
    for key in groups:
        fcandidates = groups[key]
        for row in featureSelected:
            if(row[1] in fcandidates):
                groups[key].remove(row[1])
                break
    redundantFeatures = []
    for key in groups:
        redundantFeatures.extend(groups[key])
    
    #Removing redundant features
    for row in featureSelected:
        if(row[1] not in redundantFeatures):
            fsMatrix.append(row)
        else:
            print("Removing", row[1])

    return np.array(fsMatrix)

#Forward Stepwise feature selection cut
def forwardSelection(featureSelected, datasetPath, maxFeatures=0):
    if(maxFeatures==0): #If parameters = 0, then explore all features
        maxFeatures = len(featureSelected)
    df = pd.read_csv(datasetPath)
    df = df.sample(frac=1, random_state=42) #Shuffle rows
    fsl = [] #feature selected names
    score = 0
    bestScore = score
    fsMatrix = []
    for i in range(len(featureSelected)):
        row = featureSelected[i]
        fsl.append(row[1])
        fsMatrix.append(row)
        X = df[fsl].values  #Selecting features by anes
        y = df[row[0]].values
        score = parallelMLClassifier(X,y) #Paralell ML Judge
        #score = classifyData(X,y) #Serial ML Judge
        #print(score,fsl)
        if(score>bestScore): #Only keep features that improve accurracy
            bestScore = score
        else:
            fsl.pop()
            fsMatrix.pop()
        #Stop after exploring maxFeatures
        if(maxFeatures-1<=i):
            break
    return fsMatrix