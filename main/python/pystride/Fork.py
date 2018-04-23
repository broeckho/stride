import os

import pystride

from .Simulation import Simulation

class Fork(Simulation):
    def __init__(self, name: str, parent):
        super().__init__(parent.dataDir)
        # Copy config from parent
        self.runConfig = parent.runConfig.copy()
        self.runConfig.setParameter('output_prefix', name)
        self.diseaseConfig = parent.diseaseConfig.copy()

        if isinstance(parent, Fork):
            self.parent = parent.parent # Flattened fork
        else:
            self.parent = parent
        self.parent.forks.append(self)

    def getWorkingDirectory(self):
        return os.path.join(pystride.workspace, self.parent.getLabel())

    def __getstate__(self):
        return dict()

    def __setstate__(self, state):
        pass
