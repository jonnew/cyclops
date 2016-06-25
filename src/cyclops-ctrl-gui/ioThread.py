import threading

class ioThread:
  def __init__(self, queue, ser, parent):
    self.running = False
    self.q = queue
    self.ser = ser
    self.count = 0
    self.snap = []
    self.parent = parent

  def start(self):
    self.running = True
    self.influx = threading.Thread(target=self.listen)
    self.influx.start()

  def end(self):
    self.running = False

  def listen(self):
    while self.running:
      data = self.ser.read(1)
      if not data == b'':  #enter if-block only if some data has been sent to PC
        self.q.put(data)