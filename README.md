# Assignment Socket Programming (FTP_server)
Practice carried out in C++ in the subject of **Redes y Sistemas Distribuidos** of the 2nd year of Computer Engineering at the University of La Laguna. 

## Introduction
This assignment has the objective of learning how to send and receive files to/from
a server, developing a program that acts as the server while the client requests its petitions.
This will be implemented using the File Transfer Protocol (FTP) with its basic commands,
such as active and passive modes, uploading and downloading a file or listing the files
contained in the server.

## Description of the developed protocol
The File Transfer Protocol or FTP is a standard network protocol that allows the
transfer of files between a client and a server in order to store or retrieve them from the
server. It uses the Transmission Control Protocol (TCP) to connect the client with the
server, involving two different connections: control and data. In our program, the control
connection will be managed with the port 2121, and its main function is to transfer the
commands sent by the client and reply to them.

In FTP we distinguish between two different modes; active and passive, being the
main difference the way they manage the data connection. In both modes, the client
connects from an unprivileged port to the server command port but in the active mode,
the client starts listening for incoming data connections from the server on a certain port,
which is specified by the client using the PORT command. Then, the server initiates a
data channel to the client from port 20.

However, the active mode depends on connecting back to the client, which may
result in problems when the client is behind a firewall and does not accept incoming TCP
connections. The solution to this problem is to use the passive mode, where the client
uses the control connection to send a PASV command and then receives an IP address
and a server port number. After that, the client connects to the port received from an
arbitrary one, which avoids the necessity of initiating a TCP connection to the client.
The code will be separated into two classes created with their respective .cpp and
.h files, one designed for the establishment of the FTP server and the other one to process
the client´s request.

When the program start running, an FTP server object is created with the port
2121, a non-privileged one so that it is not needed to execute the program as administrator.
Once the object is created, we create a TCP socket and, while the program keeps running,
it will establish connections on different threads with the sockets that the command
accept() returns form a queue of already accepted connections.

On the code of each command will be found many different types of response
codes with a brief message to give the client information about the current state of the
connection. Each response code is formed with 3 numbers, being the first of them which
defines the aspect of it, and the others to differentiate them:
- 1XX: Information
- 2XX: Success
- 3XX: Redirection
- 4XX: Client error
- 5XX: Server error

Also, this assignment has been focused on the develop of a little FTP server
capable of a short set of actions, that will be implemented by us as commands and consist
of:

- **PORT**: This command executes when an operation in active mode is done,
establishing the connection with the given IP and port on the arguments.
- **PASV**: Runs when the connection is on passive mode, the client tells where to
connect the data port on the server.
- **RETR**: Executes when the client requests a file, and the connection is already
established. The server sends a copy of the file to the client.
- **STOR**: Executes when the client tries sending a file to the server and the
connection is already established. The server downloads a copy of the file,
creating a new one or replacing the one existing already.
- **LIST**: This command is executed when the client introduces the ‘ls’ option. After
that, the files where the server is running are listed.

## Guide for the compilation of the code
To compile the code a *makefile* has been provided so that the user only has to enter
the command `make` in the console and have an executable named `ftp_server`. To
execute, the only requirement is to have the necessary permission and type
`./ftp_server` in the command line. Before compiling, make sure that you have the
following packages installed: `gcc, g++ and make`.

- In order to use it, we will have to open a terminal in the project directory, and simply type
command `make`:

<p align="center">
  <img src="https://github.com/feichay10/FTP_server/blob/ee7f0f40b1e2c16caf334046e547d7299eed67f6/assets/Screenshot_1.png" />
</p>

- Then, ro run the server:
<p align="center">
  <img src="https://github.com/feichay10/FTP_server/blob/ee7f0f40b1e2c16caf334046e547d7299eed67f6/assets/Screenshot_2.png" />
</p>

- All that is left is to run the client, for which we will need another terminal:
<p align="center">
  <img src="https://github.com/feichay10/FTP_server/blob/ee7f0f40b1e2c16caf334046e547d7299eed67f6/assets/Screenshot_3.png" />
</p>

- Now everything is set, and all we should do to connect to the server (which is run locally)
is opening a local host connection:
<p align="center">
  <img src="https://github.com/feichay10/FTP_server/blob/ee7f0f40b1e2c16caf334046e547d7299eed67f6/assets/Screenshot_4.png" />
</p>

## Test case
- Getting a file in active mode:
<p align="center">
  <img src="https://github.com/feichay10/FTP_server/blob/ee7f0f40b1e2c16caf334046e547d7299eed67f6/assets/Screenshot_5.png" />
</p>

- Getting a file in passive mode:
<p align="center">
  <img src="https://github.com/feichay10/FTP_server/blob/ee7f0f40b1e2c16caf334046e547d7299eed67f6/assets/Screenshot_6.png" />
</p>

- Putting a file in active mode:
<p align="center">
  <img src="https://github.com/feichay10/FTP_server/blob/ee7f0f40b1e2c16caf334046e547d7299eed67f6/assets/Screenshot_7.png" />
</p>

- *ls* in active mode:
<p align="center">
  <img src="https://github.com/feichay10/FTP_server/blob/ee7f0f40b1e2c16caf334046e547d7299eed67f6/assets/Screenshot_8.png" />
</p>

- *ls*^in passive mode:
<p align="center">
  <img src="https://github.com/feichay10/FTP_server/blob/ee7f0f40b1e2c16caf334046e547d7299eed67f6/assets/Screenshot_9.png" />
</p>
