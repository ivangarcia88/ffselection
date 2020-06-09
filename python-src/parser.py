import argparse
import sys

#Display menu when no parameter is given
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
    print("    -x forward stepwise selection cut (integer 0-n), set the max subset size, if 0 selected n is the limit")
    print("    -s static cut (integer)")
    print(" ")
    print("Correlation options: (if none selected, pearson and fastMIC will be executed)")
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
    print("Extra options: ")
    print("    -b dataset orientation (integer 0-1, default = 0, column by feature)")
    print("    -w write correlation results")
    #print("    -z use correlation results") 
    
#Separate parser and set default value
def resultParser(args):
	dictT = {"filepath":"i", "target":"g", "remove_redundant":"r", "threads":"t",
			"alpha":"a", "forward_selection":"x", "static_cut":"s", "pearson":"P", 
			"fastMIC":"R", "SMIC":"S", "clumps":"u","minpearson":"p", "maxpearson":"e", 
			"neigh":"n", "cooling_factor":"c", "minTempo":"m", "minPM":"j", "maxPM":"k", 
			"dataset_orientation":"b", "write_correlation":"w", "use_results":"z"}
	fsoptions = ["remove_redundant","forward_selection","static_cut"]
	fsArgs = {"remove_redundant":None,"forward_selection":None,"static_cut":None}
	micArgs = ""
	for key in args:
		value = args[key]
		if(key in fsoptions):
			if(key=="static_cut"):
				fsArgs[key] = value
			else:
				fsArgs[key] = value
		else:
			if(value!=None and value!=False):
				keyt = dictT[key]
				micArgs += " -"+keyt
				if(value!=True):
					micArgs += " "+str(value)
	#print(args)
	if(not(args["pearson"] or args["fastMIC"] or args["SMIC"])):
		#micArgs += " -P -R"
		micArgs += " -d"

	return [micArgs,fsArgs,args]

#Create and run parser
def run():
	if(len(sys.argv)>1):
		parser = argparse.ArgumentParser(description='MICSelect')
		parser.add_argument('-i', '--filepath', required=True, help="input (filepath)")
		parser.add_argument('-y', '--target', required=True, help="targetName (string)")
		parser.add_argument('-r', '--remove_redundant', action="store_true", help="remove redundant features")
		parser.add_argument('-t', '--threads', 	type=int, help="threads (integer)")
		parser.add_argument('-a', '--alpha', type=float, help="alpha (float)")
		parser.add_argument('-x', '--forward_selection', type=int,  help="forward stepwise selection")
		parser.add_argument('-s', '--static_cut', type=int, help="number of features")
		parser.add_argument('-P', '--pearson', action="store_true", help="Use pearson correlation in the pipeline")
		parser.add_argument('-R', '--fastMIC', action="store_true", help="Use fastMIC correlation in the pipeline")
		parser.add_argument('-S', '--SMIC', action="store_true", help="Use sMIC correlation in the pipeline")
		parser.add_argument('-u', '--clumps', type=float, help="clumps (double >0)")
		parser.add_argument('-p', '--minpearson', type=float, help="minpearson (float 0-1)")
		parser.add_argument('-e', '--maxpearson', type=float, help="maxpearson (float 0-1)")
		parser.add_argument('-n', '--neigh', type=float, help="neigh (float)")
		parser.add_argument('-c', '--cooling_factor', type=float, help="cooling factor (float)")
		parser.add_argument('-m', '--minTempo', type=float, help="min trempo (float 0-1)")
		parser.add_argument('-j', '--minPM', type=float, help="min PM (float 0-1)")
		parser.add_argument('-k', '--maxPM', type=float, help="max PM (float 0-1)")
		parser.add_argument('-b', '--dataset_orientation', type=int, default=0, help="dataset orientation (integer 0-1)")
		parser.add_argument('-w', '--write_correlation', action="store_true", help="write scores of correlation")
		#parser.add_argument('-z', '--use_results', help="use correlation results")
		args = vars(parser.parse_args())
		return resultParser(args)		
	else:
		help()
		return False



	