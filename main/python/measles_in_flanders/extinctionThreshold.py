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

def plotHistogram(xs, filename):
    plt.hist(xs)
    plt.savefig(filename)
    plt.clf()

def plotOutbreakSizeFrequencies(outputDir, scenarioNames, scenarioDisplayNames, numDays):
    allFinalSizes = []
    for scenario in scenarioNames:
        scenarioFinalSizes = []
        seeds = getRngSeeds(outputDir, scenario)
        for s in seeds:
            casesFile = os.path.join(outputDir, scenario + "_" + str(s), 'cases.csv')
            with open(casesFile) as csvfile:
                reader = csv.DictReader(csvfile)
                for row in reader:
                    timestep = int(row["timestep"])
                    if timestep == (numDays - 1):
                        cases = int(row["cases"])
                        allFinalSizes.append(cases)
                        scenarioFinalSizes.append(cases)
        plotHistogram(scenarioFinalSizes, scenario + "_FinalSizeFrequencies.png")
    plotHistogram(allFinalSizes, "All_FinalSizeFrequencies.png")

def main(outputDir, numDays):
    scenarioNames = ["Scenario1", "Scenario2", "Scenario3", "Scenario4"]
    scenarioDisplayNames = ["Uniform immunity rates +\nno household-based clustering",
                            "Age-dependent immunity rates +\nno household-based clustering",
                            "Uniform immunity rates +\nhousehold-based clustering",
                            "Age-dependent immunity rates +\nhousehold-based clustering"]
    # Final size frequencies -> threshold for extinction
    plotOutbreakSizeFrequencies(outputDir, scenarioNames, scenarioDisplayNames, numDays)

if __name__=="__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("outputDir", type=str, help="Directory containing simulation output files.")
    parser.add_argument("--numDays", type=int, default=365, help="Number of simulation days for which to make plots.")
    args = parser.parse_args()
    main(args.outputDir, args.numDays)
