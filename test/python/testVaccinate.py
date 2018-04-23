import sys
from pystride.Simulation import Simulation

status = True

def vaccinate(sim, timestep):
    if timestep == 20:
        pop = sim.GetSimulator().GetPopulation()
        for pIndex in range(pop.size()):
            if pop[pIndex].GetHealth().IsSusceptible():
                pop[pIndex].GetHealth().SetImmune()

def check(sim, timestep):
    timestop = timestep
    if timestep == 20:
        check.value = getattr(check, 'value', 0)
        check.value = sim.GetSimulator().GetPopulation().GetInfectedCount()
    if timestep > 20:
        if sim.GetSimulator().GetPopulation().GetInfectedCount() != check.value:
            print("Infection count still changing on day ", timestep)
            sim.Stop()
            status = False

# Build simulation
simulation = Simulation()
simulation.loadRunConfig("../config/run_default.xml")

# Register the callbacks
simulation.registerCallback(vaccinate)
simulation.registerCallback(check)

simulation.run()
if not status:
    print("Vaccination test failed.")
    sys.exit(1)