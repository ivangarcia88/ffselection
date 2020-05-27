#sklearn
import threading
#import multiprocessing
from multiprocessing import Process, Manager
from sklearn import linear_model, datasets, svm, metrics
from sklearn.datasets import load_iris
from sklearn.datasets import load_digits
from sklearn.neighbors import KNeighborsClassifier, RadiusNeighborsClassifier
from sklearn.pipeline import Pipeline
from sklearn.tree import DecisionTreeClassifier
from sklearn.tree import DecisionTreeRegressor
from sklearn.linear_model import Perceptron
from sklearn.neural_network import BernoulliRBM
from sklearn.cluster import KMeans, MiniBatchKMeans
#from sklearn.cross_validation import train_test_split
#numpy
import numpy as np
from numpy import genfromtxt
#featureselection
#import featureSelection as fs

##### Auxiliar functions
def prepareData(dataset, cols, targetRow, transpose=0, normalize=1):
    target = dataset[targetRow]
    for i in range(0,len(target)):
        if(target[i]<=0):
            target[i] = 0
        else:
             target[i] = 1

    #patch remove target
    dataset = dataset[1:]
    #end patch

    #print target
    #data = dataset[:-1]
    #data = dataset[1:3]
    data = dataset[cols]
    if(transpose):
        data = data.transpose()
    row_sums = data.max(axis=1)
    if(normalize):
        data = data / row_sums[:, np.newaxis]
        #dataNormalized = dataNormalized.transpose()*10
    return [data,target]
def getAcurracy(classifierResults, results):
    #dif  = results - classifierResults
    #dif = abs(dif)
    #return (len(results)- sum(dif))/ len(results)
    diff = 0
    for i in range(0,len(results)):
        if(classifierResults[i] != results[i]):
            diff = diff+1.0
    return ((len(results) - diff) / (len(results)))
def separeDataAndTrainning(data, target, indices, samples, testpercentage):
    dataTrain = data[indices[:-samples*testpercentage]]
    targetTrain = target[indices[:-samples*testpercentage]]
    dataTest  = data[indices[-samples*testpercentage:]]
    targetTest  = target[indices[-samples*testpercentage:]]
    return [dataTrain,targetTrain,dataTest,targetTest]
def processDataToClassify (samples, testpercentage, datasetPath, cols, targetRow):
    np.random.seed(0)
    indices = np.random.permutation(samples)
    rawData = genfromtxt(datasetPath, delimiter=',', skip_header=0, usecols=range(1,samples+1))
    [data,target] = prepareData(rawData, cols, targetRow , 1, 0) #max 0 32
    [dataTrain,targetTrain,dataTest,targetTest] = separeDataAndTrainning (data, target, indices, samples, testpercentage)
    return [dataTrain,targetTrain,dataTest,targetTest]

##### Classifiers
#Cluster
#-Kmeans
def executeKM(data,result):
    dataTrain, targetTrain, dataTest, targetTest, resultPosition = data
    km = KMeans()
    km.fit(dataTrain, targetTrain) 
    result[resultPosition] = getAcurracy(km.predict(dataTest), targetTest)
#-Spectral clustering
def executeMBKM(data,result):
    dataTrain, targetTrain, dataTest, targetTest, resultPosition = data
    km = MiniBatchKMeans()
    km.fit(dataTrain, targetTrain) 
    result[resultPosition] = getAcurracy(km.predict(dataTest), targetTest)
#Decision Tree Clasifier
#-DecisionTreeClassifier
def executeDTC(data,result):
    dataTrain, targetTrain, dataTest, targetTest, resultPosition = data
    dtc = DecisionTreeClassifier(random_state=0)
    dtc.fit(dataTrain, targetTrain)
    result[resultPosition] = getAcurracy(dtc.predict(dataTest), targetTest)
#-DecisionTreeRegressor
def executeDTR(data,result):
    dataTrain, targetTrain, dataTest, targetTest, resultPosition = data
    dtr = DecisionTreeRegressor()
    dtr.fit(dataTrain, targetTrain)
    result[resultPosition] = getAcurracy(dtr.predict(dataTest), targetTest)
