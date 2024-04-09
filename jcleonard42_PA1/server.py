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
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
except socket.error: # if a socket object is not created, terminate server creation
            print("Could not create server socket: terminating program")
            sys.exit(1)


# Define the server address and port
server_address = ('vm-2', 8080)


# Bind the socket to the server address
try:
    server_socket.bind(server_address)
except socket.error: # If socket cannot be bound to server, terminate creation
            print("Could not bind server address to socket")
            sys.exit(1)


# Listen for incoming connections (max 5 clients in the queue)
try:
    server_socket.listen(5)
    print("Server is listening on", server_address)
except socket.error:
            print("Error while waiting for client connection")
            sys.exit(1)


while True:
    # Wait for a client to connect
    client_socket, client_address = server_socket.accept()
    if not client_socket: # if a client socket is not generated upon connection, exit
          break
    
    # Print a message to indicate the client connection
    print(f"Received connection from {client_address}")

    # Handle client data
    while True:
        # Receive data from the client
        data = client_socket.recv(1048576).decode('utf-8')
        if not data:
            break
        
        # Process and respond to the client's data
        response = f"{data}"
        
        # Send the response back to the client
        try:
            client_socket.send(response.encode('utf-8'))
        except socket.error:
            print("Could not respond to client, exiting cnnection")
            break

    # Close the client socket
    client_socket.close()