import argparse
import csv
import matplotlib.pyplot as plt
import os

from collections import OrderedDict
from mpl_toolkits.mplot3d import Axes3D

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

def histogram3D(data, xBinSize, yBinSize, title, xLabel, yLabel, zLabel):
    """
    """
    xs = []
    ys = []
    zs = []

    for key, val in data.items():
        if val != 0:
            xs.append(key[0])
            ys.append(key[1])
            zs.append(val)

    fig = plt.figure()
    ax = fig.add_subplot(111, projection="3d")
    ax.set_xlabel(xLabel)
    ax.set_ylabel(yLabel)
    ax.set_zlabel(zLabel)
    ax.set_title(title)
    ax.bar3d(xs, ys, [0] * len(xs), xBinSize, yBinSize, zs, shade=True)
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

def getSizeAndDurationFrequencies(outbreakSizes, outbreakDurations, sizeScale, durationScale):
    frequencies = {}
    for i in range(0, 30001, sizeScale):
        for j in range(0, 366, durationScale):
            frequencies[(i, j)] = 0
    for i in range(len(outbreakSizes)):
        size = outbreakSizes[i] - (outbreakSizes[i] % sizeScale)
        duration = outbreakDurations[i] - (outbreakDurations[i] % durationScale)
        frequencies[(size, duration)] += 1
    return frequencies

def removeNoOutbreaks(casesPerDay, minOutbreakSize):
    totalCases = casesPerDay[-1]
    noOutbreakIds = [i for i in range(len(totalCases)) if totalCases[i] < minOutbreakSize]
    for day in range(len(casesPerDay)):
        casesThisDay = casesPerDay[day]
        casesPerDay[day] = [casesThisDay[i] for i in range(len(casesThisDay)) if i not in noOutbreakIds]
    return casesPerDay

def main(directory, scenarios, onlyOutbreaks):
    numDays = 365
    numEnsembles = 1
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
        # Plot distribution of outbreak durations for scenario
        histogram(scenarioOutbreakDurations, 100, "Outbreak duration distribution for " + scenarioName + " scenario", "Outbreak duration", "Frequency")

        # Plot distribution of sizes + durations
        sizeScale = 1000
        durationScale = 30
        sizeDurationFreqs = getSizeAndDurationFrequencies(scenarioOutbreakSizes, scenarioOutbreakDurations, sizeScale, durationScale)
        histogram3D(sizeDurationFreqs, sizeScale, durationScale, "Distribution of outbreak sizes and durations for " + scenarioName + " scenario", "Outbreak size", "Outbreak duration", "Frequency")
        #histogram3D(scenarioOutbreakSizes, scenarioOutbreakDurations, scenarioName)
    # Plot proportion of outbreak occurences per ensemble
    boxplot(allProportionOutbreaks, scenarios, "Occurence of outbreaks", "", "Proportion outbreaks", 0, 1)
    # Plot distribution of outbreak sizes
    boxplot(allOutbreakSizes, scenarios, "Outbreak sizes", "", "Total number infected", 0, 30000)

if __name__=="__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("dir", type=str, help="path to directory containing data")
    parser.add_argument("--onlyOutbreaks", type=bool, default=False)
    args = parser.parse_args()
    #main(args.dir, ["Random", "AgeDependent", "Clustering"], args.onlyOutbreaks)
    main(args.dir, ["Random"], args.onlyOutbreaks)

'''
'''

def main(directory, numRuns):
    # Plot results of Stan runs
    casesPerDayStan = [[]] * 366
    totalCasesStan = []
    for i in range(5):
        with open(os.path.join(directory, "Random" + str(i) + "_stan_infected.csv")) as csvfile:
            reader = csv.reader(csvfile)
            header = next(reader) # skip header
            casesPerDay = []
            day = 0
            for row in reader:
                dayRes = [int(x) for x in row]
                casesPerDay.append(dayRes)
                casesPerDayStan[day] = casesPerDayStan[day] + dayRes
                day += 1
        totalCasesStan += [x for x in casesPerDay[200]]

    durationsStan = getOutbreakDurations(casesPerDayStan, 200)

    # Plot results of naive runs
    totalCasesNaive = []
    casesPerDayNaive = [[]] * 201
    for i in range(numRuns):
        filename = "NaiveRandom" + str(i) + "_cases.csv"
        cases = []
        with open(os.path.join(directory, filename)) as csvfile:
            reader = csv.reader(csvfile)
            for row in reader:
                cases = [int(x) for x in row]
                for i in range(len(cases)):
                    casesPerDayNaive[i] = casesPerDayNaive[i] + [cases[i]]
        totalCasesNaive.append(cases[200])

    durationsNaive = getOutbreakDurations(casesPerDayNaive, 200)


    #plt.hist(totalCasesNaive, np.logspace(np.log10(1.0),np.log10(50000.0), 50))
    #plt.hist(totalCasesStan, np.logspace(np.log10(1.0),np.log10(50000.0), 50))
    #plt.gca().set_xscale("log")
    histogram(durationsStan, 15, "Using Stan", "", "")
    histogram(durationsNaive, 15, "Naive approach", "", "")

    histogram(totalCasesStan, 20, "Using Stan", "", "Frequency")
    histogram(totalCasesNaive, 20, "Naive approach", "", "Frequency")
'''

import argparse
import csv
import matplotlib.pyplot as plt
import numpy as np
import os

def histogram(data, numBins, xMin, xMax, xLabel, yLabel):
    """
        Create histogram.
    """
    plt.hist(data, bins=numBins, range=(xMin, xMax))
    plt.ylim(0, 300)
    plt.xlabel(xLabel)
    plt.ylabel(yLabel)
    plt.show()

def main(directory, numDays, numEnsembles, ensembleSize):
    # Read stan results
    totalCasesStan = []
    for i in range(numEnsembles):
        filename = "Random" + str(i) + "_stan_infected.csv"
        with open(os.path.join(directory, filename)) as csvfile:
            reader = csv.reader(csvfile)
            casesPerDay = [[]] * 366
            next(reader) # Skip header
            day = 0
            for row in reader:
                casesPerDay[day] = [int(x) for x in row]
                day += 1
            totalCasesStan = totalCasesStan + casesPerDay[numDays - 1]

    # Read naive results
    totalCasesNaive = []
    for i in range(numEnsembles * ensembleSize):
        filename = "NaiveRandom" + str(i) + "_cases.csv"
        with open(os.path.join(directory, filename)) as csvfile:
            reader = csv.reader(csvfile)
            casesPerDay = [int(x) for x in next(reader)]
            totalCasesNaive.append(casesPerDay[numDays - 1])

    minInfected = 0
    maxInfected = max(max(totalCasesNaive), max(totalCasesStan))
    # Plot results
    numBins = [40]
    for b in numBins:
        histogram(totalCasesStan, b, minInfected, maxInfected, "Total number of infected cases after " + str(numDays) + " days", "Frequency")
        histogram(totalCasesNaive, b, minInfected, maxInfected, "Total number of infected cases after " + str(numDays) + " days", "Frequency")



if __name__=="__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("dir", type=str, help="path to directory containing data")
    parser.add_argument("--numDays", type=int, default=366)
    parser.add_argument("--numEnsembles", type=int, default=10)
    parser.add_argument("--ensembleSize", type=int, default=100)

    args = parser.parse_args()
    main(args.dir, args.numDays, args.numEnsembles, args.ensembleSize)

'''
