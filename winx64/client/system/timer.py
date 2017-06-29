import time

class timer:
    def __init__(self, timeout):
        self.timeout = timeout
        self.clock_sample = None
        self.started = False

    def start(self):
        self.clock_sample = time.clock()
        self.started = True


    def tick(self):
        if not self.started:
            return
        now = time.clock()
        if (now - self.clock_sample) > self.timeout:
            self.clock_sample = time.clock()
            return True

