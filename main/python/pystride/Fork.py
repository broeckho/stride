'''
from .PyController import PyController


class Fork(PyController):
    def __init__(self, name: str, parent):
<<<<<<< HEAD
        super().__init__()
        # Copy configuration from parent
=======
        super().__init__(parent.dataDir)
        # Copy config from parent
>>>>>>> master
        self.runConfig = parent.runConfig.copy()
        self.runConfig.setParameter("output_prefix", name)

        if isinstance(parent, Fork):
            # Flattened fork
            self.parent = parent.parent
        else:
            self.parent = parent
        self.parent.forks.append(self)
    '''