#Nearest neighbors
#-KKNeighborsClassifier
def executeKNN(data,result):
    dataTrain, targetTrain, dataTest, targetTest, resultPosition = data
    knn = KNeighborsClassifier()
    knn.fit(dataTrain, targetTrain) 
    KNeighborsClassifier(algorithm='auto', leaf_size=30, metric='minkowski',
               metric_params=None, n_jobs=1, n_neighbors=5, p=2,
               weights='uniform')
    result[resultPosition] = getAcurracy(knn.predict(dataTest), targetTest)
#-RadiusNeighborsClassifier
def executeKRN(data,result):
    dataTrain, targetTrain, dataTest, targetTest, resultPosition = data
    knn = RadiusNeighborsClassifier()
    knn.fit(dataTrain, targetTrain) 
    KNeighborsClassifier(algorithm='auto', leaf_size=30, metric='minkowski',
               metric_params=None, n_jobs=1, n_neighbors=5, p=2,
               weights='uniform')
    result[resultPosition] = getAcurracy(knn.predict(dataTest), targetTest)
#Linear model
#-Logistic Regression
def executeLR(data,result):
    dataTrain, targetTrain, dataTest, targetTest, resultPosition = data
    logistic = linear_model.LogisticRegression()
    logistic.C = 6000.0
    logistic_classifier = linear_model.LogisticRegression(C=100.0)
    logistic_classifier.fit(dataTrain, targetTrain)
    result[resultPosition] = getAcurracy(logistic_classifier.predict(dataTest), targetTest)
#-Perceptron
def executePerc(data,result):
    dataTrain, targetTrain, dataTest, targetTest, resultPosition = data
    nnp = Perceptron().fit(dataTrain, targetTrain)
    result[resultPosition] = getAcurracy(nnp.predict(dataTest), targetTest)
#Support Vector Machine (linear and radial)
#Linear Kernel
def executeSVML(data,result):
    dataTrain, targetTrain, dataTest, targetTest, resultPosition = data
    svm_rbf = svm.SVC(kernel='linear', gamma=0.7, C=1).fit(dataTrain, targetTrain)
    result[resultPosition] = getAcurracy(svm_rbf.predict(dataTest), targetTest)
def executeSVMR(data,result):
    dataTrain, targetTrain, dataTest, targetTest, resultPosition = data
    svm_rbf = svm.SVC(kernel='rbf', gamma=0.7, C=1).fit(dataTrain, targetTrain)
    result[resultPosition] = getAcurracy(svm_rbf.predict(dataTest), targetTest)
#Neural network
def executeRBMLR(data,result):
    dataTrain, targetTrain, dataTest, targetTest, resultPosition = data
    logistic = linear_model.LogisticRegression()
    rbm = BernoulliRBM(random_state=0, verbose=False)
    classifier = Pipeline(steps=[('rbm', rbm), ('logistic', logistic)])
    rbm.learning_rate = 0.001
    rbm.n_iter = 25
    rbm.n_components = 100
    logistic.C = 6000.0
    classifier.fit(dataTrain, targetTrain)
    result[resultPosition] = getAcurracy( classifier.predict(dataTest), targetTest)
def executeRBMP(data,result):
    dataTrain, targetTrain, dataTest, targetTest, resultPosition = data
    logistic = Perceptron()
    rbm = BernoulliRBM(random_state=0, verbose=False)
    classifier = Pipeline(steps=[('rbm', rbm), ('logistic', logistic)])
    rbm.learning_rate = 0.001
    rbm.n_iter = 25
    rbm.n_components = 100
    logistic.C = 6000.0
    classifier.fit(dataTrain, targetTrain)
    result[resultPosition] = getAcurracy( classifier.predict(dataTest), targetTest)
