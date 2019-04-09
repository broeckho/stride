import os
import time

import pystride
from .Config import Config
from .PyObserver import PyObserver
from .PyRunner import PyRunner

class PyController:
    def __init__(self, config_path=None, data_dir="../data"):
        self.forks = list()
        self.dataDir = data_dir
        self.timestamp = time.strftime("%Y%m%d_%H%M%S", time.localtime())

        self.runner = PyRunner()
        self.observer = PyObserver()
        self.runner.registerObserver(self.observer)

        if config_path != None:
            # Load run config from file
            self.loadRunConfig(config_path)
        else:
            self.runConfig = Config(root="run")

    def loadRunConfig(self, config_path):
        self.runConfig = Config(config_path)
        self.runConfig.setParameter("output_prefix", self.getOutputPrefix())

    def getOutputPrefix(self):
        output_prefix = self.runConfig.getParameter("output_prefix")
        if output_prefix == None:
            return self.timestamp+"/"
        return output_prefix

    def getWorkingDirectory(self):
        return pystride.workspace

    def getOutputDirectory(self):
        return os.path.join(self.getWorkingDirectory(), self.getOutputPrefix())

    def linkData(self):
        file_params = [
            "population_file",
            "holidays_file",
            "age_contact_matrix_file",
            "disease_config_file",
        ]
        for param in file_params:
            src = os.path.join(self.dataDir, os.path.basename(self.runConfig.getParameter(param)))
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

    def control(self):
        """
            Run the current simulation.
        """
        print("PyController starting run at " + time.strftime("%d/%m/%Y %H:%M:%S", time.localtime()))

        # Set correct links to data files in configuration
        self.linkData()
        # Create output directory and write configuration to file
        os.makedirs(self.getOutputPrefix(), exist_ok=True)
        self.runConfig.toFile(os.path.join(self.getOutputPrefix(), "config.xml"))

        self.runner.setup(self.runConfig)
        self.observer.setSimulator(self.runner.getSimulator())
        self.runner.run()
        print("PyController closing off at " + time.strftime("%d/%m/%Y %H:%M:%S", time.localtime()))

    def controlForks(self):
        """ Run all forks but not the root simulation. """
        for fork in self.forks:
            fork.control()

    def controlAll(self):
        """ Run the root simulation and all forks. """
        self.control()
        self.controlForks()

'''
    def aggregateForkOutput(self):
        summary_file  = open(os.path.join(self.getOutputDirectory(), self.getLabel()+ '_summary.csv'), 'w')
        cases_file    = open(os.path.join(self.getOutputDirectory(), self.getLabel()+ '_cases.csv'), 'w')
        is_first = True

        parser = LogParser()

        for fork in self.forks:
            if is_first:
                summary_file.write(open(os.path.join(fork.getOutputDirectory(),'summary.csv'),'r').read())
                is_first = False
            else:
                fork_summary_lines = open(os.path.join(fork.getOutputDirectory(), 'summary.csv'), 'r').readlines()
                for line in fork_summary_lines[1:]:
                    summary_file.write(line)

            cases_file.write(open(os.path.join(fork.getOutputDirectory(), 'cases.csv'), 'r').read())
            parser.run(fork.getOutputDirectory())

        summary_file.close()
        cases_file.close()
'''

from .Fork import Fork
