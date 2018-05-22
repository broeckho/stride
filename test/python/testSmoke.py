import pystride
from pystride.PyController import PyController

# Configure simulation
controller = PyController(config_path="../config/run_default.xml")
# Let controller run the simulation
controller.control()
