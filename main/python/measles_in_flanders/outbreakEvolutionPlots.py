import argparse
import csv
import matplotlib.pyplot as plt
import os

DAY_SCALE = 10

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
def createOutbreakEvolutionPlots(outputDir, scenarioNames, numDays):
    for scenario in scenarioNames:
        newCases = getNewCasesPerDay(cumulativeCases)
        plt.boxplot(newCases[::dayScale], labels=days)
        plt.xticks(rotation=90)
        plt.xlabel("Day")
        plt.ylabel("New cases per day")
        plt.ylim(0, 600)
        plt.savefig(os.path.join(outputDir, scenario + "NewCasesPlot.png"))
        plt.clf()
'''

def getCumulativeCasesPerDay(outputDir, scenarioName, numDays, extinctionThreshold):
    seeds = getRngSeeds(outputDir, scenarioName)
    allCumulativeCases = []
    for day in range(numDays):
        allCumulativeCases.append([])
    for s in seeds:
        casesFile = os.path.join(outputDir, scenarioName + "_" + str(s), "cases.csv")
        cumulativeCases = []
        with open(casesFile) as csvfile:
            reader = csv.DictReader(csvfile)
            for row in reader:
                timestep = int(row["timestep"])
                if timestep < numDays:
                    cumulativeCases.append(int(row["cases"]))
        # Check if outbreak or extinction occurs - only include outbreak cases
        if cumulativeCases[-1] > extinctionThreshold:
            for day in range(numDays):
                allCumulativeCases[day].append(cumulativeCases[day])
    return(allCumulativeCases)

def getNewCasesPerDay(outputDir, scenarioName, numDays, extinctionThreshold):
    cumulativeCases = getCumulativeCasesPerDay(outputDir, scenarioName, numDays, extinctionThreshold)
    newCasesPerDay = []
    lastDay = [1] * len(cumulativeCases[0])
    for today in cumulativeCases:
        newToday = [x - y for x, y in zip(today, lastDay)]
        newCasesPerDay.append(newToday)
        lastDay = today
    return newCasesPerDay

def cumulativeCasesPerDayPlots(outputDir, scenarioNames, scenarioDisplayNames, numDays, extinctionThreshold):
    days = range(numDays)[::DAY_SCALE]
    for scenario in scenarioNames:
        cumultiveCasesPerDay = getCumulativeCasesPerDay(outputDir, scenario, numDays, extinctionThreshold)
        plt.boxplot(cumultiveCasesPerDay[::DAY_SCALE], labels=days)
        plt.xticks(rotation=90)
        plt.xlabel("Day")
        plt.ylabel("Cumulative cases")
        plt.savefig(os.path.join(outputDir, scenario + "_CumulativeCases.png"))
        plt.clf()

def newCasesPerDayPlots(outputDir, scenarioNames, scenarioDisplayNames, numDays, extinctionThreshold):
    days = range(numDays)[::DAY_SCALE]
    for scenario in scenarioNames:
        newCasesPerDay = getNewCasesPerDay(outputDir, scenario, numDays, extinctionThreshold)
        plt.boxplot(newCasesPerDay[::DAY_SCALE], labels=days)
        plt.xticks(rotation=90)
        plt.xlabel("Day")
        plt.ylabel("New cases")
        plt.savefig(os.path.join(outputDir, scenario + "_NewCases.png"))
        plt.clf()

def main(outputDir, numDays, extinctionThreshold):
    scenarioNames = ["Scenario1", "Scenario2", "Scenario3", "Scenario4"]
    scenarioDisplayNames = ["Uniform immunity rates +\nno household-based clustering",
                            "Age-dependent immunity rates +\nno household-based clustering",
                            "Uniform immunity rates +\nhousehold-based clustering",
                            "Age-dependent immunity rates +\nhousehold-based clustering"]
    cumulativeCasesPerDayPlots(outputDir, scenarioNames, scenarioDisplayNames, numDays, extinctionThreshold)
    newCasesPerDayPlots(outputDir, scenarioNames, scenarioDisplayNames, numDays, extinctionThreshold)

if __name__=="__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("outputDir", type=str, help="Directory containing simulation output files.")
    parser.add_argument("--numDays", type=int, default=365, help="Number of simulation days for which to make plots.")
    parser.add_argument("--extinctionThreshold", type=int, default=0, help="Extinciton threshold.")
    args = parser.parse_args()
    main(args.outputDir, args.numDays, args.extinctionThreshold)
