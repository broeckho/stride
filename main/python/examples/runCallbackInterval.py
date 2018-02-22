from pystride.Simulation import Simulation

# The callback function
def printInfected(sim, timestep):
    if (timestep % 2)  == 0:
        print("Amount infected at timestep {}: {}".format(
            timestep, sim.GetSimulator().GetPopulation().GetInfectedCount()))

# Build simulation
simulation = Simulation()
simulation.loadRunConfig("../config/run_default.xml")

# Register callback
simulation.registerCallback(printInfected)

simulation.run()
