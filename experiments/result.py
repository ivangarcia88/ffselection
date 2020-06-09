from sklearn.model_selection import train_test_split
from sklearn.svm import SVC
from sklearn.ensemble import RandomForestClassifier
from sklearn.neighbors import KNeighborsClassifier
from sklearn.datasets import make_classification
from sklearn.model_selection import StratifiedKFold
from sklearn.metrics import plot_roc_curve
from sklearn.metrics import auc
import matplotlib.pyplot as plt
import pandas as pd
import numpy as np
import os 
import sys

def raScore(datasetPath, datasetName, ax, axTitle):
    df = pd.read_csv(datasetPath)
    data = df.values
    X = data[:,1:]
    y = data[:,0]

    cv = StratifiedKFold(n_splits=3)
    svc = SVC(random_state=42)
    rfc = RandomForestClassifier(random_state=42)
    knn = KNeighborsClassifier(n_neighbors=3)

    #fig, ax = plt.subplots()
    classifiers = [svc,rfc,knn]
    names = ["SVC","RFC","KNN"]
    colors = ["red","blue","green"]
    lmean_tpr = []
    lmean_auc = []
    mean_fpr = np.linspace(0, 1, 100)
    for classifier, name, color in zip(classifiers, names, colors):
        tprs = []
        aucs = []
        for i, (train, test) in enumerate(cv.split(X, y)):
            classifier.fit(X[train], y[train])
            viz = plot_roc_curve(classifier, X[test], y[test],
                                 name='ROC fold {}{}{}'.format(i," ",name),
                                 alpha=0.5, lw=1, ax=ax, color=color, linestyle=':')
            interp_tpr = np.interp(mean_fpr, viz.fpr, viz.tpr)
            interp_tpr[0] = 0.0
            tprs.append(interp_tpr)
            aucs.append(viz.roc_auc)

     
        mean_tpr = np.mean(tprs, axis=0)
        mean_tpr[-1] = 1.0
        mean_auc = auc(mean_fpr, mean_tpr)
        lmean_tpr.append(mean_tpr)
        lmean_auc.append(mean_auc)
        std_auc = np.std(aucs)
        #print("fpr",mean_fpr)
        #print("tpr",mean_tpr)
        ax.plot(mean_fpr, mean_tpr, color=color,
                label=r'Mean ROC '+name+' (AUC = %0.2f $\pm$ %0.2f)' % (mean_auc, std_auc),
                lw=2, alpha=.8)

    ax.plot([0, 1], [0, 1], linestyle='--', lw=2, color='purple',label='Chance', alpha=.8)
    gmean_tpr = np.mean(lmean_tpr, axis=0)
    gmean_auc = np.mean(lmean_auc)

    ax.plot(mean_fpr, gmean_tpr, color="black",
        label=r'Mean ROC (AUC = %0.2f)' % (gmean_auc),
        lw=2, alpha=.8)

    ax.set(xlim=[-0.05, 1.05], ylim=[-0.05, 1.05],
               title=axTitle)
    ax.legend(loc="lower right")
        
    return gmean_auc
    #plt.show()

def compareRaScore(testName, datasetInfo1, datasetInfo2):
    f, (ax1, ax2) = plt.subplots(1, 2)
    f.set_size_inches(18.5, 10.5, forward=True)

    datasetName = datasetInfo2[1][3:]
    plt.suptitle("ROC curve comparison "+datasetName)
    fullAuc = raScore(datasetInfo1[0],datasetInfo1[1],ax1,"Full features set") # full
    fsAuc = raScore(datasetInfo2[0],datasetInfo2[1],ax2,"After feature selection") # fs
    #plt.show()
    plt.savefig("results/"+testName+"/"+testName+"-"+datasetName+".png")
    plt.savefig("results/"+testName+"/"+testName+"-"+datasetName+".svg")
    return [round(fullAuc,2), round(fsAuc,2)]

def compareRaScoreV2(testName, datasetInfo1, datasetInfo2, ax):
    #f, axs = plt.subplots(12, 2)
    datasetName = datasetInfo2[1][3:]
    #plt.suptitle("ROC curve comparison "+datasetName)
    #fullAuc = raScore(datasetInfo1[0],datasetInfo1[1],axs[0][0],"Full features set") # full
    #fsAuc = raScore(datasetInfo2[0],datasetInfo2[1],axs[0][1],"After feature selection") # fs
    fullAuc = raScore(datasetInfo1[0],datasetInfo1[1],ax[0],"Full features set "+datasetName) # full
    fsAuc = raScore(datasetInfo2[0],datasetInfo2[1],ax[1],"After feature selection "+datasetName) # fs
    fsAuc = raScore(datasetInfo2[0],datasetInfo2[1],ax[2],"After feature selection "+datasetName) # fs
    
    return [round(fullAuc,2), round(fsAuc,2)]
    

pathfull = "datasets-test/full/"
pathfs =  "datasets-test/fs/"
datasets = ["ds1.csv", "ds2.csv", "ds3.csv", "ds4.csv", "ds5.csv", "ds6.csv", "ds7.csv", "ds8.csv", 
"ds9.csv", "ds10.csv", "ds11.csv", "ds12.csv", ]

testName = "top10"

if(not os.path.exists("results")):
    os.mkdir("results/")
if(not os.path.exists("results/"+testName)):
    os.mkdir("results/"+testName)


fullAucList = []
fsAucList = []

i = 0
for datasetName in datasets:
    if(i%6==0):
        plt.savefig("results/full"+str(i)+".png")
        plt.margins(0.01)
        f, axs = plt.subplots(6, 3)
        plt.suptitle('Amazing Stats', size=16, y=1.12);      
        

        f.set_size_inches(30, 10*6)

    
    print(datasetName)
    datasetInfo1 = (pathfull+datasetName,"full "+datasetName)
    datasetInfo2 = (pathfs+datasetName,"fs "+datasetName)
    [fullAuc, fsAuc] = compareRaScoreV2(testName, datasetInfo1, datasetInfo2, axs[i%6])
    fullAucList.append(fullAuc)
    fsAucList.append(fsAuc)
    i+=1
    if(i==1):
        break
    
plt.savefig("results/full"+str(i)+".png")   

    

#df = pd.DataFrame(np.array([fullAucList, fsAucList]),
#                   columns=datasets)
#df.to_csv ("results/"+testName+".csv", index = False, header=True)