class Subject:
    """
        Base class for subjects.
    """
    def __init__(self):
        self.observers = list()

    def registerObserver(self, observer):
        self.observers.append(observer)

    def unregisterObserver(self, observer):
        self.observers.remove(observer)

    def notifyObservers(self, event):
        for obs in self.observers:
            obs.update(event)
