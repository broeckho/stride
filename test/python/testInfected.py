#############################################
#           Test Python environment         #
#############################################

import pystride
from pystride.PyController import PyController
'''
import csv
import os
from shutil import rmtree

# Test output and target
target = 118925
summaryFile = os.path.join("testSimple", "summar.csv")

def summary(sim, timestep):
    if timestep == int(simulation.runConfig.getParameter("num_days")) - 1:
        numDays = timestep + 1
        pop = sim.GetSimulator().GetPopulation()
        popSize = pop.size()
        numInfected = pop.GetInfectedCount()
        with open(summaryFile, 'w') as csvfile:
            fieldNames = ['pop_size', 'num_days','num_infected']
            writer = csv.DictWriter(csvfile, fieldNames)
            writer.writeheader()
            writer.writerow({'pop_size': popSize, 'num_days' : numDays, 'num_infected' : numInfected})

# Configure simulation
simulation = Simulation()
simulation.loadRunConfig("../config/run_default.xml")
simulation.runConfig.setParameter("output_prefix", "testSimple")
simulation.runConfig.setParameter("use_install_dirs", "true")

# Clean up leftover of previous failed  testrun
if os.path.isdir("testSimple"):
    rmtree("testSimple")

# Register the "vaccinate" callback
simulation.registerCallback(summary)

# Run the simulation
simulation.run()
# Check test against target
with open(summaryFile) as csvfile:
    reader = csv.DictReader(csvfile)
    infected = int((next(reader))['num_infected'])
    print("Target: {} - actual value: {}".format(target, infected))
    # Accept a 10% margin of error
    assert (abs(target - infected) <= (target * 0.1)),"Test fails."
'''



'''
#############################################
#           Test Python environment         #
#############################################

import os
from shutil import rmtree

import pystride
from pystride.Event import EventType
from pystride.PyController import PyController


def checkNumCases(simulator):
    """
    """
    targetCases = 118925
    actualCases = simulator.GetPopulation().GetInfectedCount()
    # Accept a 10% margin of error
    assert (abs(targetCases - actualCases) <= (targetCases * 0.1)),"Expected value is {} - actual value is {}".format(targetCases, actualCases)


# Configure simulation
sim = PyController("../config/run_default.xml")
sim.runConfig.setParameter("output_prefix", "testSimple")
sim.runConfig.setParameter("use_install_dirs", "true")

# Register callback function to check number of cases at
# end of simulation
sim.registerCallback(checkNumCases, EventType.Finished)

# Clean up leftover of previous (failed) testrun
if os.path.isdir("testSimple"):
    rmtree("testSimple")

# Run the simulation
sim.run()
'''
