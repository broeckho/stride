import csv
import matplotlib.pyplot as plt
import os

from pystride.Event import Event, EventType
from pystride.PyController import PyController

def trackCases(simulator, event):
    """
        Callback function to track cumulative cases
        after each time-step.
    """
    outputPrefix = simulator.GetConfigValue("run.output_prefix")
    timestep = event.timestep
    cases = simulator.GetPopulation().GetInfectedCount()
    with open(os.path.join(outputPrefix, "cases.csv"), "a") as csvfile:
        fieldnames = ["timestep", "cases"]
        writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
        if timestep == 0:
            writer.writeheader()
        writer.writerow({"timestep": timestep, "cases": cases})

def plotNewCases(outputPrefix, immunityLevels):
    """
        Plot new cases per day for a list of vaccination levels.
    """
    for v in immunityLevels:
        days = []
        newCasesPerDay = []
        prevCumulativeCases = 0 # Keep track of how many cases have been recorded until current time-step
        with open(os.path.join(outputPrefix + "_" + str(v), "cases.csv")) as csvfile:
            reader = csv.DictReader(csvfile)
            for row in reader:
                days.append(int(row["timestep"]))
                cumulativeCases = int(row["cases"])
                newCasesPerDay.append(cumulativeCases - prevCumulativeCases)
                prevCumulativeCases = cumulativeCases
        plt.plot(days, newCasesPerDay)
        plt.xlabel("Simulation day")
        plt.ylabel("New cases per day")
        plt.legend(str(v)+"% immunity")
        plt.show()

def runSimulation(outputPrefix, immunity_rate):
    # Set up simulator
    control = PyController(data_dir="data")
    # Load configuration from file
    control.loadRunConfig(os.path.join("config", "outbreak_2019_estimates.xml"))
    # Set some parameters
    control.runConfig.setParameter("output_prefix", outputPrefix+"_"+str(immunity_rate))
    control.runConfig.setParameter("immunity_rate", immunity_rate)
    control.runConfig.setParameter("vaccine_profile", None)
    control.registerCallback(trackCases, EventType.Stepped)
    # Run simulation
    control.control()

def main():
    outputPrefix = "ImmunityLevel"
    immunityLevels = [95,96,97,98,99]
    # Run simulations
    for level in immunityLevels:
        if (os.path.exists(os.path.join(outputPrefix+"_"+str(level), "cases.csv"))):
            open(os.path.join(outputPrefix+"_"+str(level), "cases.csv"), 'w').close() #TODO: cleanup
        runSimulation(outputPrefix, level)
    # Post-processing
    plotNewCases(outputPrefix, immunityLevels)

if __name__=="__main__":
    main()
