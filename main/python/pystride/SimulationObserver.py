from pystride.stride.stride import SimulatorObserver, Simulator, Population

class SimulationObserver(SimulatorObserver):
    def __init__(self, simulation):
        super().__init__()
        self.callbacks = list()
        self.simulation = simulation

    def RegisterCallback(self, callback):
        self.callbacks.append(callback)

    def Update(self, timestep):
        for c in self.callbacks:
            try:
                c(self.simulation, timestep)
            except Exception as e:
                print(e)
