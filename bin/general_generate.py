#################################################################
# Script to perform a given amount of simmulations for given    #
# parameters                                                    #
# usage: python3.X general_generate.py start end step           #
#           with start, end and step being the borders          #
#           and steps for the parameter value                   #
#################################################################

import pystride
import time
import os
import csv
import sys
import re
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


def plotNewCases(outputPrefix, levels):
    """
        Plot new cases per day for a list of vaccination levels.
    """
    legend = []
    for v in levels:
        legend.append(str(round(v*100, 1)) + "% {}".format(param))
        days = [i for i in range(0, sim_days)]
        newCasesPerDay = [0] * sim_days
        prevCumulativeCases = 0
        with open(os.path.join(outputPrefix + "_" + str(v), "cases.csv")) as csvfile:
            reader = csv.DictReader(csvfile)
            day_index = 0
            for row in reader:
                if row["timestep"] == "timestep":
                    day_index = 0
                    prevCumulativeCases = 0
                    continue
                cumulativeCases = int(row["cases"])
                newCasesPerDay[day_index] += cumulativeCases - prevCumulativeCases
                prevCumulativeCases = cumulativeCases
                day_index += 1
        newCasesPerDay = [case/sim_days for case in newCasesPerDay]
        plt.plot(days, newCasesPerDay)
    plt.xlabel("Simulation day")
    plt.ylabel("New cases per day")
    plt.legend(legend)
    plt.savefig(os.path.join(outputPrefix + "_plots", param + "_{0}_{1}_{2}".format(sys.argv[1], sys.argv[2], sys.argv[3])))
    plt.show()


def runSimulation(level, outputPrefix):
    tree = ET.parse(os.path.join("config", config))
    geopop = tree.getroot().find("geopop_gen")
    comuters = geopop.find(param)
    comuters.text = str(level)
    tree.write(os.path.join("config", config))

    for _ in range(0, runs):
        controller = PyController(data_dir="data")
        controller.loadRunConfig(os.path.join("config", config))
        controller.runConfig.setParameter("num_days", sim_days)
        controller.runConfig.setParameter("output_prefix", outputPrefix + "_" + str(level))
        controller.runConfig.setParameter("seeding_rate", 0.00000334)
        controller.registerCallback(trackCases, EventType.Stepped)
        controller.control()


def main():
    if not os.path.exists("SimulationResults"):
        os.mkdir("SimulationResults")
    else:
        for temp in range(int(sys.argv[1]), int(sys.argv[2])+1, int(sys.argv[3])):
            if percentage:
                temp = temp/100
            path = "SimulationResults/{0}_{1}/cases.csv".format(param, temp)
            if os.path.exists(path):
                os.remove(path)
    if not os.path.exists("SimulationResults/{0}_plots".format(param)):
        os.mkdir("SimulationResults/{0}_plots".format(param))
    outputPrefix = "SimulationResults/{0}".format(param)
    t_start = time.mktime(time.localtime())
    levels = []
    for level in range(int(sys.argv[1]), int(sys.argv[2])+1, int(sys.argv[3])):
        if percentage:
            level = level/100
        runSimulation(level, outputPrefix)
        levels.append(level)
    plotNewCases(outputPrefix, levels)
    t_elapsed = time.mktime(time.localtime()) - t_start
    print("Total time elapsed: " + str(round(t_elapsed)) + " seconds")


# the parameter that varies
param = "fraction_workplace_commuters"

# the used config file
config = "run_generate_default_temp.xml"

# the number of simulations
runs = 50

# the number of days per simulation
sim_days = 500

# is the parameter a percentage (for loops can only step with whole numbers)
percentage = True


if __name__=="__main__":
    main()
