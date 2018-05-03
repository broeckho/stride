from .PyController import PyController

class Fork(PyController):
    def __init__(self, name: str, parent):
        super().__init__(data_dir=parent.dataDir)
        # Copy configuration from parent
        self.runConfig = parent.runConfig.copy()
        self.runConfig.setParameter("output_prefix", name)
        if isinstance(parent, Fork):
            self.parent = parent.parent # Flattened Fork
        else:
            self.parent = parent
        self.parent.forks.append(self)

# TODO working directory = parent directory ?
