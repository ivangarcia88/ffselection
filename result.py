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

#Process ROC AUC Score with KFold
def raScore(datasetPath, ax, axTitle):
    df = pd.read_csv(datasetPath)
    #df = df.sample(frac=1, random_state=30) #Shuffle rows
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
        mean_auc = np.round(auc(mean_fpr, mean_tpr),2)
        lmean_tpr.append(mean_tpr)
        lmean_auc.append(mean_auc)
        std_auc = np.round(np.std(aucs),2)
        ax.plot(mean_fpr, mean_tpr, color=color,
                label=r'Mean ROC '+name+' (AUC = %0.2f $\pm$ %0.2f)' % (mean_auc, std_auc),
                lw=2, alpha=.8)

    ax.plot([0, 1], [0, 1], linestyle='--', lw=2, color='purple',label='Chance', alpha=.8)
    gmean_tpr = np.round(np.mean(lmean_tpr, axis=0),2)
    gmean_auc = np.round(np.mean(lmean_auc),2)

    ax.plot(mean_fpr, gmean_tpr, color="black",
        label=r'Mean ROC (AUC = %0.2f)' % (gmean_auc),
        lw=2, alpha=.8)

    ax.set(xlim=[-0.05, 1.05], ylim=[-0.05, 1.05],
               title=axTitle)
    ax.legend(loc="lower right")
        
    return gmean_auc
    #plt.show()

#Process and plot individually
def individualRaScore(testName, datasetName):
    datasetInfo1 = "datasets-input/"+datasetName
    datasetInfo2 = "datasets-test/fss/"+datasetName
    datasetInfo3 = "datasets-test/fsx/"+datasetName
    f, ax = plt.subplots(1, 3)
    f.set_size_inches(30, 10.5, forward=True)
    plt.suptitle("ROC curve comparison "+datasetName)
    fullAuc = raScore(datasetInfo1,ax[0],"Full features set "+datasetName) # full
    fsAuc = raScore(datasetInfo2,ax[1],"Top 5 features "+datasetName) # fs
    fs2Auc = raScore(datasetInfo3,ax[2],"Stepwise feature selection set "+datasetName) # fs2
    #plt.show()
    plt.savefig("results/"+testName+"/"+testName+"-"+datasetName+".png")
    #plt.savefig("results/"+testName+"/"+testName+"-"+datasetName+".svg")
    return [round(fullAuc,2), round(fsAuc,2), round(fs2Auc,2)]

#Process and plot by groups    
def groupedRaScore(testName, datasetName, ax):
    datasetInfo1 = "datasets-input/"+datasetName
    datasetInfo2 = "datasets-test/fss/"+datasetName
    datasetInfo3 = "datasets-test/fsx/"+datasetName
    fullAuc = raScore(datasetInfo1,ax[0],"Full features set "+datasetName) # full
    fsAuc = raScore(datasetInfo2,ax[1],"Top 5 features "+datasetName) # fs
    fs2Auc = raScore(datasetInfo3,ax[2],"Stepwise feature selection set "+datasetName) # fs2
    return [round(fullAuc,2), round(fsAuc,2), round(fs2Auc,2)]
    

#Process and plot results
def processResults(datasets, pGrouped=True):
    fullAucList = []
    fsAucList = []
    fs2AucList = []
    i = 0
    size = 6
    for datasetName in datasets:
        print(datasetName)
        if(pGrouped): #Plot grouped
            if(i%size==0):
                f, axs = plt.subplots(size, 3)
                plt.subplots_adjust(left=0.02, bottom=0.01, right=0.99, top=0.99, wspace=0.07, hspace=0.11)
                f.set_size_inches(26.4, 8*6)        
            [fullAuc, fsAuc, fs2Auc] = groupedRaScore(testName, datasetName, axs[i%size])
            if(i%size==(size-1)): #Save figure
                plt.savefig("results/"+testName+"/"+"full"+str(i+1)+".png")
                #plt.savefig("results/"+testName+"/"+"full"+str(i+1)+".svg")
                #plt.savefig("results/"+testName+"/"+"full"+str(i+1)+".pdf")
        else: #Plot individual
            [fullAuc, fsAuc, fs2Auc] = individualRaScore(testName, datasetName)
        #Apend results
        fullAucList.append(fullAuc)
        fsAucList.append(fsAuc)
        fs2AucList.append(fs2Auc)
        i+=1
    return [fullAucList, fsAucList, fs2AucList]


if __name__ == "__main__":

    testName = "top5-20"
    datasets = ["ds1.csv", "ds2.csv", "ds3.csv", "ds4.csv", "ds5.csv", "ds6.csv", 
                "ds7.csv", "ds8.csv", "ds9.csv", "ds10.csv", "ds11.csv", "ds12.csv", ]

    #Create result folder
    if(not os.path.exists("results")):
        os.mkdir("results/")
    if(not os.path.exists("results/"+testName)):
        os.mkdir("results/"+testName)

    #Process and plot
    results = processResults(datasets)
        
    #Save results
    df = pd.DataFrame(np.array(results), columns=datasets)
    df.to_csv ("results/"+testName+".csv", index = False, header=True)