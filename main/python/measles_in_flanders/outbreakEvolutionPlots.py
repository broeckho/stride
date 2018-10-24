'''import argparse
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

def plotHistogram(xs, filename):
    plt.hist(xs)
    plt.savefig(filename)
    plt.clf()

"""
def getCumulativeCasesPerDay(outputDir, scenarioName, numDays):
    cumulativeCasesPerDay = []
    for day in range(numDays):
        cumulativeCasesPerDay.append([])
    seeds = getRngSeeds(outputDir, scenarioName)
    for s in seeds:
        casesFile = os.path.join(outputDir, scenarioName + str(s), 'cases.csv')
        with open(casesFile) as csvfile:
            reader = csv.DictReader(csvfile)
            for row in reader:
                if int(row['timestep']) < numDays:
                    cumulativeCasesPerDay[int(row['timestep'])].append(int(row['cases']))
    return cumulativeCasesPerDay

def getNewCasesPerDay(cumulativeCases):
    newCasesPerDay = []
    lastDay = [1] * len(cumulativeCases[0])
    for today in cumulativeCases:
        newToday = [x - y for x, y in zip(today, lastDay)]
        newCasesPerDay.append(newToday)
        lastDay = today
    return newCasesPerDay

def createOutbreakEvolutionPlots(outputDir, scenarioNames, numDays):
    dayScale = 10
    days = range(numDays)[::dayScale]
    for scenario in scenarioNames:
        cumulativeCases = getCumulativeCasesPerDay(outputDir, scenario, numDays)
        plt.boxplot(cumulativeCases[::dayScale], labels=days)
        plt.xticks(rotation=90)
        plt.xlabel("Day")
        plt.ylabel("Cumulative cases per day")
        plt.ylim(0, 50000)
        plt.savefig(os.path.join(outputDir, scenario + "CumulativeCasesPlot.png"))
        plt.clf()
        newCases = getNewCasesPerDay(cumulativeCases)
        plt.boxplot(newCases[::dayScale], labels=days)
        plt.xticks(rotation=90)
        plt.xlabel("Day")
        plt.ylabel("New cases per day")
        plt.ylim(0, 600)
        plt.savefig(os.path.join(outputDir, scenario + "NewCasesPlot.png"))
        plt.clf()
"""

def main(outputDir, numDays):
    scenarioNames = ["Scenario1", "Scenario2", "Scenario3", "Scenario4"]
    scenarioDisplayNames = ["Uniform immunity rates +\nno household-based clustering",
                            "Age-dependent immunity rates +\nno household-based clustering",
                            "Uniform immunity rates +\nhousehold-based clustering",
                            "Age-dependent immunity rates +\nhousehold-based clustering"]
    # TODO compare different R0s -> plane of R0 vs size vs ??
    # TODO escape probabilities?
    # TODO outbreak evolution plots? Added value for paper?

if __name__=="__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("outputDir", type=str, help="Directory containing simulation output files.")
    parser.add_argument("--numDays", type=int, default=365, help="Number of simulation days for which to make plots.")
    args = parser.parse_args()
    main(args.outputDir, args.numDays)
'''
