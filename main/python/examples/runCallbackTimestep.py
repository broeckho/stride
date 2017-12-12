from pystride.Simulation import Simulation

def vaccinate(sim, timestep):
    if timestep == 20:
        print("Vaccinating everyone who is not yet infected")
        pop = sim.GetSimulator().GetPopulation()
        for pIndex in range(pop.size()):
            if pop[pIndex].GetHealth().IsSusceptible():
                pop[pIndex].GetHealth().SetImmune()

# Build simulation
simulation = Simulation()
simulation.loadRunConfig("config/run_default.xml")

# Register the "vaccinate" callback
simulation.registerCallback(vaccinate)

simulation.run()
