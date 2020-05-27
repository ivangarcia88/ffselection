import os.path
import sys, getopt
import common
#from sets import Set

####### Feature Selector Options #######

#Feature Selector 
#1 param
#Mandatory
#-i input - checkDataset()
#-y targetName - checkDataset()
#Not mandatory
#-t threads - isDigit()
#-a alpha - isANumber()
#-x cut method - isDigit()
#-s static cut - isDigit()

#0 param
#-h header
#-r remove redundant
#-w writeResults (correlation results)
#-z use previous reulsts
#-P pearson
#-R FastMIC
#-S SMIC

#PMIC
#-u clumps - isANumber()
#-p minpearson - isANumber()
#-e maxpearson - isANumber()

#parametros MIC -u,-p,-e,-n,-c,-m,-j,-k,-t,-a,-h
#parametros FS -x
#parametros FS-MIC -i,-y
#SMIC
#-n neigh isANumber()
#-c cooling isANumber()
#-m min tempo isANumber()
#-j min PM isANumber()
#-k max PM isANumber()

###################################


optionMap = {}
relationMap = {}
validationMap = {}
micOptions = ['-u','-p','-e','-n','-c','-m','-j','-k','-t','-a','-h']
optionMap['-i'] = 0
optionMap['-y'] = 1
optionMap['-w'] = 2
optionMap['-z'] = 3
optionMap['-a'] = 4
optionMap['-h'] = 5
optionMap['-t'] = 6
optionMap['-u'] = 7
optionMap['-p'] = 8
optionMap['-n'] = 9
optionMap['-c'] = 10
optionMap['-m'] = 11
optionMap['-j'] = 12
optionMap['-k'] = 13
optionMap['-e'] = 14
optionMap['-x'] = 15
optionMap['-s'] = 16
optionMap['-r'] = 17

tel = {'jack': 4098, 'sape': 4139}
relationMap[0]= ['-a','-h','-i','-t','-w','-y','-z', '-x', '-r', '-h', '-s'] #for all
relationMap[1] = ['-e','-p','-u'] #PMIC
relationMap[2] = ['-n','-c','-m','-j','-k'] #SMIC
validationMap[-1] = ['-i', '-y', '-w', '-z', '-h', '-r'] # don't require validation (in function optionValidation)
validationMap[0] = ['-t', '-c', '-x', '-s'] # is a digit
validationMap[1] = ['-u','-p','-e','-n','-m','-j','-k','-a'] # is a number

def help():
    print(" ")
    print("Usage: main.py -i [InputDataSet] -y [TargetName], [Feature Selection Options], [Correlation Options], [Algorithm options]")
    print(" ")
    print("Feature Selection mandatory options:")
    print("    -i input (filepath)")
    print("    -y targetName (string)")
    print(" ")
    print("Feature Selection not mandatory options:")
    print("    -r remove redundant features")
    print("    -t threads (integer)")
    print("    -a alpha (float)")
    print("    -x cut method (integer 0-4) [0 -> cut 10% of data, 1 cut by distance, 2 cut by distance with cross validation, 3 binary cut, 4 optimal cut (long time validation)]")
    print("    -s static cut (integer)")
    print("    -w write correlation results")
    print("    -z use correlation results") 
    print(" ")
    print("Correlation options:")
    print("    -P use pearson")
    print("    -R use FastMIC")
    print("    -S use SMIC")
    print(" ") 
    print("FastMIC options:")
    print("    -u clumps (integer)")
    print("    -p minpearson (float 0-1)")
    print("    -e maxpearson (float 0-1)")
    print(" ")
    print("SMIC options:")
    print("    -n neigh (float)")
    print("    -c cooling factor (float)")
    print("    -m min tempo (float 0-1)")
    print("    -j min PM (float 0-1)")
    print("    -k max PM float(0-1)")
    print(" ") 

def optionValidation(option,parameter):
    global validationMap
    if(option in validationMap[0]):
        return isADigit(parameter)
    elif(option in validationMap[1]):
        return isANumber(parameter)
    elif(option in validationMap[-1]):
        return [True]
    else:
        error = "option "+option+" not found"
        return [False,error]
def isANumber(data):
    try:
        float(data)
        return [True]
    except ValueError:
        error = data+" is not a number"
        return [False,error]
    return [True]
def isADigit(data):
    if not (data.isdigit()):
        error = "invalid: "+data+", must be an integer"
        return [False,error]
    else:
        return [True]
