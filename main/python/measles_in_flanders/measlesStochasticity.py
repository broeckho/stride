import argparse
import os


def runEnsemble(ensembleSize, ensembleIndex, scenarioName):
    stanArg = " --stan " + str(ensembleSize)
    configFile = "measles" + scenarioName + "immunity.xml"
    configArg = " -c " + configFile
    overrideArg = " -o 'output_prefix=" + scenarioName + str(ensembleIndex) + "'"
    os.system("./bin/stride" + stanArg + configArg + overrideArg)

def main(ensembleSize, numEnsembles):
    for i in range(numEnsembles):
        runEnsemble(ensembleSize, i, "Random")
        runEnsemble(ensembleSize, i, "AgeDependent")
        runEnsemble(ensembleSize, i, "Clustering")

if __name__=="__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('ensembleSize', type=int, help='size of 1 ensemble')
    parser.add_argument('numEnsembles', type=int, help='number of ensembles to run')
    args = parser.parse_args()
    main(args.ensembleSize, args.numEnsembles)
