import sys

import pystride
from pystride.Event import EventType
from pystride.PyController import PyController


def printInfected(simulator, event):
    day = event.timestep
    print("Infected at day {}: {}".format(day, simulator.GetPopulation().GetInfectedCount()))
    sys.stdout.flush()

# Configure Simulation
sim = PyController(config_path="../config/run_default.xml")
sim.runConfig.setParameter("population_file", "pop_antwerp.csv")
sim.runConfig.setParameter("num_days", 10)
sim.runConfig.setParameter("output_prefix", "testPrint")

# Register callback
sim.registerCallback(printInfected, EventType.Stepped)
# Run simulation
sim.run()
