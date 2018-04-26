from .Observer import Observer

class PyObserver(Observer):
    def __init__(self, simulator=None):
        self.simulator = simulator
        self.callbacks = {}
        for name in EvenType:
            self.callbacks[name] = []

    def registerCallback(self, callback, event_type):
        self.callbacks[event_type].append(callback)

    def update(self, event):
        for callback in self.callbacks[event.event_type]:
            try:
                callback(simulator)
            except Exception as e:
                print(e)
