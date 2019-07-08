import sys

import pystride
from pystride.Event import EventType
from pystride.PyController import PyController

def printInfected(simulator, event):
    day = event.timestep
    print("Infected at day {}: {}".format(day, simulator.GetPopulation().GetInfectedCount()))
    sys.stdout.flush()

# Configure Simulation
controller = PyController(config_path="../config/run_default.xml")
controller.runConfig.setParameter("population_file", "pop_antwerp.csv")
controller.runConfig.setParameter("num_days", 10)
controller.runConfig.setParameter("output_prefix", "pytest_Print/")
# Register callback
controller.registerCallback(printInfected, EventType.Stepped)
# Let controller run the simulation
controller.control()
