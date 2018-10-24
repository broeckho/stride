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

def getFinalOutbreakSize(outputDir, scenarioName, seed, numDays):
    casesFile = os.path.join(outputDir, scenarioName + "_" + str(seed), "cases.csv")
    with open(casesFile) as csvfile:
        reader = csv.DictReader(csvfile)
        for row in reader:
            timestep = int(row["timestep"])
            if timestep == numDays - 1:
                return int(row["cases"])

def getSusceptiblesAtStart(outputDir, scenario, seed):
    totalSusceptibles = 0
    susceptiblesFile = os.path.join(outputDir, scenario + "_" + str(seed), "susceptibles.csv")
    with open(susceptiblesFile) as csvfile:
        reader = csv.DictReader(csvfile)
        for row in reader:
            isSusceptible = int(row["susceptible"])
            if isSusceptible == 1:
                totalSusceptibles += 1
    return totalSusceptibles

def getEscapeProbability(outputDir, scenarioName, seed, finalOutbreakSize):
    totalSusceptibles = getSusceptiblesAtStart(outputDir, scenarioName, seed)
    escapeProbability = (totalSusceptibles - finalOutbreakSize) / totalSusceptibles
    return escapeProbability

def escapeProbabilityPlot(outputDir, scenarioNames, scenarioDisplayNames, numDays, extinctionThreshold):
    allEscapeProbabilities = []
    for scenario in scenarioNames:
        print(scenario)
        seeds = getRngSeeds(outputDir, scenario)
        scenarioEscapeProbabilities = []
        for s in seeds:
            print("        " + scenario + str(s))
            finalOutbreakSize = getFinalOutbreakSize(outputDir, scenario, s, numDays)
            if finalOutbreakSize > extinctionThreshold:
                scenarioEscapeProbabilities.append(getEscapeProbability(outputDir, scenario, s, finalOutbreakSize))
        allEscapeProbabilities.append(scenarioEscapeProbabilities)
    plt.boxplot(allEscapeProbabilities, labels=scenarioDisplayNames)
    plt.ylabel("Escape probability")
    plt.ylim(0, 1.05)
    plt.savefig(os.path.join(outputDir, "EscapeProbabilityPlot.png"))
    plt.clf()

def main(outputDir, numDays, extinctionThreshold):
    scenarioNames = ["Scenario1", "Scenario2", "Scenario3", "Scenario4"]
    scenarioDisplayNames = ["Uniform\nimmunity rates\n+ no clustering",
                            "Age-dependent\nimmunity rates\n+ no clustering",
                            "Uniform\nimmunity rates\n+ clustering",
                            "Age-dependent\nimmunity rates +\nclustering"]
    escapeProbabilityPlot(outputDir, scenarioNames, scenarioDisplayNames, numDays, extinctionThreshold)

if __name__=="__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("outputDir", type=str, help="Directory containing simulation output files.")
    parser.add_argument("--numDays", type=int, default=365, help="Number of simulation days for which to make plots.")
    parser.add_argument("--extinctionThreshold", type=int, default=0, help="Extinction threshold.")
    args = parser.parse_args()
    main(args.outputDir, args.numDays, args.extinctionThreshold)
