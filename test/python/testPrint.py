import sys
from pystride.Simulation import Simulation

# The callback function
def printInfected(sim, timestep):
    print("Infected at day {}: {}".format(timestep-1, sim.GetSimulator().GetPopulation().GetInfectedCount()))
    sys.stdout.flush()

# Build simulation
simulation = Simulation()
simulation.loadRunConfig("../config/run_default.xml")

# reconfigure some of the default parameters
simulation.runConfig.setParameter("population_file", "pop_antwerp.csv")
simulation.runConfig.setParameter("num_days", 10)
simulation.runConfig.setParameter("output_prefix", "testPrint")

# Register callback
simulation.registerCallback(printInfected)

simulation.run()
