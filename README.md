A simple Unix command line shell written in C and developed and tested on Linux. This project was developed to gain practical experience with Unix/Linux systems programming, process creation, inter-process communication, file descriptors, and command execution.

Features:
- Executes Unix/Linux commands
- Creates and manages processes using fork() and execvp()
- Waits for foreground processes using waitpid()
- Supports background execution using &
- Supports pipes using pipe()
- Supports input redirection using <
- Supports output redirection using >
- Supports built-in cd command
- Supports built-in exit command
- Uses file descriptors and dup2() for input/output redirection

Technologies Used:
- C, Linux / Unix, POSIX system calls, Process management, Inter process communication (IPC), File descriptors, NetBeans IDE 
  
Example Commands:
- ls
- pwd
- sleep 3 &
- ls | wc
- echo HELLO > test.txt
- cat < test.txt
- cd
- echo HELLO > test.txt
cat < test.txt
cd 
