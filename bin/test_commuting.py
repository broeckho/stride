#################################################################
# Script to obtain the infection count for a range of R0 values #
#################################################################

import pystride
import time
import os
import csv
import sys
import matplotlib.pyplot as plt

from pystride.Event import Event, EventType
from pystride.PyRunner import PyRunner
from pystride.PyController import PyController

import xml.etree.ElementTree as ET

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

def plotNewCases(outputPrefix, vaccinationLevels):
    """
        Plot new cases per day for a list of vaccination levels.
    """
    legend = []
    for v in vaccinationLevels:
        legend.append(str(round(v*100,1)) + "% commuters")
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
    plt.legend(legend)
    plt.savefig(os.path.join(outputPrefix + "_plots", sys.argv[1]))
    plt.show()

def runSimulation(comuter_fraction, outputPrefix):
	tree = ET.parse(os.path.join("config", 'run_generate_default.xml'))
	geopop = tree.getroot().find("geopop_gen")
	comuters = geopop.find("fraction_workplace_commuters")
	comuters.text = str(comuter_fraction)
	tree.write(os.path.join("config", "run_generate_default_temp.xml"))

	# Configure simulation
	controller = PyController(data_dir="data")
	controller.loadRunConfig(os.path.join("config", "run_generate_default_temp.xml"))
	controller.runConfig.setParameter("num_days", "400")
	controller.runConfig.setParameter("output_prefix", outputPrefix + "_" + str(comuter_fraction))
	controller.runConfig.setParameter("seeding_rate", 0.00000334)
	controller.registerCallback(trackCases, EventType.Stepped)
	controller.control()


def main():
	if not os.path.exists("SimulationResults"):
		os.mkdir("SimulationResults");
	else:
		for comuters in range(30, 71, 10):
			comuters = comuters/100
			path = "SimulationResults/commuting_test_" + str(comuters) + "/cases.csv"
			if os.path.exists(path):
				os.remove(path)
	if not os.path.exists("SimulationResults/commuting_test_plots"):
		os.mkdir("SimulationResults/commuting_test_plots")
	outputPrefix = "SimulationResults/commuting_test"
	t_start = time.mktime(time.localtime())
	comuterLevels = []
	for comuters in range(30, 71, 10):
		comuters = comuters/100
		runSimulation(comuters, outputPrefix)
		comuterLevels.append(comuters)
	plotNewCases(outputPrefix, comuterLevels)
	t_elapsed = time.mktime(time.localtime()) - t_start
	print("Total time elapsed: " + str(round(t_elapsed)) + " seconds")

if __name__=="__main__":
    main()

