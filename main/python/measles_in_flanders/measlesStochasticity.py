import argparse
import os
import time


def runEnsemble(ensembleSize, ensembleIndex, scenarioName):
    stanArg = " --stan " + str(ensembleSize)
    configFile = "measles" + scenarioName + "Immunity.xml"
    configArg = " -c " + configFile
    overrideArg = " -o 'output_prefix=" + scenarioName + str(ensembleIndex) + "'"
    os.system("./bin/stride" + stanArg + configArg + overrideArg)

def runNaive(ensembleSize, numEnsembles, scenarioName):
    totalRuns = ensembleSize * numEnsembles
    for i in range(totalRuns):
        configFile = "measles" + scenarioName + "Immunity.xml"
        configArg = " -c " + configFile
        prefixOverrideArg = " -o 'output_prefix=Naive" + scenarioName + str(i) + "'"
        seedOverrideArg = " -o 'rng_seed=" + str(i) + "'"
        os.system("./bin/stride" + configArg + prefixOverrideArg + seedOverrideArg)


def main(ensembleSize, numEnsembles):
    t1 = time.perf_counter()
    print(numEnsembles)
    for i in range(numEnsembles):
        runEnsemble(ensembleSize, i, "Random")
        #runEnsemble(ensembleSize, i, "AgeDependent")
        #runEnsemble(ensembleSize, i, "Clustering")
    elapsed_time1 = time.perf_counter() - t1
    t2 = time.perf_counter()
    #runNaive(ensembleSize, numEnsembles, "Random")
    elapsed_time2 = time.perf_counter() - t2
    print("Using stan: " + str(elapsed_time1))
    print("Naive: " + str(elapsed_time2))

if __name__=="__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('ensembleSize', type=int, help='size of 1 ensemble')
    parser.add_argument('numEnsembles', type=int, help='number of ensembles to run')
    args = parser.parse_args()
    main(args.ensembleSize, args.numEnsembles)
