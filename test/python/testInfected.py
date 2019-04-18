import os
from shutil import rmtree

import pystride
from pystride.Event import EventType
from pystride.PyController import PyController


# Callback to check if number of cases produced by sim is acceptable
def checkNumCases(simulator, event):
    # Target for total number of cases after simulation
    targetCases = 31833
    actualCases = simulator.GetPopulation().GetInfectedCount()
    # Accept a 10% margin of error
    assert (abs(targetCases - actualCases) <= (targetCases * 0.1)),"Expected value is {} - actual value is {}".format(targetCases, actualCases)


# Configure simulation
controller = PyController(config_path="../config/run_default.xml")
controller.runConfig.setParameter("output_prefix", "pytest_Infected/")
controller.runConfig.setParameter("use_install_dirs", "true")

# Clean up leftover of previous failed testrun
if os.path.isdir("testInfected"):
    rmtree("testInfected")

# Register callback
controller.registerCallback(checkNumCases, EventType.AtFinished)

# Let controller run the simulation.
controller.control()
