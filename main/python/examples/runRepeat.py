import pystride
from pystride.Simulation import Simulation

# Set the workspace (default = .)
pystride.workspace = "simulations"

# Configure simulation
simulation = Simulation()
simulation.loadRunConfig("../config/run_default.xml")
simulation.runConfig.setParameter("num_days", 10)

# Create 5 identical forks except for the seed
for index in range(5):
    fork = simulation.fork("instance" + str(index))
    fork.runConfig.setParameter("rng_seed", index)

# Run all forks
simulation.runForks()
