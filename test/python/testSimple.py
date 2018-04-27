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
