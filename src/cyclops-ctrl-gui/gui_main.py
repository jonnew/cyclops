# imports
import gui_frame, serial, ioThread, packet_gen
# defines
gui = gui_frame.Application()

def callback():
  if (gui.serUP):
    disconnect()
  gui.master.destroy()

gui.master.protocol("WM_DELETE_WINDOW", callback)

# PUT THE PORT_ID HERE
PORT = '/dev/ttyACM0'
gui.serUP = False
ser = serial.Serial()

io = ioThread.ioThread(gui.q, ser, gui)

def connect():
  try:
    ser.port = PORT
    ser.baudrate = 57600
    ser.timeout = 0.01
    ser.open()
    gui.serUP = True
    try:
      #start thread to listen on port
      io.start()
      #start polling function
      gui.processQ()
      gui.right_put('^_^ Opened %s, now listening for messages!' % PORT)
    except:
      gui.right_put('*_* Some thread error, restart propram!')
  except:
    gui.right_put('*_* Error in setting up port @ "%s".\n*_* Try another port (launch Arduino IDE)\n*_* Or disconnect-reconnect Teensy.' % PORT)

def disconnect():
  if gui.serUP:
    ser.close()
    gui.serUp = False
    gui.right_put('^_^ Closed port: %s' % PORT)
    gui.q.put('^_^ Session closed normally --------------\n')
    #logfile.close()
    io.end()

def do_user_command(user_command):
  user_command = user_command.split(',')
  sm = user_command[0]
  if sm in 'sS':
    channels, command = user_command[1:]
    if 0 < len(channels) <= 4:
      if 0 <= int(command) < 8:
        return packet_gen.make_sb([int(x) for x in channels], int(command))
    return -1

  elif sm in 'mM':
    channel = user_command[1]
    command = user_command[2]
    args    = user_command[3:]
    print(channel, command, args)
    if 0 <= int(channel) < 4:
      if 0 <= int(command) < 32:
        return packet_gen.make_mb_pkt(int(channel), int(command), args)
    return -1

  else:
    return -1

def bytesToStr(bytes_seq):
  try:
    res = ' '.join([hex(x) if x > 0 else hex(0xff+x) for x in bytes_seq])
    return res
  except:
    if bytes_seq >= 0:
      return hex(bytes_seq)
    return hex(0xff + bytes_seq)
    

def parse(event):
  cmd = gui.cmd.get()
  if not cmd=='':
    gui.cmd.delete('0', 'end')
    serial_out = do_user_command(cmd)
    if gui.serUP:
      if serial_out != -1:
        print(serial_out)
        ser.write(serial_out)
        show_on_right = cmd + " := " + bytesToStr(serial_out)
      else:
        print ("invalid:", cmd)
        show_on_right = "invalid " + cmd
    else:
      gui.right_put('*_* First connect to the Serial Port!')
    cmd = cmd.replace(" ", "")
    gui.right_put(show_on_right)

###################################
gui.setup(connect, disconnect, parse)
gui.master.title('Teensy Comms')
gui.mainloop()
