'''
Name: Jonathan Leonard
Class: CSC-4200
Assignment: PA1
Instructor: Susmit Shannigrahi
'''

import socket
import sys


# Create a socket object
try:
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
except socket.error: # if the client socket cannot be created, exit the program gracefully
            print("Could not create client socket: terminating program")
            sys.exit(1)


'''
****** Important Note *********
when using this program on different machines
it is important to change server address and port
'''
# Define the server address and port
server_address = ('vm-2', 8080)


# Attempts to connect to the server, end if fails
try:
    client_socket.connect(server_address)
except socket.error: # if the client cannot connect to the server, exit the program gracefully
            print("Could not connect with the socket-server: terminating program")
            client_socket.close()
            sys.exit(1)


# Loops while messages are being sent
while True:

    # Get user input
    message = input("Enter a message to send to the server (or 'exit' to quit): ")
        

    # Send the message to the server
    try:
        client_socket.sendall(message.encode('utf-8'))
    except socket.error: # if message cannot be sent to server, exit program and try reconnecting
            print("Could not send message to server: terminating program")
            break

    # if user enters exit, end while loop
    if message == 'exit':
        break
    

    # Receive the server's response
    try:
        response = client_socket.recv(1048576).decode('utf-8')
    except socket.error: # if a message cannot be received from the server, exit and try reconnecting
          print("Could not send message to server: terminating program")
          break
    
    # print the server's response
    print("Server Response:", response)


# Close the client socket
client_socket.close()