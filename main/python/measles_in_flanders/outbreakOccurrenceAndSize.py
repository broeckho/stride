import argparse
import csv
import matplotlib.pyplot as plt
import os

'''
def chunks(l, n):
    """Yield successive n-sized chunks from l."""
    for i in range(0, len(l), n):
        yield l[i:i + n]
'''

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

def getFinalOutbreakSizes(outputDir, scenarioName, numDays):
    finalSizes = []
    seeds = getRngSeeds(outputDir, scenarioName)
    for s in seeds:
        finalSizes.append(getFinalOutbreakSize(outputDir, scenarioName, s, numDays))
    return finalSizes

def finalSizesPlot(outputDir, scenarioNames, scenarioDisplayNames, numDays, extinctionThreshold):
    allFinalSizes = []
    for scenario in scenarioNames:
        finalSizes = getFinalOutbreakSizes(outputDir, scenario, numDays)
        finalSizes = [x for x in finalSizes if x > extinctionThreshold]
        allFinalSizes.append(finalSizes)
    plt.boxplot(allFinalSizes, labels=scenarioDisplayNames)
    plt.ylabel("Final outbreak size")
    plt.savefig("FinalSizesNoExtinction.png")
    plt.clf()

def outbreakOccurrencePlot(outputDir, scenarioNames, scenarioDisplayNames, numDays, extinctionThreshold):
    outbreakOccurrences = []
    for scenario in scenarioNames:
        finalSizes = getFinalOutbreakSizes(outputDir, scenario, numDays)
        outbreaks = []
        for f in finalSizes:
            if f <= extinctionThreshold:
                outbreaks.append(0)
            else:
                outbreaks.append(1)
        pctOutbreaks = sum(outbreaks) / len(outbreaks)
        outbreakOccurrences.append(pctOutbreaks)
    plt.bar(scenarioDisplayNames, outbreakOccurrences)
    plt.ylim(0, 1)
    plt.ylabel("Fraction outbreaks")
    plt.savefig("OutbreakOccurrencePlot.png")
    plt.clf()

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

def getEscapeProbability(outputDir, scenario, seed, numDays):
    totalSusceptibles = getSusceptiblesAtStart(outputDir, scenario, seed)
    finalSize = getFinalOutbreakSize(outputDir, scenario, seed, numDays)
    # susceptibles - infected) / susceptibles
    escapeProbability = (totalSusceptibles - finalSize) / totalSusceptibles
    return escapeProbability

def escapeProbabilityPlot(outputDir, scenarioNames, scenarioDisplayNames, numDays):
    allEscapeProbabilities = []
    for scenario in scenarioNames:
        seeds = getRngSeeds(outputDir, scenario)
        scenarioEscapeProbabilities = []
        for s in seeds:
            scenarioEscapeProbabilities.append(getEscapeProbability(outputDir, scenario, s, numDays))
        allEscapeProbabilities.append(scenarioEscapeProbabilities)
    plt.boxplot(allEscapeProbabilities, labels=scenarioDisplayNames)
    plt.ylabel("Escape probability")
    plt.ylim(0, 1.1)
    plt.show()

def main(outputDir, numDays, extinctionThreshold):
    scenarioNames = ["Scenario1", "Scenario2", "Scenario3", "Scenario4"]
    scenarioDisplayNames = ["Uniform\nimmunity rates\n+ no clustering",
                            "Age-dependent\nimmunity rates\n+ no clustering",
                            "Uniform\nimmunity rates\n+ clustering",
                            "Age-dependent\nimmunity rates +\nclustering"]
    # Occurrence of outbreaks
    outbreakOccurrencePlot(outputDir, scenarioNames, scenarioDisplayNames, numDays, extinctionThreshold)
    # Final size distribution without extinction cases
    finalSizesPlot(outputDir, scenarioNames, scenarioDisplayNames, numDays, extinctionThreshold)
    # Escape probabilities
    escapeProbabilityPlot(outputDir, scenarioNames, scenarioDisplayNames, numDays)

if __name__=="__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("outputDir", type=str, help="Directory containing simulation output files.")
    parser.add_argument("--numDays", type=int, default=365, help="Number of simulation days for which to make plots.")
    parser.add_argument("--extinctionThreshold", type=int, default=0, help="Extinction threshold.")
    args = parser.parse_args()
    main(args.outputDir, args.numDays, args.extinctionThreshold)
