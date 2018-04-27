import os
import time

import pystride
from .Config import Config
from .PyObserver import PyObserver
from .PyRunner import PyRunner

class PyController:
    def __init__(self, config_path=None, data_dir="../data"):
        self.forks = list()
        self.runner = PyRunner()
        self.dataDir = data_dir
        self.timestamp =  time.strftime("%Y%m%d_%H%M%S", time.localtime())
        self.observer = PyObserver()
        self.runner.registerObserver(self.observer)
        if config_path != None:
            # Load run config from file
            self.loadRunConfig(config_path)
        else:
            self.runConfig = Config(root="run")
        # TODO make disease configuration customisable? But disease_config_file is data file, not config file...

    def loadRunConfig(self, config_path: str):
        self.runConfig = Config(config_path)
        self.runConfig.setParameter("output_prefix", self.getOutputPrefix())

    def getOutputPrefix(self):
        output_prefix = self.runConfig.getParameter("output_prefix")
        if output_prefix == None:
            return self.timestamp

    def linkData(self):
        # TODO is it useful to copy data files into an 'output directory'?
        file_params = [
            "population_file",
            "holidays_file",
            "age_contact_matrix_file",
            "disease_config_file",
        ]
        for param in file_params:
            src = os.path.join(self.dataDir, self.runConfig.getParameter(param))
            self.runConfig.setParameter(param, src)

    def registerCallback(self, callback, event_type):
        self.observer.registerCallback(callback, event_type)

    def fork(self, name: str):
        """
            Create a new simulation instance from this one.
            :param str name: the name of the fork.
        """
        f = Fork(name, self)
        return f

    def run(self):
        """
            Run the current simulation.
        """
        print("PyController starting run at " + time.strftime("%d/%m/%Y %H:%M:%S", time.localtime()))

        # TODO create output directory? No output is generated automatically
        # TODO Save configuration to file?
        #os.makedirs(self.getOutputDirectory(), exist_ok=True)
        self.linkData()

        # Setup runner (build simulator etc)
        self.runner.setup(self.runConfig)
        # Set simulator for PyObserver
        self.observer.setSimulator(self.runner.getSimulator())
        # Run simulation
        self.runner.run()

        print("PyController closing off at " + time.strftime("%d/%m/%Y %H:%M:%S", time.localtime()))

    def runForks(self):
        """ Run all forks but not the root simulation. """
        for fork in self.forks:
            fork.run()

    def runAll(self):
        """ Run the root simulation and all forks. """
        self.run()
        self.runForks()


from .Fork import Fork
