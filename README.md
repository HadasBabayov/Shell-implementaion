# Shell-implementaion

In this project I implemented shell. <br/>
I executed Foreground commands and Background commands. <br/>
For both types I created child processes, but in Background the parent process does not wait for the child process - i.e. the child process runs in the background. <br/>
In Foreground, the parent process will wait for the child process to complete before continuing its run. <br/>

**Instructions for how to run and use the code:**

First, compile ex1.c file by this command:
 ```
 gcc ex1.c
  ```
  
 Now, run this command:
  ```
  ./a.out
  ```
  
  You can enter SHELL commands and see how it works.
