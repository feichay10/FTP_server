//****************************************************************************
//                         REDES Y SISTEMAS DISTRIBUIDOS
//
//                     2º de grado de Ingeniería Informática
//
//              This class processes an FTP transaction.
//
//****************************************************************************

#include <cstring>
#include <cstdarg>
#include <cstdio>
#include <cerrno>
#include <netdb.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <locale.h>
#include <langinfo.h>
#include <fcntl.h>
#include <unistd.h>

#include <sys/stat.h>
#include <iostream>
#include <dirent.h>

#include "common.h"

#include "ClientConnection.h"

int define_socket_TCP(int port);

ClientConnection::ClientConnection(int s)
{
  int sock = (int)(s);

  char buffer[MAX_BUFF];

  control_socket = s;
  // Check the Linux man pages to know what fdopen does.
  fd = fdopen(s, "a+");
  if (fd == NULL)
  {
    std::cout << "Connection closed" << std::endl;

    fclose(fd);
    close(control_socket);
    ok = false;
    return;
  }

  ok = true;
  data_socket = -1;
  parar = false;
};

ClientConnection::~ClientConnection()
{
  fclose(fd);
  close(control_socket);
}

int connect_TCP(uint32_t address, uint16_t port)
{
  // Implement your code to define a socket here
  struct sockaddr_in sin;
  struct hostent *hent;
  int s;

  memset(&sin, 0, sizeof(sin));
  sin.sin_family = AF_INET;
  sin.sin_port = htons(port);
  sin.sin_addr.s_addr = address;

  std::string x = std::to_string(address);
  char const *pchar = x.c_str();

  if (hent = gethostbyname(pchar))
  {
    memcpy(&sin.sin_addr, hent->h_addr, hent->h_length);
  }
  else if ((sin.sin_addr.s_addr == INADDR_NONE))
  {
    errexit("Cannot solve the name \"%s\"\n", pchar);
  }

  s = socket(AF_INET, SOCK_STREAM, 0);

  if (s < 0)
  {
    errexit("Cannot create the socket: %s\n", strerror(errno));
  }
  if (connect(s, (struct sockaddr *)&sin, sizeof(sin)) < 0)
  {
    errexit("Cannot connect with %s: %s\n", address, strerror(errno));
  }
  return s; //You must return the socket descriptor
}

void ClientConnection::stop()
{
  close(data_socket);
  close(control_socket);
  parar = true;
}

#define COMMAND(cmd) strcmp(command, cmd) == 0

// This method processes the requests.
// Here you should implement the actions related to the FTP commands.
// See the example for the USER command.
// If you think that you have to add other commands feel free to do so. You
// are allowed to add auxiliary methods if necessary.

