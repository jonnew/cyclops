#!/bin/python
import serial, time, struct

def make_sb(channel_list, command):
  channel_bits = 0
  for i in channel_list:
    channel_bits |= (1 << i)
  channel_bits <<= 3
  res = channel_bits | command | 128
  return res.to_bytes(1, byteorder="little") # byteorder not really required because it' just gonna be 1 byte

def make_mb_header(channel, command):
  res = channel << 5 | command
  return res.to_bytes(1, byteorder="little")

def make_args(command, args_list):
  print(args_list)
  if (command in [0, 1]):
    res = struct.pack('<c', int(args_list[0]).to_bytes(1, byteorder='little'))
  elif (command in [2]):
    res = struct.pack('<cc', int(args_list[0]).to_bytes(1, byteorder='little'), int(args_list[1]).to_bytes(1, byteorder='little'))
  elif (command in [3, 7, 8]):
    res = struct.pack('<i', int(args_list[0]))
  elif (command in [4, 5]):
    res = struct.pack('<f', float(args_list[0]))
  elif (command in [6, 9, 10]):
    res = struct.pack('<h', int(args_list[0]))
  return res

def make_mb_pkt(channel, command, args_list):
  return make_mb_header(channel, command)+make_args(command, args_list)

"""
packets = [
  make_sb([3,2,1,0], 1), #stop
  make_sb([3,2,1,0], 0), #start
  make_mb_pkt(0, 1, [0]), #one_shot -- reset and blink once
  make_mb_pkt(0, 2, [0, 2]), #n_shot -- reset and blink twice
  make_mb_pkt(0, 0, [0]), #loopback
]


ser = serial.Serial('/dev/ttyACM0', 57600)
for i in range(0, len(packets)):
  input("send next?")
  ser.write(packets[i])
  print("sent", i)
  print("header =", ser.read(1))
  print("<", ser.read(1))
"""

"""
start    | s, , 0
stop     | s, , 1
reset    | s, , 2
swap     | s, , 3
identity | s, , 7

change_source_l    | m, , 0, 
change_source_o    | m, , 1, 
change_source_n    | m, , 2, ,
change_time_period | m, , 3, 
time_factor        | m, , 4, 
voltage_factor     | m, , 5, 
voltage_offset     | m, , 6, 
square_on_time     | m, , 7, 
square_off_time    | m, , 8, 
square_on_level    | m, , 9, 
square_off_level   | m, , 10, 
"""
