import argparse
import csv
import matplotlib.pyplot as plt
import os

from collections import OrderedDict

def boxplot(data, labels, title, xLabel, yLabel, yMin, yMax):
    """
        Create boxplot.
    """
    plt.boxplot(data, labels=labels)
    plt.title(title)
    plt.xlabel(xLabel)
    plt.ylabel(yLabel)
    plt.ylim(yMin, yMax)
    plt.show()

def histogram(data, numBins, title, xLabel, yLabel):
    """
        Create histogram.
    """
    plt.hist(data, bins=numBins)
    plt.title(title)
    plt.xlabel(xLabel)
    plt.ylabel(yLabel)
    plt.show()

def getCasesPerDay(directory, scenarioName, ensembleId):
    casesPerDay = []
    filename = os.path.join(directory, scenarioName + str(ensembleId) + "_stan_infected.csv")
    with open(filename) as csvfile:
        reader = csv.reader(csvfile)
        next(reader) # Skip header
        for row in reader:
            casesPerDay.append([int(x) for x in row])
    return casesPerDay

def getProportionOutbreaks(casesPerDay, minOutbreakSize):
    totalCases = casesPerDay[-1]
    outbreaks = [1.0] * len(totalCases)
    for i in range(len(totalCases)):
        if totalCases[i] < minOutbreakSize:
            outbreaks[i] = 0.0
    return sum(outbreaks) / len(outbreaks)

def getOutbreakDurations(casesPerDay, numDays):
    totalCases = casesPerDay[-1]
    outbreakDurations = [0] * len(totalCases)
    for day in range(numDays, -1, -1):
        casesThisDay = casesPerDay[day]
        for i in range(len(casesThisDay)):
            if casesThisDay[i] != totalCases[i] and outbreakDurations[i] == 0:
                outbreakDurations[i] = day
    return outbreakDurations

def removeNoOutbreaks(casesPerDay, minOutbreakSize):
    totalCases = casesPerDay[-1]
    noOutbreakIds = [i for i in range(len(totalCases)) if totalCases[i] < minOutbreakSize]
    for day in range(len(casesPerDay)):
        casesThisDay = casesPerDay[day]
        casesPerDay[day] = [casesThisDay[i] for i in range(len(casesThisDay)) if i not in noOutbreakIds]
    return casesPerDay

def main(directory, scenarios, onlyOutbreaks):
    numDays = 365
    numEnsembles = 40
    minOutbreakSize = 2
    allProportionOutbreaks = []
    allOutbreakSizes = []
    for scenarioName in scenarios:
        scenarioCases = {}
        for day in range(numDays + 1):
            scenarioCases[day] = []
        scenarioProportionOutbreaks = []
        scenarioOutbreakSizes = []
        scenarioOutbreakDurations = []
        for ensembleId in range(numEnsembles):
            casesPerDay = getCasesPerDay(directory, scenarioName, ensembleId)
            scenarioProportionOutbreaks.append(getProportionOutbreaks(casesPerDay, minOutbreakSize))
            if onlyOutbreaks:
                # Remove cases in which no outbreak occurs
                casesPerDay = removeNoOutbreaks(casesPerDay, minOutbreakSize)
            for day in range(numDays):
                scenarioCases[day] += casesPerDay[day]
            scenarioOutbreakSizes += casesPerDay[-1]
            scenarioOutbreakDurations += getOutbreakDurations(casesPerDay, numDays)
        allProportionOutbreaks.append(scenarioProportionOutbreaks)
        allOutbreakSizes.append(scenarioOutbreakSizes)
        # Plot distribution of cases per day for scenario
        title = "Cumulative cases per day for " + scenarioName + " scenario"
        data = list(scenarioCases.values())[1::30]
        labels = list(scenarioCases.keys())[1::30]
        boxplot(data, labels, title, "Day", "Number of infected", 0, 30000)
        # Plot distribution of outbreak sizes for scenario
        histogram(scenarioOutbreakSizes, 100, "Outbreak size distribution for " + scenarioName + " scenario", "Outbreak size", "Frequency")
        histogram(scenarioOutbreakDurations, 100, "Outbreak duration distribution for " + scenarioName + " scenario", "Outbreak duration", "Frequency")

    # Plot proportion of outbreak occurences per ensemble
    boxplot(allProportionOutbreaks, scenarios, "Occurence of outbreaks", "", "Proportion outbreaks", 0, 1)
    # Plot distribution of outbreak sizes
    boxplot(allOutbreakSizes, scenarios, "Outbreak sizes", "", "Total number infected", 0, 30000)

if __name__=="__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("dir", type=str, help="path to directory containing data")
    parser.add_argument("--onlyOutbreaks", type=bool, default=False)
    args = parser.parse_args()
    main(args.dir, ["Random", "AgeDependent", "Clustering"], args.onlyOutbreaks)
