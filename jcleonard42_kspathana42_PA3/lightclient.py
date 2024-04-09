'''
Name: Jonathan Leonard and Kevin Pathana
Class: CSC-4200
Assignment: PA3
Instructor: Susmit Shannigrahi
'''

import socket
import struct
import logging
import argparse
import time

parser = argparse.ArgumentParser()
parser.add_argument("-s", "--server", help="The IP address of the server")
parser.add_argument("-p", "--port", type=int, help="The port the server listens on")
parser.add_argument("-l", "--log", help="The log file location")
args = parser.parse_args()

server_ip = args.server
port = args.port
log_file = args.log

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((server_ip, port))

logging.basicConfig(filename=log_file, level=logging.INFO, format='%(message)s')

PACKET_FORMAT = ">III8s"
VERSION = 17
HELLO = 1
LIGHTON = 2

def send_hello(conn):
    packet = struct.pack(PACKET_FORMAT, VERSION, HELLO, len("HELLO"), b"HELLO")
    conn.sendall(packet)
    timestamp = time.strftime("%Y-%m-%d %H:%M:%S") + f",{str(int(time.time() % 1 * 1000)).zfill(3)}"
    logging.info("%s Sending HELLO Packet", timestamp)
    print("%s Sending HELLO Packet" % timestamp)

def receive_packet(conn):
    header = b""
    while len(header) < 12:
        chunk = conn.recv(12 - len(header))
        if not chunk:
            break
        header += chunk

    if len(header) < 12:
        return None

    version, msg_type, msg_len = struct.unpack(">III", header)
    timestamp = time.strftime("%Y-%m-%d %H:%M:%S") + f",{str(int(time.time() % 1 * 1000)).zfill(3)}"
    logging.info("%s Received Data: version: %d type: %d length: %d", timestamp, version, msg_type, msg_len)
    print("%s Received Data: version: %d type: %d length: %d" % (timestamp, version, msg_type, msg_len))

    if version != VERSION:
        logging.error("%s VERSION MISMATCH", timestamp)
        print("%s VERSION MISMATCH" % timestamp)
        return None

    message = conn.recv(8)
    message = message.decode()

    return (msg_type, message)

def send_command(conn):
    command = "LIGHTON".encode('utf-8')
    length = len(command)
    packet = struct.pack(PACKET_FORMAT, VERSION, LIGHTON, length, command)
    conn.sendall(packet)
    timestamp = time.strftime("%Y-%m-%d %H:%M:%S") + f",{str(int(time.time() % 1 * 1000)).zfill(3)}"
    logging.info("%s Sending command", timestamp)
    print("%s Sending command" % timestamp)

# Send a hello packet to the server and receive a reply from it.
send_hello(s)
reply = receive_packet(s)

if reply is not None:
   msg_type, message = reply

if msg_type == HELLO:
   timestamp = time.strftime("%Y-%m-%d %H:%M:%S")
   logging.info("%s VERSION ACCEPTED", timestamp)
   print("%s VERSION ACCEPTED" % timestamp)
   print("%s Received Message %s" % (timestamp, message))

send_command(s)

reply = receive_packet(s)
if reply is not None:
   msg_type, message = reply

if msg_type == HELLO:
   timestamp = time.strftime("%Y-%m-%d %H:%M:%S") + f",{str(int(time.time() % 1 * 1000)).zfill(3)}"
   logging.info("%s VERSION ACCEPTED", timestamp)
   print("%s VERSION ACCEPTED" % timestamp)
   print("%s Received Message %s" % (timestamp, message))

   if message == "SUCCESS":
       print("Command Successful")

print("Closing socket")
s.close()
