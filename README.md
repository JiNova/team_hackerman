# Final Project: Buffer overflow exploit with shell injection

## Team Members: 
 - Sushant Bansal
 - Andreas Brandner
 - Chen-Ju Hou
 - Yuta Nakamura

## Description:
- Implement a server in C that reads data from a socket and writes it to memory with an unsafe memory operation
- Implement a client program that writes a malicious payload to spawn a reverse shell via TCP to the server's socket

## Crafting the vulnerable system:
- Create a vulnerable environment in a vm(We are using Ubuntu 16.04 64-bit) through disabling protections such as ASLR
	- Disable ASLR temporarily with: `echo "0" | sudo dd of=/proc/sys/kernel/randomize_va_space` 
	- Disable ASLR permanently by adding `kernel.randomize_va_space = 0` to /etc/sysctl.conf

## Exploitation:
- Server is being run by user root
- Open netcat on own machine to catch reverse shell from server (e.g. `nc localhost -lp 12345` if shellcode will connect to port 12345)
- Start client with payload, e.g. `python payload.py | ./client 0.0.0.0`
- Shell with root access is now accessible in netcat


## Usage Notes:
- Server
	- Compile with: `gcc server.c -o server -fno-stack-protector -z execstack`, so the server won't have stack-protection and the stack will be executable
	- Usage: ./server.exec	
	- Close all clients before exiting server! , otherwise reset terminal session before restarting server
	- Hit < Ctrl > + < C > to close the server
- Client
	- Compile with makefile
	- Usage: ./client.exec <IP_SERVER>
	- Send "exit" to server to quit client
	
## VM configuration:
- add port-forwarding for ssh-server and socket-server in VirtualBox
