'''
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
'''

'''
import os
from time import localtime, strftime

import pystride
from pystride.stride.stride import StrideRunner
from .Config import Config
from .SimulationObserver import SimulationObserver
from .LogParser import LogParser


class Simulation:
    def __init__(self, dataDir=None):
        self.forks = list()
        self.runner = StrideRunner()
        self.observer = SimulationObserver(self.runner)
        self.runConfig = Config(root="run")
        self.diseaseConfig = Config(root="disease")
        self.timestamp =  strftime("%Y%m%d_%H%M%S", localtime())
        if dataDir == None:
            self.dataDir = os.path.join("..", "data")
        else:
            self.dataDir = dataDir

    def loadRunConfig(self, filename: str):
        self.runConfig = Config(filename)
        self.diseaseConfig = Config(os.path.join(self.dataDir, self.runConfig.getParameter("disease_config_file")))
        self.runConfig.setParameter('output_prefix', self.getLabel())

    def loadDiseaseConfig(self, filename: str):
        self.diseaseConfig = Config(filename)
        self.runConfig.setParameter("disease_config_file", os.path.basename(filename))

    def getLabel(self):
        label = self.runConfig.getParameter('output_prefix')
        if label == None:
            return self.timestamp
        return label

    def getWorkingDirectory(self):
        return pystride.workspace

    def getOutputDirectory(self):
        return os.path.join(self.getWorkingDirectory(), self.getLabel()+"/")

    def _linkData(self):
        dataDestDir = os.path.join(self.getWorkingDirectory(), "data_sim")
        os.makedirs(dataDestDir, exist_ok=True)
        file_params = [
            "population_file",
            "holidays_file",
            "age_contact_matrix_file",
            # TODO disease_config_file?
        ]
        for param in file_params:
            src = os.path.realpath(os.path.join(self.dataDir,self.runConfig.getParameter(param)))
            dst = os.path.join(dataDestDir, self.runConfig.getParameter(param))
            self.runConfig.setParameter(param, dst)
            if (os.path.isfile(src)) and (not (os.path.isfile(dst))):
                os.symlink(src, dst)

    def _setup(self, linkData=True):
        """
            Create folder in workspace to run simulation.
            Copy config and link to data
        """
        if linkData:
            self._linkData()
        os.makedirs(self.getOutputDirectory(), exist_ok=True)
        # Store disease configuration
        oldDiseaseFile = self.runConfig.getParameter("disease_config_file")[:-4]
        diseaseFile = oldDiseaseFile + "_" + self.getLabel() + ".xml"
        diseasePath = os.path.join(self.getOutputDirectory(), diseaseFile)
        self.diseaseConfig.toFile(diseasePath)
        self.runConfig.setParameter("disease_config_file", diseasePath)
        # Store the run configuration
        configPath = os.path.join(self.getOutputDirectory(), self.getLabel() + ".xml")
        oldLabel = self.getLabel()
        self.runConfig.setParameter("output_prefix", self.getOutputDirectory())
        self.runConfig.toFile(configPath)
        self.runConfig.setParameter('output_prefix', oldLabel)

    def registerCallback(self, callback):
        """ Registers a callback to the simulation. """
        self.observer.RegisterCallback(callback)

    def fork(self, name: str):
        """
            Create a new simulation instance from this one.
            :param str name: the name of the fork.
        """
        f = Fork(name, self)
        return f

    def run(self, trackIndexCase=False):
        """ Run the current simulation. """
        self._setup()

        configPath = os.path.join(self.getOutputDirectory(), self.getLabel() + ".xml")
        try:
            self.runner.Setup(trackIndexCase, configPath)
            self.runner.RegisterObserver(self.observer)
            self.runner.Run()
        except:
            print("Exception while running the simulator. Closing down.")
            exit(1)

    def runForks(self, *args, **kwargs):
        """ Run all forks but not the root simulation. """
        for fork in self.forks:
            fork.run(*args, **kwargs)

    def runAll(self, *args, **kwargs):
        """ Run the root simulation and all forks. """
        self.run(*args, **kwargs)
        self.runForks(*args, **kwargs)

    def __getstate__(self):
        return dict()

    def __setstate__(self, state):
        pass

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

from .Fork import Fork

'''
