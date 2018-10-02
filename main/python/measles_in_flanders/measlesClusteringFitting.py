'''import argparse
import csv
import matplotlib.pyplot as plt
import os
import random
from scipy import stats

def getSeeds(outputDir, scenarioName):
    seeds = []
    seedsFile = os.path.join(outputDir, scenarioName + 'Seeds.csv')
    with open(seedsFile) as csvfile:
        reader = csv.reader(csvfile)
        seeds = [int(x) for x in next(reader)]
    return seeds

def getFinalOutbreaksSizes(outputDir, scenarioName, finalDay):
    finalSizes = []
    seeds = getSeeds(outputDir, scenarioName)
    for s in seeds:
        casesFile = os.path.join(outputDir, scenarioName + str(s), 'cases.csv')
        with open(casesFile) as csvfile:
            reader = csv.DictReader(csvfile)
            for row in reader:
                if int(row['timestep']) == finalDay:
                    finalSizes.append(int(row['cases']))
                    break
    return finalSizes

def getRandomIndices(sampleSize):
    indices = []
    for i in range(sampleSize):
        indices.append(random.randrange(sampleSize))
    return indices

def distributionOfSampleMeans(finalOutbreakSizes, sampleSize, numSampleSets):
    sampleMeans = []
    for i in range(numSampleSets):
        indices = getRandomIndices(sampleSize)
        sizes = [finalOutbreakSizes[i] for i in indices]
        sampleMeans.append(sum(sizes) / len(sizes))
    plt.hist(sampleMeans)
    plt.show()
    stats.probplot(sampleMeans, dist="norm", plot=plt)
    plt.show()

def ksTest(finalOutbreakSizes):
    trainingSetSize = int((len(finalOutbreakSizes) / 100) * 50)
    print(stats.ks_2samp(finalOutbreakSizes[:trainingSetSize], finalOutbreakSizes[trainingSetSize:]))

def main(outputDir, sampleSize, numSampleSets):
    scenarioNames = ['Random']
    for scenario in scenarioNames:
        finalOutbreakSizes = getFinalOutbreaksSizes(outputDir, scenario, 364)
        distributionOfSampleMeans(finalOutbreakSizes, sampleSize, numSampleSets)
        ksTest(finalOutbreakSizes)


if __name__=="__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("dir", type=str, help="Directory containing simulation output files.")
    parser.add_argument("--sampleSize", type=int, default=10)
    parser.add_argument("--numSampleSets", type=int, default=5)
    args = parser.parse_args()
    main(args.dir, args.sampleSize, args.numSampleSets)
'''
