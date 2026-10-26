# ITT440 Fork Program in C

This program demonstrates process creation and message passing in C.

## What it does

- The parent process asks the user to type a message.
- The program creates two child processes with `fork()`.
- The parent sends the same message to both children using separate pipes.
- Both children display the message in the terminal.
- Pressing Ctrl+C displays an interrupt message from the parent.

## Compile and run

```bash
gcc assignment.c -o a
./a.
```

This program is intended for Linux or another Unix-like operating system.
