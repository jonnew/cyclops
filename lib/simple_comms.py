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
  if (command in [0, 1]):
    # 1 arg, 
    res = struct.pack('<c', args_list[0].to_bytes(1, byteorder="little"))
  elif (command in [2]):
    res = struct.pack('<cc', args_list[0].to_bytes(1, byteorder="little"), args_list[1].to_bytes(1, byteorder="little"))
  elif (command in [3, 7, 8]):
    res = struct.pack('<i', args_list[0].to_bytes(4, byteorder="little"))
  elif (command in [4, 5]):
    res = struct.pack('<f', args_list[0])
  elif (command in [6]):
    res = struct.pack('<h', args_list[0].to_bytes(2, byteorder="little"))
  return res

def make_mb_pkt(channel, command, args_list):
  return make_mb_header(channel, command)+make_args(command, args_list)


ser = serial.Serial('/dev/ttyACM0', 57600)

#print(make_pkt([3,0,1], 6, [], 'sb')) # 1101 1110
#print(make_pkt([2], 4, [12.5]))       # 0100 0100     0000 0000 0000 0000 0100 1000 0100 0001

packets = [
  make_sb([3,2,1,0], 1), #stop
  make_sb([3,2,1,0], 0), #start
  make_mb_pkt(0, 1, [0]), #one_shot -- reset and blink once
  make_mb_pkt(0, 2, [0, 2]), #n_shot -- reset and blibk twice
  make_mb_pkt(0, 0, [0]), #loopback
]

for i in range(0, len(packets)):
  input()
  ser.write(packets[i])
  print("sent", i)
  ch = b'.'
  while ch != b'\n':
    ch = ser.read(1)
    print(ch)