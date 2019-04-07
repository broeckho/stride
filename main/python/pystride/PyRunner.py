import time

from pystride.stride.pystride import Sim, CreateSim

from .Event import EventType, Event, SteppedEvent
from .Stopwatch import Stopwatch
from .Subject import Subject

class PyRunner(Subject):
    """
        Class responsible for driving the simulation loop.
        Functions as the 'model' in the MVC pattern.
    """
    def __init__(self):
        super().__init__()
        self.stopwatch = Stopwatch("run_clock")
        self.simulator = None
        self.runConfig = None

    def getClock(self):
        return self.stopwatch

    def getConfig(self):
        return self.runConfig

    def setConfig(self, run_config):
        self.runConfig = run_config

    def getSimulator(self):
        return self.simulator

    def setup(self, run_config):
        self.notifyObservers(Event(EventType.SetupBegin))
        self.stopwatch.start()
        self.runConfig = run_config

        self.simulator = CreateSim(self.runConfig.toString())
        self.stopwatch.stop()
        self.notifyObservers(Event(EventType.SetupEnd))

    def run(self):
        self.stopwatch.start()
        num_days = int(self.runConfig.getParameter("num_days"))
        print("Beginning simulation run.")
        self.notifyObservers(Event(EventType.AtStart))
        # Actual simulation loop
        for day in range(num_days):
            self.simulator.TimeStep()
            self.notifyObservers(SteppedEvent(day))
        self.notifyObservers(Event(EventType.AtFinished))
        self.stopwatch.stop()
        print("Simulation run finished after {} seconds.".format(self.stopwatch.get()))
