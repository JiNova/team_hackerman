### Team Members: Sushant Bansal, Chen Ju Hou, Andreas Brandner, Yuta Nakamura
### Topic name: Buffer overflow exploit with shell injection

## Description:
- Implement a server in C that reads data from a socket and writes it to memory with an unsafe memory operation
- Implement client program that writes a malicious payload (e.g., code that exploits the server and spawns a shell) to the server's socket

## Crafting the vulnerable system:
- Create a vulnerable environment in a vm through disabling protections such as ASLR


## Exploitation:
- Write a malicious script that allow attackers to exploit the server
- We will analyze the targeted system and find where we could inject shellcodes through our script
- Send over the malicious payload to the targeted server which will give the attackers shell access.
- Host on some cloud platform VM that will allow user to execute shell commands on the targeted machine after the exploit for demonstration.


## Usage Notes:
- Compile using the makefile provided
- Server
	- Usage: ./server.exec	
	- Close all clients before exiting server! , otherwise reset terminal session before restarting server
	- Hit < Ctrl > + < C > to close the server

- Client
	- Usage: ./client.exec <IP_SERVER>
	- Send "exit" to server to quit client