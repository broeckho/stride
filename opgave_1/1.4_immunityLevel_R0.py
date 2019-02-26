import csv
import matplotlib.pyplot as plt
import os
import random

from pystride.Event import Event, EventType
from pystride.PyController import PyController

accumulator =[]

def outputDirectory(immunity_rate, R0):
    return "ImmunityLevel_" + str(immunity_rate)+ "_" + str(R0)

def trackCases(simulator, event):
    """
        Callback function to track cumulative cases
        after each time-step.
    """
    global accumulator
    cases = simulator.GetPopulation().GetInfectedCount()
    timestep = event.timestep
    if len(accumulator) <= timestep:
        # first simulation
        accumulator.append(cases)
    else:
        accumulator[timestep] += cases
    

def emptyAccumulator(immunity_rate, R0, nrSimulations):
    global accumulator
    
    cases = [accumulator[i]/nrSimulations for i in range(len(accumulator))]
    outputPath = outputDirectory(immunity_rate, R0)
    with open(os.path.join(outputPath, "cases.csv"), "w") as csvfile:
        fieldnames = ["timestep", "cases"]
        writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
        writer.writeheader()
        for i in range(len(cases)):
            writer.writerow({"timestep": i, "cases": cases[i]})
            
    accumulator = []

def plotNewCases(immunityLevels, R0Values):
    """
        Plot new cases per day for a list of immunity levels and R0 values.
    """
    for v in immunityLevels:
        for R0 in R0Values:
            days = []
            newCasesPerDay = []
            prevCumulativeCases = 0 # Keep track of how many cases have been recorded until current time-step
            outputPath = outputDirectory(v, R0)
            with open(os.path.join(outputPath, "cases.csv")) as csvfile:
                reader = csv.DictReader(csvfile)
                for row in reader:
                    days.append(int(row["timestep"]))
                    cumulativeCases = float(row["cases"])
                    newCasesPerDay.append(cumulativeCases - prevCumulativeCases)
                    prevCumulativeCases = cumulativeCases
            plt.plot(days, newCasesPerDay, label=str(v)+"% immunity, R0 = " + str(R0))
            plt.xlabel("Simulation day")
            plt.ylabel("New cases per day")
            plt.legend()
            plt.savefig(outputPath+"/graph.png")
            plt.clf()

def runSimulation(immunity_rate, R0):
    # Set up simulator
    control = PyController(data_dir="data")
    # Load configuration from file
    control.loadRunConfig(os.path.join("config", "outbreak_2019_estimates.xml"))
    # Set some parameters
    control.runConfig.setParameter("output_prefix", outputDirectory(immunity_rate, R0))
    control.runConfig.setParameter("immunity_profile", "Random")
    control.runConfig.setParameter("immunity_rate", immunity_rate/100)
    control.runConfig.setParameter("rng_seed", random.randint(0,100000000))
    control.runConfig.setParameter("vaccine_profile", None)
    control.runConfig.setParameter("r0", R0)
    control.registerCallback(trackCases, EventType.Stepped)
    # Run simulation
    control.control()

def main():
    #Values in percent
    minImmunityLevel = 65
    maxImmunityLevel = 76
    step = 1
    immunityLevels = list(range(minImmunityLevel, maxImmunityLevel, step))

    minR0 = 12
    maxR0 = 19
    R0step = 1
    R0Values = list(range(minR0, maxR0, R0step))

    nrSimulations = 5

    global accumulator

    # Run simulations
    for level in immunityLevels:
        for R0 in R0Values:
            for index in range(nrSimulations):
                runSimulation(level, R0)
            emptyAccumulator(level, R0, nrSimulations)
    # Post-processing
    plotNewCases(immunityLevels, R0Values)

if __name__=="__main__":
    main()
