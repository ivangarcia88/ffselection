from os import path
import argparse
import pandas as pd


def transposeDataset(inputPath, outPath):
    df = pd.read_csv(inputPath, header=None)
    df2 = df.T
    df2.to_csv(outPath, header=None, index=False)

if __name__ == "__main__":
	parser = argparse.ArgumentParser(description='Transposing CSV')
	parser.add_argument('-i', '--input', required=True, help="The path of input dataset")
	parser.add_argument('-o', '--output', required=False, help="The path of output dataset, if not set, will overwrite input dataset")
	
	args = vars(parser.parse_args())
	inputPath = args["input"]
	if(path.exists(inputPath)):
		outputPath = args["output"]
		if(outputPath == None):
			outputPath = inputPath
		transposeDataset(inputPath,outputPath)
	else:
		print("ERROR:", inputPath, "does not exist")

	


#transposeDataset("splice_scaleT.csv", "splice_scale.csv")