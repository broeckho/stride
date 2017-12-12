import pystride

from pystride.Simulation import Simulation

pystride.workspace = "simulations"

def checkFinished(sim, timestep):
    pop = sim.GetSimulator().GetPopulation()
    infected = pop.GetInfectedCount()
    print("Amount infected: ", infected)
    if infected >= 4000:
        sim.Stop()


# Configure simulation
simulation = Simulation()
simulation.loadRunConfig("config/run_default.xml")
simulation.runConfig.setParameter("generate_person_file", 0)
simulation.registerCallback(checkFinished)

simulation.run()
