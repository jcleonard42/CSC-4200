# CSC 4200 - PA3_README

**Authors:**
- Jonathan Leonard (jcleonard42)
- Kevin Pathana (kspathana42)

## Work Process Description:

We chose to write the server and client in different languages but managed to get them to communicate properly as specified in the assignment. Kevin Pathana wrote the lightserver in C while Jonathan Leonard wrote the lightclient program in Python. What we have is a simple socket server and client that communicate with each other flawlessly. We have included a Windows compilation for the server as it was first written for Windows, then ported over to Linux with the change of libraries and function calls. As such, the compilation command requires the `-lws2_32` tag for Windows, while the Linux compilation does not. As for the Python client, it does not need to have any differences between the Windows and Linux version, as it works on both with no problem. We have tested them in the Google VM and it does indeed work with the Google VMs. We did have to install GCC and other GNU libraries through `sudo apt-get install gcc` because for some reason the Google VMs did not come preloaded with them.

## Windows Compilation:

**Server command:**

- Compilation: `gcc -o lightserver lightserver.c -lws2_32`
- Execution: `.\lightserver <port number> .\<log file>`

**Client Command:**

- `python lightclient.py -s <ip address> -p <port number> -l .\<log file>` (or `python3`)

## Linux Compilation:

**Server command:**

- Compilation: `gcc -o lightserver lightserver.c`
- Execution: `./lightserver <port number> ./<log file>`

**Client Command:**

- `python lightclient.py -s <ip address> -p <port number> -l .\<log file>` (or `python3`)

## PCAP Capture:

We used termshark (terminal UI for tshark, which in turn uses TCPDUMP) to capture the data desired from the client and server. (Screenshot is not accurate data, the .pcap files will have the data).

## Google VMs (Ubuntu 23.04):

- V1 = Server
- V2 = Client
