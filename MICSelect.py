import sys
sys.path.insert(0,'python-src')
sys.path.insert(0,'mic/')
import numpy as np
import pandas as pd
import time
import pymictools
import futil
import parser
import fs

if __name__ == "__main__":
    args = parser.run()
    if(args): #if there are arguments
        [micArgs,fsArgs,fullArgs] = args
        
        futil.createFolder("tmp")
        filename = futil.getFileName(fullArgs["filepath"])    
        if(fullArgs["dataset_orientation"]==0): #dataset column feature
            micInputPath = "tmp/"+filename
            mlInputPath = fullArgs["filepath"]
            futil.transposeDataset(fullArgs["filepath"],micInputPath)
            micArgs = micArgs.replace(fullArgs["filepath"],micInputPath)
        else: #dataset row feature
            micInputPath = fullArgs["filepath"]
            mlInputPath = "tmp/"+filename
            futil.transposeDataset(fullArgs["filepath"],mlInputPath)
            
        #Exectute ranking features
        initTime = time.time()
        MICCommand = "mictools"+micArgs
        print(MICCommand)
        result = pymictools.Run(MICCommand) #matrix measure as list of list
        if(len(result) <= 5):
            print("Error, tool cannot work with too few samples")
            futil.finish()
        featureSelected = fs.featureRanking(result)
        totalFeatures = len(featureSelected)
        
        #Writting correlation if required
        if(fullArgs["write_correlation"]):
            df = pd.DataFrame(featureSelected)
            df.to_csv("correlation-output/"+filename, header=["feature 1", "feature 2", "score"], index=False)
        
        #Getting parameters
        s = fullArgs["static_cut"]
        r = fullArgs["remove_redundant"]
        x = fullArgs["forward_selection"]
        
        #Static feature selection cut
        if(s!=None):
            featureSelected = featureSelected[0:s,:]
        #Forward Stepwise feature selection cut
        if(x!=None):
            featureSelected = fs.forwardSelection(featureSelected,mlInputPath,x)
        #Removing redundant features    
        if(r):
            print("Removing redundant features")
            fstring = ""
            for row in featureSelected:
                fstring += row[1]+"\n"
            keysPath = "tmp/keys.txt"
            futil.createFolder("tmp")
            futil.createFile(keysPath,fstring[:-1])
            MICCommand = "mictools"+ " -i " +micInputPath + " -f " + keysPath + " -R" " -t " +str(fullArgs["threads"]) 
            print(MICCommand)
            result = pymictools.Run(MICCommand)
            featureSelected = fs.removeRedundant(featureSelected,result)

        #Displaying results
        #print(featureSelected)
        print("====================================================")
        print("Total number of features",totalFeatures)
        print("Number of features selected",len(featureSelected))
        endTime = time.time()
        print("Feature selection time", round(endTime-initTime,4))
        print("Writing output dataset")
        fs.selectFeatures(featureSelected, mlInputPath,"datasets-output/"+filename)
        print("====================================================")