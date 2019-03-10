#################################################################
# Script to obtain the infection count for a range of R0 values #
#################################################################

import pystride
import time
import os
import csv
import sys
import itertools
import matplotlib.pyplot as plt

from pystride.Event import Event, EventType
from pystride.PyRunner import PyRunner
from pystride.PyController import PyController

import xml.etree.ElementTree as ET

data = []

def count(simulator, event):
    timestep = event.timestep
    cases = simulator.GetPopulation().GetInfectedCount()
    data_dict = data[len(data)-1]
    if timestep in data_dict:
        data_dict[timestep] += cases
    else:
        data_dict[timestep] = cases

def save(prefixes):
    """
        Callback function to track cumulative cases
        after each time-step.
    """
    for i in range(0, len(prefixes)):
        with open(prefixes[i], "w") as csvfile:
            fieldnames = ["timestep", "cases"]
            writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
            writer.writeheader()
            for key, value in data[i].items():
                print(key)
                print(value)
                writer.writerow({"timestep": key, "cases": value})

def plotNewCases(outputPrefix, immunities):
    """
        Plot new cases per day for a list of vaccination levels.
    """
    legend = []
    for v in immunities:
        legend.append(str(round(v*100,1)) + "% immunity")
        days = []
        newCasesPerDay = []
        prevCumulativeCases = 0 # Keep track of how many cases have been recorded until current time-step
        with open(os.path.join(outputPrefix + "_" + str(v), "cases.csv")) as csvfile:
            reader = csv.DictReader(csvfile)
            for row in reader:
                days.append(int(row["timestep"]))
                cumulativeCases = int(row["cases"])/5
                newCasesPerDay.append(cumulativeCases - prevCumulativeCases)
                prevCumulativeCases = cumulativeCases
        plt.plot(days, newCasesPerDay)
    plt.xlabel("Simulation day")
    plt.ylabel("New cases per day")
    plt.legend(legend)
    plt.savefig(os.path.join(outputPrefix + "_plots", sys.argv[4]))
    plt.show()

def runSimulation(immunity, outputPrefix):
	# Configure simulation
	controller = PyController(data_dir="data")
	controller.loadRunConfig(os.path.join("config", "outbreak_2019_estimates.xml"))
	controller.runConfig.setParameter("output_prefix", outputPrefix + "_" + str(immunity))
	controller.runConfig.setParameter("immunity_profile", "Random")	
	controller.runConfig.setParameter("immunity_rate", immunity)
	controller.runConfig.setParameter("vaccine_profile", "None")
	controller.registerCallback(count, EventType.Stepped)
	controller.control()


def main():
	prefixes = []
	if not os.path.exists("SimulationResults"):
		os.mkdir("SimulationResults");
	else:
		for immunity in range(int(sys.argv[1]), int(sys.argv[2])+1, int(sys.argv[3])):
			immunity = immunity/100
			path = "SimulationResults/immunity_test_" + str(immunity) + "/cases.csv"
			if os.path.exists(path):
				os.remove(path)
			prefixes.append(path)
	if not os.path.exists("SimulationResults/immunity_test_plots"):
		os.mkdir("SimulationResults/immunity_test_plots")
	outputPrefix = "SimulationResults/immunity_test"
	t_start = time.mktime(time.localtime())
	immunitylevels = []
	for immunity in range(int(sys.argv[1]), int(sys.argv[2])+1, int(sys.argv[3])):
		data.append(dict())
		immunity = immunity/100
		for _ in range(0,5):
			runSimulation(immunity, outputPrefix)
		immunitylevels.append(immunity)
	save(prefixes)
	plotNewCases(outputPrefix, immunitylevels)
	t_elapsed = time.mktime(time.localtime()) - t_start
	print("Total time elapsed: " + str(round(t_elapsed)) + " seconds")

if __name__=="__main__":
    main()

