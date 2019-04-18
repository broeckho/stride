import pystride
from pystride.PyController import PyController

# Configure simulation
controller = PyController(config_path="../config/run_default.xml")
controller.runConfig.setParameter("output_prefix", "pytest_Smoke/")
# Let controller run the simulation
controller.control()
