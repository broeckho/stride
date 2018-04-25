import time

class Stopwatch:
    def __init__(self, name="stopwatch"):
        self.accumulated = None
        self.last_start = None
        self.name = name
        self.is_running = False

    def start(self):
        if not self.is_running:
            self.is_running = True
            self.last_start = time.mktime(time.localtime())

    def stop(self):
        if self.is_running:
            now = time.mktime(time.localtime())
            if self.accumulated != None:
                self.accumulated += (now - self.last_start)
            else:
                self.accumulated = (now - self.last_start)
            self.running = False

    def reset(self):
        self.accumulated = None
        self.running = False

    def isRunning(self):
        return self.is_running

    def getName(self):
        return self.name

    def get(self):
        temp = self.accumulated
        now = time.mktime(time.localtime())
        if self.is_running:
            if temp != None:
                temp += (now - self.last_start)
            else:
                temp = (now - self.last_start)
        return temp
