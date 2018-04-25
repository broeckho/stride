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

    # TODO getClock()?
    # TODO getConfig() ?

    def getSimulator(self):
        return self.simulator

    def setup(self, run_config):
        self.notifyObservers(Event(EventType.SetupBegin))
        self.stopwatch.start()
        status = True
        self.runConfig = run_config
        # TODO write config to file
        # TODO output prefix?
        print("Setup starting at: " + time.strftime("%d/%m/%Y %H:%M:%S", time.localtime()))

        # TODO build simulator
        #   log start builder
        #   SimulatorBuilder builder(config)
        config_string = self.runConfig.toString()
        print(config_string)
        builder = SimulatorBuilder(config_string)
        #   self.simulator = builder.build()
        #   log end Building

        self.stopwatch.stop()
        self.notifyObservers(Event(EventType.SetupEnd))
        '''
                        print("Setup finished at: " + time.strftime("%d/%m/%Y %H:%M:%S", time.localtime()))
                        # TODO return status?
        '''

    def run(self):
        self.stopwatch.start()
        num_days = int(self.runConfig.getParameter("num_days"))
        # TODO log start?
        # TODO notify observers that sim is about to start
        for day in range(num_days):
            # TODO self.sim->TimeStep()
            # TODO log time stepped
            print("Stepped: timestep " + str(day))
            self.notifyObservers(SteppedEvent(day))

        # TODO notify observers that sim is finished
        self.stopwatch.stop()
        # TODO log finished?
