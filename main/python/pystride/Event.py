from enum import Enum

class EventType(Enum):
    AtStart = 1
    Stepped = 2
    AtFinished = 3
    SetupBegin = 4
    SetupEnd = 5

class Event:
    def __init__(self, event_type):
        self.event_type = event_type

    def __str__(self):
        return self.event_type.name


class SteppedEvent(Event):
    def __init__(self, timestep):
        super(SteppedEvent, self).__init__(EventType.Stepped)
        self.timestep = timestep
