#! /usr/bin/python3
# file: server.py

import socket
import sys

serversocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

host = socket.gethostname()

port = 8088

address = ('127.0.0.1', port)
#address = (host, port)

serversocket.bind(address)

serversocket.listen(5)

print("Waiting for connection ..." + "\r\n")

count = 0

while (count < 5):
	clientsocket, addr = serversocket.accept()

	count = count + 1
	print("Client connected, count: ", count)

	msg = "Hello" + "\r\n"

	clientsocket.send(msg.encode('utf-8'))

	clientsocket.close()
	
	print("Client disconnect")

serversocket.close()
