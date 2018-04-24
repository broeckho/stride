import os
import time

import pystride
from .Config import Config
from .PyRunner import PyRunner

class PyController:
    def __init__(self, config_path=None):
        # self.forks = list()
        self.runner = PyRunner()
        self.timestamp =  time.strftime("%Y%m%d_%H%M%S", time.localtime())
        if config_path != None:
            # Load run and disease config from file
            self.loadRunConfig(config_path)
            # TODO load disease config
        else:
            self.runConfig = Config(root="run")
            self.diseaseConfig = Config(root="disease")

    def loadRunConfig(self, config_path: str):
        self.runConfig = Config(config_path)
        #self.diseaseConfig = Config(os.path.join(self.dataDir, self.runConfig.getParameter("disease_config_file")))
        #self.runConfig.setParameter("output_prefix", self.getLabel())

    def getOutputPrefix(self):
        output_prefix = self.runConfig.getParameter("output_prefix")
        if output_prefix == None:
            return self.timestamp
        return output_prefix

    def getWorkingDirectory(self):
        return pystride.workspace

    def getOutputDirectory(self):
        pass

    '''
        def getOutputDirectory(self):
            return os.path.join(self.getWorkingDirectory(), self.getLabel()+"/")
    '''

    def run(self):
        """
            Run the current simulation
        """
        print("Run starting at " + time.strftime("%d/%m/%Y %H:%M:%S", time.localtime()))
        # TODO create output directory
        # TODO copy data files to output/data?

        # TODO setup runner + execute
        # self.runner.setup(self.runConfig)
        # self.runner.run()
        print("Run ending at " + time.strftime("%d/%m/%Y %H:%M:%S", time.localtime()))

    def runForks(self):
        pass

    def runAll(self):
        pass

'''
        unsigned int      m_max_num_threads; /// Max number  of OpenMP threads.
        std::string       m_output_prefix;   /// Prefix to output (name prefix or prefix dir)
        util::Stopwatch<> m_run_clock;       ///< Stopwatch for timing the computation.
'''

'''
    def __init__(self, dataDir=None):
        if dataDir == None:
            self.dataDir = os.path.join("..", "data")
        else:
            self.dataDir = dataDir

    def loadDiseaseConfig(self, filename: str):
        self.diseaseConfig = Config(filename)
        self.runConfig.setParameter("disease_config_file", os.path.basename(filename))

    def _linkData(self):
        dataDestDir = os.path.join(self.getOutputDirectory(), "data")
        os.makedirs(dataDestDir, exist_ok=True)
        file_params = [
            "population_file",
            "holidays_file",
            "age_contact_matrix_file",
            # TODO disease_config_file?
        ]
        for param in file_params:
            src = os.path.join(self.dataDir, self.runConfig.getParameter(param))
            self.runConfig.setParameter(param, src)
            dst = os.path.join(dataDestDir, os.path.basename(src))
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

from .Fork import Fork'''