def containsFilds(lista,listb): #a superset of b
    return Set(lista).issuperset(Set(listb))

def checkDataset(datasetPath, target):
    try:
        headers,rawData = common.getRawData(datasetPath,separateHeaderFlag=1)
    except:
        error = datasetPath+" does not exist"
        return [False,error]
    #Check if target field exist
    if ( not target in headers ) :
        error = "target does not exist in datset"
        return [False,error]
    #Check rows length
    rowLength = len(rawData[0])
    for row in rawData:
        if(len(row) != rowLength):
            error = "mismatch length in rows"
            return [False,error]
    return [True]
def splitingOptions(dataString):
    optionList = []
    paramList = []
    algList = [0,0,0]
    dataList = dataString.split(' ')
    dataList.remove(dataList[0])
    jumpNext = False
    for o in dataList:
        if (o == ""):
            break
        # 0 param case (algorithm case)
        if (jumpNext):
            paramList.append(o)
            jumpNext = False
        else:
            if(o == '-P'):
                algList[0] = 1
                jumpNext = False
            elif(o == '-R'):
                algList[1] = 1
                jumpNext = False
            elif(o == '-S'):
                algList[2] = 1
                jumpNext = False
            elif(o == '-h' or o == '-r' or o == '-w' or o == '-z'):
                optionList.append(o)
                paramList.append(1)
                jumpNext = False
            else:
                # 1 param case
                if(o in optionMap.keys()):
                    optionList.append(o)
                    jumpNext = True
                else:
                    error = o+ "is an invalid option"
                    return [False,error]
    #print "3:", [optionList, paramList, algList]
    if(sum(algList)==0):
        algList = [1,1,0]
    return [optionList, paramList, algList]
def detectError(dataString,verbose=0):
    global optionMap
    global relationMap
    optionListSorted = [False]*len(optionMap.keys())
    paramListSorted = [False]*len(optionMap.keys())
    
    #Spliting Parameters
    splitedOptions = splitingOptions(dataString)
    if(splitedOptions[0]!=False):
        [optionList, paramList, algList] = splitedOptions
    else:
        return splitedOptions
    #Mapping Data and checking valid options and dependencies
    validOptionDependency = relationMap[0]
    #print validOptionDependency
    for i in range(1,3):
        if(algList[i]):
            for o in relationMap[i]:
                validOptionDependency.append(o)
    
    for i in range(0, len(optionList) ): #checking valid options and dependencies
        option = optionList[i]
        parameter = paramList[i]
        if(option in validOptionDependency):
            optionListSorted[optionMap[option]] = option
            paramListSorted[optionMap[option]] = parameter
        else:
            error =  option+" is not a valid option or does not reach algorithm dependencies"
            return [False,error]

    #Checking obligatory parameters (-i, -y) and validate
    if not(optionListSorted[optionMap['-i']] and optionListSorted[optionMap['-y']]): #check for mandatory parameters
        error =  option+" mandatory options not reached (-i -y)"
        return [False,error]
    checkMandatory = checkDataset( paramListSorted[optionMap['-i']], paramListSorted[optionMap['-y']])
    if not ( checkMandatory[0] ) : #checking datset consistency and if target exists
        return checkMandatory
    
    #Checking for errors
    if(verbose):
        print (splitedOptions)
        print (optionListSorted)
        print (paramListSorted)
    for i in range(0,len(optionListSorted)):
        option = optionListSorted[i]
        if(option != False):
            parameter = paramListSorted[i]
            if(verbose):
                print (i)
                print (option)
                print (parameter)
            if not (optionValidation(option,parameter)[0]):
                return optionValidation(option,parameter)
    MICStringParameters =  ""
    for option in micOptions:
        position = optionMap[option]
        optionString = optionListSorted[position]
        if (optionString!=False): #if parameter is set
            MICStringParameters += optionString+" "+paramListSorted[position]+" "
    MICStringParameters = MICStringParameters[:-1]
    return optionListSorted, paramListSorted, MICStringParameters, algList

def argToString(argv):
    argString = ""
    for arg in argv:
        argString += arg+" "
    return argString

def run(argv):
    argv2 = argToString(argv)
    #argv2 = "main.py -i datasets-input/dukeT.csv -y 0 -P -R -x 2 -t 8 -r -a 0.6 -z" #example
    #argv2 = "main.py -i datasets-input/leuT.csv -y 0 -P -R -s 300 -t 8 -r"
    return detectError(argv2,0)

#run('')