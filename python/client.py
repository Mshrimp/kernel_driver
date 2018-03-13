#! /usr/bin/python3
# file: client.py

import socket
import sys

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

host = socket.gethostname()

port = 8088

#address = (host, port)
address = ('127.0.0.1', port)

s.connect(address)

print("Client connected")

msg = s.recv(1024)

#print(msg)

s.close()

print (msg.decode('utf-8'))

