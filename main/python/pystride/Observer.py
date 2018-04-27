class Observer:
    """
        Base class for Observers.
    """
    def __init__(self):
        pass

    def update(self, event):
        print("Update on " + event)
