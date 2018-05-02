from .Event import EventType
from .Observer import Observer

class PyObserver(Observer):
    def __init__(self):
        self.simulator = None
        self.callbacks = {}
        for name in EventType:
            self.callbacks[name] = []

    def setSimulator(self, simulator):
        self.simulator = simulator

    def registerCallback(self, callback, event_type):
        self.callbacks[event_type].append(callback)

    def update(self, event):
        for callback in self.callbacks[event.event_type]:
            try:
                callback(self.simulator, event)
            except Exception as e:
                print(e)