def executeClassifiersParallelized(dataTrain, targetTrain, dataTest, targetTest, classifierListToExecute, verbose=0):
    #0resultKM, 1resultMBKM, 2resultDTC,  3resultDTR,  4resultKNN,    5resultKRN,   
    #6resultLR, 7resultPerc, 8resultSVML, 9resultSVMR, 10resultRBMLR, 11resultRBMP
    global resultKM, resultMBKM, resultDTC, resultDTR, resultKNN, resultKRN, resultLR, resultPerc, resultSVML, resultSVMR, resultRBMLR, resultRBMP
    threads = []
    algorithmsName = []
    manager = Manager()
    results = manager.list([-1]*12)
    resultsToReturn = []
    if(classifierListToExecute[0]):
        threads.append(Process(target=executeKM, args=([dataTrain,targetTrain,dataTest,targetTest,0],results)))
        algorithmsName.append('KM')
    if(classifierListToExecute[1]):
        threads.append(Process(target=executeMBKM, args=([dataTrain,targetTrain,dataTest,targetTest,1],results)))
        algorithmsName.append('MBKM')
    if(classifierListToExecute[2]):
        threads.append(Process(target=executeDTC, args=([dataTrain,targetTrain,dataTest,targetTest,2],results)))
        algorithmsName.append('DTC')
    if(classifierListToExecute[3]):
        threads.append(Process(target=executeDTR, args=([dataTrain,targetTrain,dataTest,targetTest,3],results)))
        algorithmsName.append('DTR')
    if(classifierListToExecute[4]):
        threads.append(Process(target=executeKNN, args=([dataTrain,targetTrain,dataTest,targetTest,4],results)))
        algorithmsName.append('KNN')
    if(classifierListToExecute[5]):
        threads.append(Process(target=executeKRN, args=([dataTrain,targetTrain,dataTest,targetTest,5],results)))
        algorithmsName.append('KRN')    
    if(classifierListToExecute[6]):
        threads.append(Process(target=executeLR, args=([dataTrain,targetTrain,dataTest,targetTest,6],results)))
        algorithmsName.append('LR')
    if(classifierListToExecute[7]):
        threads.append(Process(target=executePerc, args=([dataTrain,targetTrain,dataTest,targetTest,7],results)))
        algorithmsName.append('Perc')
    if(classifierListToExecute[8]):
        threads.append(Process(target=executeSVML, args=([dataTrain,targetTrain,dataTest,targetTest,8],results)))
        algorithmsName.append('SMVL')
    if(classifierListToExecute[9]):
        threads.append(Process(target=executeSVMR, args=([dataTrain,targetTrain,dataTest,targetTest,9],results)))
        algorithmsName.append('SVMR')
    if(classifierListToExecute[10]):
        threads.append(Process(target=executeRBMLR, args=([dataTrain,targetTrain,dataTest,targetTest,10],results)))
        algorithmsName.append('RBMLR')
    if(classifierListToExecute[11]):
        threads.append(Process(target=executeRBMP, args=([dataTrain,targetTrain,dataTest,targetTest,11],results)))
        algorithmsName.append('RBMP')
    for i in threads:
       i.start()
    for i in threads:
        i.join()
    counter = 0
    for j in range(0,len(classifierListToExecute)):
        if(classifierListToExecute[j]):
            resultsToReturn.append(results[j])
            if(verbose):
                print ((algorithmsName[counter],':', resultsToReturn[counter]))
            counter = counter+1
    #print algorithmsName
    return resultsToReturn


#iris = load_iris()
#X, y = iris.data, iris.target

#executeClassifiersParallelized(X,y,X,y, [1,1,1,1,1,1,1,1,1,1,1,1],1)

'''
iris = load_iris()
X, y = iris.data, iris.target
executeClassifiersParallelized(X,y,X,y, [1,1,1,1,1,1,1,1,1,1,1,1],1)
#executeClassifiersParallelized(X,y,X,y, [1,0,1,0,1,0,1,0,1,0,1,0],1)
digits = load_digits()
X, y = digits.data, digits.target
executeClassifiersParallelized(X,y,X,y, [1,1,1,1,1,1,1,1,1,1,1,1],1)
#|executeClassifiersParallelized(X,y,X,y, [1,0,1,0,1,0,1,0,1,1,1,1],1)
#'''