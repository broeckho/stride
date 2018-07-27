import csv
import matplotlib.pyplot as plt
import os

def plotTargetImmunityProfile():
    pass

'''
def ageDependentImmunityProfile():
    ages = range(100)
    immunityProbs = [1] * 100
    immunityProbs[0:10] = [0.30, 0.95, 0.94, 0.93, 0.91, 0.89, 0.87, 0.87, 0.89, 0.91]
    immunityProbs[10:20] = [0.93, 0.94, 0.95, 0.95, 0.94, 0.92, 0.88, 0.83, 0.81, 0.83]
    immunityProbs[20:30] = [0.84, 0.86, 0.87, 0.88, 0.93, 0.96, 0.97, 0.98, 0.99, 0.99]

    susceptibleProbs = [1 - x for x in immunityProbs]
    plt.plot(ages, susceptibleProbs, 'bo')
    plt.xlabel("Age")
    plt.ylabel("Percent susceptible")
    plt.ylim([0,1])
    plt.show()
'''

def plotAgeImmunityProfile(scenarioName):
    ages = []
    susceptibles = []
    with open(os.path.join(scenarioName, "age_immunity_profile.csv")) as csvfile:
        reader = csv.DictReader(csvfile)
        for row in reader:
            ages.append(int(row["age"]))
            susceptibles.append(float(row["susceptible"]))
    plt.plot(ages, susceptibles, 'bo')
    plt.title("Age-related immunity profile for " + scenarioName + " scenario")
    plt.xlabel("Age")
    plt.ylabel("Fraction susceptible")
    plt.ylim((0, 1))
    plt.show()

def getCasesPerDay(scenarioName):
    timesteps = []
    total_cases = []
    new_cases = []

    with open(os.path.join(scenarioName, "cases.csv")) as csvfile:
        reader = csv.DictReader(csvfile)
        for row in reader:
            timestep = int(row["timestep"])
            cases = int(row["cases"])
            if timestep == 0:
                last = cases
            timesteps.append(timestep)
            total_cases.append(cases)
            new_cases.append(cases - last)
            last = cases
    return (timesteps, total_cases, new_cases)

def plotNewCasesPerDay(scenarios):
    lines = ["b-", "g-", "r-", "y-", "m-"]
    handles = [None] * len(scenarios)
    for i in range(len(scenarios)):
        lineStyle = lines[i]
        scenarioName = scenarios[i]
        (timesteps, totalCases, newCases) = getCasesPerDay(scenarioName)
        handles[i], = plt.plot(timesteps, newCases, lineStyle, label=scenarioName)
    plt.xlabel("Day")
    plt.ylabel("Number of new cases")
    plt.title("New cases per day")
    plt.legend(handles=handles)
    plt.show()

'''
def plotTotalCasesPerDay(scenarios):
    lines = ["b-", "g-", "r-", "y-", "m-"]
    handles = [None] * len(scenarios)
    for i in range(len(scenarios)):
        lineStyle = lines[i]
        scenarioName = scenarios[i]
        (timesteps, totalCases, newCases) = getCasesPerDay(os.path.join(scenarioName, "cases.csv"))
        handles[i], = plt.plot(timesteps, totalCases, lineStyle, label=scenarioName)
    plt.plot(timesteps, [SUSCEPTIBLES_AT_START] * len(timesteps), "k-")
    plt.xlabel("Day")
    plt.ylabel("Total number of cases")
    plt.title("Total number of cases per day\n(black horizontal line = # susceptibles at start)")
    plt.legend(handles=handles)
    plt.show()
'''

def main(scenarioNames):
    for scenario in scenarioNames:
        plotAgeImmunityProfile(scenario)
    plotNewCasesPerDay(scenarioNames)

if __name__=="__main__":
    main(["Random", "AgeDependent", "Clustering"])
