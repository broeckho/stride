import argparse
import csv
import matplotlib.pyplot as plt
import os

def getRngSeeds(outputDir, scenarioName):
    seeds = []
    seedsFile = os.path.join(outputDir, scenarioName + "Seeds.csv")
    with open(seedsFile) as csvfile:
        reader = csv.reader(csvfile)
        for row in reader:
            for s in row:
                seeds.append(int(s))
    return seeds

'''


def plotHistogram(xs, filename):
    plt.hist(xs)
    plt.savefig(filename)
    plt.clf()

"""
def getFinalOutbreaksSizes(outputDir, scenarioName, numDays):
    finalSizes = []
    seeds = getRngSeeds(outputDir, scenarioName)
    for s in seeds:
        casesFile = os.path.join(outputDir, scenarioName + str(s), "cases.csv")
        with open(casesFile) as csvfile:
            reader = csv.DictReader(csvfile)
            for row in reader:
                if int(row['timestep']) == (numDays - 1):
                    finalSizes.append(int(row['cases']))
                    break
    return finalSizes

def createOutbreakSizesPlots(outputDir, scenarioNames, numDays):
    allFinalSizes = []
    for scenario in scenarioNames:
        finalSizes = getFinalOutbreaksSizes(outputDir, scenario, numDays)
        allFinalSizes.append(finalSizes)
    plt.boxplot(allFinalSizes, labels=["Uniform +\nnot clustered",
                "Age-dependent +\nnot clustered", "Uniform +\nclustered",
                "Age-dependent +\nclustered"])
    plt.ylabel("Final outbreak sizes after {} days".format(numDays + 1))
    plt.savefig(os.path.join(outputDir, "FinalSizesPlot.png"))
    plt.clf()
'''

def getFinalOutbreakSizes(outputDir, scenarioName, numDays):
    finalSizes = []
    seeds = getRngSeeds(outputDir, scenarioName)
    for s in seeds:
        casesFile = os.path.join(outputDir, scenarioName + "_" + str(s), "cases.csv")
        with open(casesFile) as csvfile:
            reader = csv.DictReader(csvfile)
            for row in reader:
                timestep = int(row["timestep"])
                if timestep == (numDays - 1):
                    finalSizes.append(int(row["cases"]))
                    break
    return finalSizes

def finalSizesPlot(outputDir, scenarioNames, scenarioDisplayNames, numDays, extinctionThreshold):
    allFinalSizes = []
    for scenario in scenarioNames:
        finalSizes = getFinalOutbreakSizes(outputDir, scenario, numDays)
        finalSizes = [x for x in finalSizes if x > extinctionThreshold]
        allFinalSizes.append(finalSizes)
    plt.boxplot(allFinalSizes, labels=scenarioDisplayNames)
    #plt.tick_params(labelsize=9)
    plt.ylabel("Final outbreak size")
    plt.show()

def main(outputDir, numDays, extinctionThreshold):
    scenarioNames = ["Scenario1", "Scenario2", "Scenario3", "Scenario4"]
    scenarioDisplayNames = ["Uniform\nimmunity rates\n+ no clustering",
                            "Age-dependent\nimmunity rates\n+ no clustering",
                            "Uniform\nimmunity rates\n+ clustering",
                            "Age-dependent\nimmunity rates +\nclustering"]
    # TODO occurrence of outbreaks
    # Final size distribution without extinction cases
    finalSizesPlot(outputDir, scenarioNames, scenarioDisplayNames, numDays, extinctionThreshold)
    # TODO escape probability?

if __name__=="__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("outputDir", type=str, help="Directory containing simulation output files.")
    parser.add_argument("--numDays", type=int, default=365, help="Number of simulation days for which to make plots.")
    parser.add_argument("--extinctionThreshold", type=int, default=0, help="Extinction threshold.")
    args = parser.parse_args()
    main(args.outputDir, args.numDays, args.extinctionThreshold)
