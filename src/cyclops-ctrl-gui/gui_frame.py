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

    self.oflux.insert( "1.0" , "="*80)
    self.oflux.insert( "1.0" , " square_off_level    | m, <channel>, 10, <uint16>                               " )   
    self.oflux.insert( "1.0" , " square_on_level     | m, <channel>, 9, <uint16>                                " )
    self.oflux.insert( "1.0" , " square_off_time     | m, <channel>, 8, <uint32>                                " )
    self.oflux.insert( "1.0" , " square_on_time      | m, <channel>, 7, <uint32>                                " )
    self.oflux.insert( "1.0" , " voltage_offset      | m, <channel>, 6, <uint16>                                " )
    self.oflux.insert( "1.0" , " voltage_factor      | m, <channel>, 5, <float>                                 " )
    self.oflux.insert( "1.0" , " time_factor         | m, <channel>, 4, <float>                                 " )
    self.oflux.insert( "1.0" , " change_time_period  | m, <channel>, 3, <uint32>                                " )
    self.oflux.insert( "1.0" , " change_source_n_shot| m, <channel>, 2, <src_id>, <cycles>                      " )
    self.oflux.insert( "1.0" , " change_source_one   | m, <channel>, 1, <src_id>                                " )
    self.oflux.insert( "1.0" , " change_source_loop  | m, <channel>, 0, <src_id>                                " )
    self.oflux.insert( "1.0" , " Multi Byte                                                                     " )
    self.oflux.insert( "1.0" , "~"*80)
    self.oflux.insert( "1.0" , " identity            | s, <anything>, 7  }                                      " )
    self.oflux.insert( "1.0" , " end (experiment)    | s, <anything>, 5  }                                      " )
    self.oflux.insert( "1.0" , " launch (experiment) | s, <anything>, 4  }  \+-----------------------+/         " )
    self.oflux.insert( "1.0" , " swap                | s, <channels>, 3  }  || 2013 for all channels ||         " )
    self.oflux.insert( "1.0" , " reset               | s, <channels>, 2  }  || 1 for ch1             ||         " )
    self.oflux.insert( "1.0" , " stop                | s, <channels>, 1  }  || 210 for ch0,1,2       ||         " )
    self.oflux.insert( "1.0" , " start               | s, <channels>, 0  }  || <channels> can be:    ||         " )
    self.oflux.insert( "1.0" , "                                            /+---------HELP----------+\         " )
    self.oflux.insert( "1.0" , " Single Byte                                                                    " )
    self.oflux.insert( "1.0" , " Messages transmitted from PC will be recorded here. [oFrame]                   " )
    self.oflux.insert( "1.0" , "="*80)

    self.iflux.insert( "1.0" , "="*80)
    self.iflux.insert( "1.0" , "Messages recieved by PC will be recorded here. (1 byte per line) [iFrame]       " )
    self.iflux.insert( "1.0" , "connected, though its safe to do so.                                            " )
    self.iflux.insert( "1.0" , "automatically disconnects. Try not to flash the teensy while the GUI is         " )
    self.iflux.insert( "1.0" , "Both panes are scrollable. You must press \"Connect\" to send packets. Quitting   " )
    self.iflux.insert( "1.0" , "="*80)

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