void ClientConnection::WaitForRequests()
{
  if (!ok)
  {
    return;
  }

  fprintf(fd, "220 Service ready\n");

  while (!parar)
  {

    fscanf(fd, "%s", command);
    if (COMMAND("USER"))
    {
      fscanf(fd, "%s", arg);
      fprintf(fd, "331 User name ok, need password\n");
    }
    else if (COMMAND("PWD"))
    {
    }
    else if (COMMAND("PASS"))
    {
      fscanf(fd, "%s", arg);
      if (strcmp(arg, "1234") == 0)
      {
        fprintf(fd, "230 User logged in\n");
      }
      else
      {
        fprintf(fd, "530 Not logged in.\n");
        parar = true;
      }
    }
    else if (COMMAND("PORT"))
    {
      // To be implemented by students
      int h1, h2, h3, h4, p1, p2 = 0;
      fscanf(fd, "%d, %d, %d, %d, %d, %d", &h1, &h2, &h3, &h4, &p1, &p2);
      uint32_t data_address = (h1 << 24) | (h2 << 16) | (h3 << 8) | h4;
      uint16_t data_port = (p1 << 8) | p2;

      data_socket = connect_TCP(data_address, data_port);

      if (data_socket < 0)
      {
        fprintf(fd, "421 Service not available, closing control connection\n");
      }
      else
      {
        fprintf(fd, "200 Command okay\n");
      }
    }
    else if (COMMAND("PASV"))
    {
      // To be implemented by students
      int s;
      int sname;

      s = define_socket_TCP(0);
      struct sockaddr_in addr;
      socklen_t length = sizeof(addr);

      sname = getsockname(s, reinterpret_cast<sockaddr *>(&addr), &length);

      uint16_t port = addr.sin_port;
      uint8_t p1 = port;
      uint8_t p2 = port >> 8;

      fprintf(fd, "227 Entering Passive Mode (127,0,0,1,%d,%d)\n", p1, p2);
      fflush(fd);
      data_socket = accept(s, reinterpret_cast<sockaddr *>(&addr), &length);
    }
    else if (COMMAND("STOR"))
    {
      // To be implemented by students
      fscanf(fd, "%s", arg);
      FILE *file;
      file = fopen(arg, "wb");

      if (file == NULL)
      {
        fprintf(fd, "File empty\n");
      }
      else
      {
        fprintf(fd, "150 File creation ok, about to open data connection\n");
        fflush(fd);
        char buffer[1024];
        int bytes = 0;
        int written = 0;
        do
        {

          bytes = recv(data_socket, buffer, sizeof(buffer), 0);

          written = fwrite(buffer, 1, bytes, file);

          if (written <= 0)
          {
            fprintf(fd, "fatal error\n");
            fflush(fd);
            break;
          }

        } while (bytes == 1024);

        fprintf(fd, "226 Closing data connection\n");
        fflush(fd);
        close(data_socket);
        fclose(file);
      }
    }
    else if (COMMAND("RETR"))
    {
      // To be implemented by students
      fscanf(fd, "%s", arg);
      FILE *file;
      file = fopen(arg, "r");

      if (file == NULL)
      {
        fprintf(fd, "File empty\n");
      }
      else
      {
        fprintf(fd, "150 File status okay; about to open data connection\n");

        char buffer[1024];
        int buff = 0;

        do
        {
          buff = fread(buffer, 1, sizeof(buffer), file);
          send(data_socket, buffer, buff, 0);
        } while (buff == 1024);

        fprintf(fd, "226 Closing data connection\n");
        close(data_socket);
        fclose(file);
      }
    }
    else if (COMMAND("LIST"))
    {
      // To be implemented by students
      char direct[1024];

      DIR *mydir;
      struct dirent *entry;
      FILE *data;
      data = fdopen(data_socket, "a+");

      getcwd(direct, sizeof(direct));

      mydir = opendir(direct);

      if (mydir == NULL)
      {
        fprintf(fd, "450 Requested file action not taken\n");
        fflush(fd);
        exit(EXIT_FAILURE);
      }
      fprintf(fd, "125 List started OK\n");
      fflush(fd);

      while ((entry = readdir(mydir)) != NULL)
      {
        if (entry->d_name[0] != '.')
        {
          fprintf(data, "%s \n", entry->d_name);
          fflush(data);
        }
      }

      fclose(data);
      fprintf(fd, "250 List completed successfully\n");
      fflush(fd);

      closedir(mydir);
    }
    else if (COMMAND("SYST"))
    {
      fprintf(fd, "215 UNIX Type: L8.\n");
    }

    else if (COMMAND("TYPE"))
    {
      fscanf(fd, "%s", arg);
      fprintf(fd, "200 OK\n");
    }

    else if (COMMAND("QUIT"))
    {
      fprintf(fd, "221 Service closing control connection. Logged out if appropriate.\n");
      close(data_socket);
      parar = true;
      break;
    }

    else
    {
      fprintf(fd, "502 Command not implemented.\n");
      fflush(fd);
      printf("Comando : %s %s\n", command, arg);
      printf("Error interno del servidor\n");
    }
  }

  fclose(fd);

  return;
};
