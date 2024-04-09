'''
Name: Jonathan Leonard
Class: CSC-4200
Assignment: PA2
Instructor: Susmit Shannigrahi
'''

import sys
import socket
import struct

def unpack_packet(conn, header_format):
    try:

        # finds header size to establish number of bytes needed
        header_size = struct.calcsize(header_format)
        header_bytes = conn.recv(header_size)

        # if header is empty, return none
        if not header_bytes:
            return None
        
        # Unpack header and place values into variables
        version, header_length, service_type, payload_length = struct.unpack(header_format, header_bytes)

        # Process payload based on service type (each has different encoding method)
        if service_type == 1:
            payload = struct.unpack('i', conn.recv(payload_length))[0]
        else:
            payload = conn.recv(payload_length)

        # Combine all portions of the packet header
        packet_header_as_string = f"Version: {version}, Header Length: {header_length}, Service Type: {service_type}"

    except:
        print("Error occurred when processing packet. Please restart server and try again!")
        sys.exit(1)
    
    # return packet header and payload
    return packet_header_as_string, payload

if __name__ == '__main__':  
    host = 'localhost'      # TODO: If running on another machine, be sure to change these values or the program will not work
    port = 12345

    header_format = 'BBBH'  # Version, Header Length, Service Type, Payload Length

    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        
        try:
            s.bind((host, port))
        except socket.error:
            print("Could not bind server address to socket. Please try restarting the server.")
            sys.exit(1)

        try:
            s.listen()
            print(f"Server listening on {host}:{port}")
        except socket.error:
            print("Error while waiting for client connection. Please try restartin the server")
            sys.exit(1)


        while True:
            try:
                conn, addr = s.accept()
            except socket.error:
                print("Error while creating client connection. Please try restarting the server.")
                sys.exit(1)


            with conn:
                print(f"Connected by: {addr}")
                while True:

                    try:
                        # Receive and unpack packet, creating a header and payload
                        header_string, payload = unpack_packet(conn, header_format)
                        
                        print(f"Received Header: {header_string}")
                        
                        if isinstance(payload, bytes): # This decodes payloads of types 2 (float) and 3 (string)

                            # Handle different payload types (type 1 is handled in unpack_packet funtion)
                            if header_string.endswith("Service Type: 2"):
                                # Handle float payload
                                payload = float(payload.decode('utf-8'))

                            elif header_string.endswith("Service Type: 3"):
                                # Handle string payload
                                payload = payload.decode('utf-8')
                        
                        # Prints packet payload
                        print(f"Received Payload: {payload}")

                        # Create a new payload to return to client
                        new_payload = header_string.encode('utf-8') + b' \nReceived Payload: ' + str(payload).encode('utf-8')

                        # Send the payload back to client
                        conn.sendall(new_payload)
                    except:
                        print("Client connection closed: Waiting for new connection.")
                        break
