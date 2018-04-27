from .PyController import PyController


class Fork(PyController):
    def __init__(self, name: str, parent):
        super().__init__()
        # Copy configuration from parent
        self.runConfig = parent.runConfig.copy()
        self.runConfig.setParameter("output_prefix", name)

        if isinstance(parent, Fork):
            # Flattened fork
            self.parent = parent.parent
        else:
            self.parent = parent
        self.parent.forks.append(self)
