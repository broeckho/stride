import os
from time import localtime, strftime

import pystride
from pystride.stride.stride import StrideRunner
from .Config import Config
from .SimulationObserver import SimulationObserver

class Simulation:
    def __init__(self):
        self.forks = list()
        self.runner = StrideRunner()
        self.observer = SimulationObserver(self.runner)
        self.runConfig = Config(root="run")
        self.diseaseConfig = Config(root="disease")
        self.timestamp =  strftime("%Y%m%d_%H%M%S", localtime())

    def loadRunConfig(self, filename: str):
        self.runConfig = Config(filename)
        self.diseaseConfig = Config(self.runConfig.getParameter("disease_config_file"))
        self.runConfig.setParameter('output_prefix', self.getLabel())

    def loadDiseaseConfig(self, filename: str):
        self.diseaseConfig = Config(filename)
        self.runConfig.setParameter("disease_config_file", filename)

    def getLabel(self):
        label = self.runConfig.getParameter('output_prefix')
        if label == None:
            return self.timestamp
        return label

    def getWorkingDirectory(self):
        return pystride.workspace

    def getOutputDirectory(self):
        return os.path.join(self.getWorkingDirectory(), self.getLabel())

    def _linkData(self):
        dataDir = os.path.join(self.getOutputDirectory(), "data")
        os.makedirs(dataDir, exist_ok=True)
        files = [
            self.runConfig.getParameter("population_file"),
            self.runConfig.getParameter("holidays_file"),
            self.runConfig.getParameter("age_contact_matrix_file"),
        ]
        for src in files:
            dst = os.path.join(dataDir, os.path.basename(src))
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
        self._setup()
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

from .Fork import Fork
