import argparse
import csv
import matplotlib.pyplot as plt
import os
import xml.etree.ElementTree as ET

MAX_AGE = 99

def getRngSeeds(outputDir, scenarioName):
    seeds = []
    seedsFile = os.path.join(outputDir, scenarioName + "Seeds.csv")
    with open(seedsFile) as csvfile:
        reader = csv.reader(csvfile)
        for row in reader:
            for s in row:
                seeds.append(int(s))
    return seeds

def getTargetSusceptibilityRates(outputDir):
    targetRatesChildTree = ET.parse(os.path.join(outputDir, 'data', 'measles_child_immunity.xml'))
    targetRatesChild = []
    for r in targetRatesChildTree.iter():
        if r.tag not in ['immunity', 'data_source', 'data_manipulation']:
            targetRatesChild.append(float(r.text))
    targetRatesAdultTree = ET.parse(os.path.join(outputDir, 'data', 'measles_adult_immunity.xml'))
    targetRatesAdult = []
    for r in targetRatesAdultTree.iter():
        if r.tag not in ['immunity', 'data_source', 'data_manipulation']:
            targetRatesAdult.append(float(r.text))
    # 1 - immunityRate = susceptibilityRate
    return [1 - (x + y) for x, y in zip(targetRatesChild, targetRatesAdult)]

def getActualSusceptibilityRates(outputDir, scenarioName, seed):
    susceptiblesFile = os.path.join(outputDir, scenarioName + "_" + str(seed), 'susceptibles.csv')
    maxAge = 99
    totalsByAge = [0] * (maxAge + 1)
    susceptiblesByAge = [0] * (maxAge + 1)
    with open(susceptiblesFile) as csvfile:
        reader = csv.DictReader(csvfile)
        for row in reader:
            age = int(float(row['age']))
            isSusceptible = int(row['susceptible'])
            totalsByAge[age] += 1
            if isSusceptible:
                susceptiblesByAge[age] += 1
    return [x / y for x, y in zip(susceptiblesByAge, totalsByAge)]

'''
def createHouseholdConstitutionPlots(outputDir, scenarioNames):
    for scenario in scenarioNames:
        seeds = getRngSeeds(outputDir, scenario)
        for s in seeds[:1]:
            households = {}
            susceptiblesFile = os.path.join(outputDir, scenario + str(s), 'susceptibles.csv')
            with open(susceptiblesFile) as csvfile:
                reader = csv.DictReader(csvfile)
                for row in reader:
                    hhId = int(row['household'])
                    isSusceptible = int(row['susceptible'])
                    if hhId in households:
                        households[hhId].append(isSusceptible)
                    else:
                        households[hhId] = [isSusceptible]
            householdImmunities = []
            for hhId, members in households.items():
                pctImmune = 1 - (sum(members) / len(members))
                householdImmunities.append(pctImmune)
            plt.hist(householdImmunities)
            plt.title(scenario)
            plt.show()
'''

def getActualAvgSusceptibilityRates(outputDir, scenarioName):
    seeds = getRngSeeds(outputDir, scenarioName)
    totalRates = [0] * (MAX_AGE + 1)
    for s in seeds:
        actualRates = getActualSusceptibilityRates(outputDir, scenarioName, s)
        totalRates = [x + y for x,y in zip(totalRates, actualRates)]
    totalRates = [x / len(seeds) for x in totalRates]
    return totalRates

def ageImmunityPlot(outputDir, scenarioNames, scenarioDisplayNames):
    targetSusceptibilityRates = getTargetSusceptibilityRates(outputDir)
    plt.plot(targetSusceptibilityRates, 'bo')
    for scenario in scenarioNames:
        actualRates = getActualAvgSusceptibilityRates(outputDir, scenario)
        plt.plot(actualRates)
    plt.xlim(0, 100)
    plt.xlabel("Age")
    plt.ylim(0,1)
    plt.ylabel("Fraction susceptible")
    plt.legend(scenarioDisplayNames)
    plt.savefig(os.path.join(outputDir, "AgeImmunityPlot.png"))
    plt.clf()

def main(outputDir):
    scenarioNames = ["Scenario1", "Scenario2", "Scenario3", "Scenario4"]
    scenarioDisplayNames = ["Uniform immunity rates +\nno household-based clustering",
                            "Age-dependent immunity rates +\nno household-based clustering",
                            "Uniform immunity rates +\nhousehold-based clustering",
                            "Age-dependent immunity rates +\nhousehold-based clustering"]
    # Plot age-dependent immunity rates for each scenario
    ageImmunityPlot(outputDir, scenarioNames, ["Data"] + scenarioDisplayNames)
    # TODO plot household constitutions

if __name__=="__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("outputDir", type=str, help="Directory containing simulation output files.")
    args = parser.parse_args()
    main(args.outputDir)
