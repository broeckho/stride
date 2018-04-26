import time

from pystride.stride.stride import Simulator, SimulatorBuilder

from .Event import EventType, Event, SteppedEvent
from .Stopwatch import Stopwatch
from .Subject import Subject


class PyRunner(Subject):
    """
            Class responsible for the actual simulation loop.
            Functions as the 'model' in the MVC pattern.
    """
    def __init__(self):
        super(PyRunner, self).__init__()
        self.stopwatch = Stopwatch("run_clock")
        self.simulator = None
        self.runConfig = None

    def getClock(self):
        return self.stopwatch

    def getConfig(self):
        return self.runConfig

    def getSimulator(self):
        return self.simulator

    def setup(self, run_config):
        self.notifyObservers(Event(EventType.SetupBegin))
        self.stopwatch.start()
        status = True
        self.runConfig = run_config
        # TODO write config to file?
        # TODO output prefix?
        print("Setup starting at: " + time.strftime("%d/%m/%Y %H:%M:%S", time.localtime()))

        print("Starting simulator build")
        builder = SimulatorBuilder(self.runConfig.toString())
        self.simulator = builder.Build()
        print("Simulator build done")

        self.stopwatch.stop()
        self.notifyObservers(Event(EventType.SetupEnd))
        print("Setup finished at: " + time.strftime("%d/%m/%Y %H:%M:%S", time.localtime()))
        # TODO return status?

    def run(self):
        self.stopwatch.start()
        num_days = int(self.runConfig.getParameter("num_days"))
        # TODO log start?
        self.notifyObservers(Event(EventType.AtStart))
        for day in range(num_days):
            self.simulator.TimeStep()
            print("Stepped: timestep " + str(day))
            self.notifyObservers(SteppedEvent(day))

        self.notifyObservers(Event(EventType.Finished))
        self.stopwatch.stop()
        # TODO log finished?
