import pystride
from pystride.PyController import PyController

# Set the workspace (default = .)
pystride.workspace = "pytest_Forker"

# Configure simulation
controller = PyController(config_path="../config/run_default.xml")
controller.runConfig.setParameter("output_prefix", "forks/")
controller.runConfig.setParameter("num_days", 2)

# Create 5 identical forks except for the seed
for index in range(3):
    fork = controller.fork("instance" + str(index))
    fork.runConfig.setParameter("rng_seed", index)

# Run all forks
controller.controlForks()
