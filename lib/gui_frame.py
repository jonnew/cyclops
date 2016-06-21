import tkinter as tk
import queue

class Application(tk.Frame):

  def __init__(self, master = None):
    tk.Frame.__init__(self, master)
    self.q = queue.Queue()
    self.serUP = False
    self.grid()
    self.msg = ''
    self.createWidgets()

  def createWidgets(self):
    self.oFrame = tk.Frame(self)
    self.iFrame = tk.Frame(self)

    self.iflux = tk.Text(self.iFrame, height=24, pady=5, relief=tk.RIDGE)
    self.oflux = tk.Text(self.oFrame, height=24, pady=5, relief=tk.RIDGE, bg="pink")
   
    self.iClear = tk.Button(self.iFrame, text='Clear', command=self.clear_iflux)
    self.oClear = tk.Button(self.oFrame, text='Clear', command=self.clear_oflux)

    self.cmd_label = tk.Label(self.oFrame, text="Command:")
    self.cmd = tk.Entry(self.oFrame, bg="cyan")
    self.placeWidgets()

  def clear_iflux(self):
    self.iflux.delete('1.0', 'end')

  def clear_oflux(self):
    self.oflux.delete('1.0', 'end')

  def placeWidgets(self):
    self.iFrame.grid(row=0, column=0, columnspan=3)
    self.iflux.grid(row=0, column=0, columnspan=3)

    self.oFrame.grid(row=0, column=3, columnspan=3)
    self.oflux.grid(row=0, column=3, columnspan=3)
    
    self.iClear.grid(row=1, column=2)
    self.oClear.grid(row=1, column=5)

    self.cmd_label.grid(row=1, column=3, sticky=tk.E)
    self.cmd.grid(row=1, column=4, sticky=tk.W)
    self.cmd.focus_force()

  def setup(self, con, discon, parse):
    self.start = tk.Button(self.iFrame, text='Connect', fg = "green", command=con)
    self.end = tk.Button(self.iFrame, text='Disconnect', fg = "red", command=discon)
    self.start.grid(row=1, column=0)
    self.end.grid(row=1, column=1)
    self.cmd.bind('<KeyPress-Return>', parse)
    self.cmd.bind('<KeyPress-KP_Enter>', parse)

  def right_put(self, msg):
    self.oflux.insert('1.0', msg+'\n')

  def left_put(self, msg):
    self.iflux.insert('1.0', msg+'\n')

  # GUI MAINLOOP
  def processQ(self):
    while self.q.qsize() > 0:
      try:
        cue = str(self.q.get(0))+'\n'
        self.iflux.insert('1.0', cue)
      except Queue.Empty:
        pass
      if not self.serUP:
        break
    if self.serUP:
      self.after(10, self.processQ)
    else:
      return