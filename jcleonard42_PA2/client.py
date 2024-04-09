'''
Name: Jonathan Leonard
Class: CSC-4200
Assignment: PA2
Instructor: Susmit Shannigrahi
'''

import sys
import argparse
import socket
import struct

def create_packet(version, header_length, service_type, payload):
    
    # This section encodes the payload based on the service type passes as an argument on the command line
    # Each except statement will exit if the chosen service type does not match the payload
    if service_type == 1:
        try:
            payload_bytes = struct.pack('i', int(payload))
        except:
            print("What you have entered is not an integer: exiting")
            sys.exit(1)

    elif service_type == 2:
        try:
            payload_bytes = str(payload).encode('utf-8')
        except:
            print("What you have entered is not a float: exiting")
            sys.exit(1)

    elif service_type == 3:
        try:
            payload_bytes = payload.encode('utf-8')
        except:
            print("What you have entered is not a string: exiting")
            sys.exit(1)

    else:
        print("Error: Invalid service_type.")
        return None

    # Calculate the payload length
    payload_length = len(payload_bytes)

    # Create a fixed-length header
    try:
        header = struct.pack('BBBH', version, header_length, service_type, payload_length)
    except:
        print("Unable to create packet header: exiting")
        sys.exit(1)

    # Combine the header and payload
    packet = header + payload_bytes

    return packet


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="Client for packet creation and sending.")
    parser.add_argument('--version', type=int, required=True, help='Packet version')
    parser.add_argument('--header_length', type=int, required=True, help='Length of the packet header')
    parser.add_argument('--service_type', type=int, required=True, help='Service type of the payload (1 for int, 2 for float, 3 for string)')
    parser.add_argument('--payload', type=str, required=True, help='Payload to be packed into the packet')
    parser.add_argument('--host', type=str, default='localhost', help='Server host')
    parser.add_argument('--port', type=int, default=12345, help='Server port')

    args = parser.parse_args()

    try:
        packet = create_packet(args.version, args.header_length, args.service_type, args.payload)
    except:
        print("One or more of the packet attributes are of the incorrect type: Sorry!")
        sys.exit(1)

    if packet is not None: # If the packet is not empty
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s: # opens a socket
            
            # connects to host and port passed on the command line
            try:
                s.connect((args.host, args.port))
            except socket.error:
                print("Error occurred during connection: exiting")
                sys.exit(1)
            
            # send packet
            try:
                s.sendall(packet)
            except socket.error:
                print("Unable to send packet: exiting")
                sys.exit(1)

            # Receive the packet from the server
            try:
                response_packet = s.recv(1048576)  
            except socket.error:
                print("Unable to receive packet: exiting")
                sys.exit(1)
            

            # Print the entire response packet as a string
            print(f"Received Packet: {response_packet.decode('utf-8')}")