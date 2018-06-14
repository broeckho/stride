import argparse
import csv
import matplotlib.pyplot as plt
import os

def main(directory):
    scenarios = ["Random", "AgeDependent", "Clustering"]
    allOutbreaks = []
    allCases = []
    allOutbreakSizes = []
    for scenarioName in scenarios:
        scenarioOutbreaks = []
        scenarioOutbreakSizes = []
        scenarioCases = {}
        for day in range(366):
            scenarioCases[day] = []
        for ensembleId in range(40):
            filename = os.path.join(directory, scenarioName + str(ensembleId) + "_stan_infected.csv")
            cases = []
            with open(filename) as csvfile:
                reader = csv.reader(csvfile)
                next(reader) # Skip header
                day = 0
                for row in reader:
                    cases.append(row)
                    scenarioCases[day] += [int(x) for x in row]
                    day += 1
            totalCases = [int(x) for x in cases[-1]] # Get total cases on last sim day
            scenarioOutbreakSizes += totalCases
            outbreak = [1.0] * len(totalCases)
            for i in range(len(totalCases)):
                if totalCases[i] <= 1:
                    outbreak[i] = 0.0

            pctOutbreaks = sum(outbreak) / len(outbreak)
            scenarioOutbreaks.append(pctOutbreaks)
        allOutbreaks.append(scenarioOutbreaks)
        allOutbreakSizes.append(scenarioOutbreakSizes)
        allCases.append(scenarioCases)
    plt.boxplot(allOutbreaks, labels=scenarios)
    plt.ylim((0,1))
    plt.ylabel("% outbreaks per 40 runs")
    plt.show()

    plt.boxplot(allOutbreakSizes, labels=scenarios)
    plt.ylim((0, 30000))
    plt.ylabel("Outbreak size")
    plt.show()

    for i in range(len(scenarios)):
        plt.boxplot(list(allCases[i].values())[0::30], labels=list(allCases[i].keys())[0::30])
        plt.xlabel("Day")
        plt.ylabel("Number of infected cases")
        plt.ylim((0, 30000))
        plt.title("Cases distribution per day for " + scenarios[i])
        plt.show()


if __name__=="__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("dir", type=str, help="path to directory containing data")
    args = parser.parse_args()
    main(args.dir)
