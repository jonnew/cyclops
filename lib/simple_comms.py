import packet_gen, curses, time, curses.textpad

def launch(stdscr):
  Y, X = stdscr.getmaxyx()

  recv_window = curses.newwin(Y-3, X, 0, 0)
  recv_window.border()
  recv_window.refresh()

  send_window = curses.newwin(3, X, Y-3, 0)
  send_window.refresh()
  tb = curses.textpad.Textbox(send_window, insert_mode=True)
  text = tb.edit()
  recv_window.addstr(1, 1, text)
  recv_window.refresh()
  time.sleep(2)

curses.wrapper(launch)

"""
ser = serial.Serial('/dev/ttyACM0', 57600)
for i in range(0, len(packets)):
  input("send next?")
  ser.write(packets[i])
  print("sent", i)
  print("header =", ser.read(1))
  print("<", ser.read(1))
"""