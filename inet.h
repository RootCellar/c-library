#ifndef INET_H
#define INET_H

// Standard libraries

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Internet 

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>

// Utility

#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>

// Local libraries

#include "memory.h"



/*
 * Setup the given socket file descriptor
 * by enabling non-blocking usage and setting TCP_NODELAY
*/
int setup_socket_flags(int fd) {
  int flags = fcntl(fd, F_GETFL, 0);
  fcntl(fd, F_SETFL, flags | O_NONBLOCK);

  flags = 1;
  setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (char *) &flags, sizeof(int));

  return 1;
}

void send_buffer(int fd, char* data, int count) {
  int j;
  int sent = 0;
  while (sent < count) {
    j = write(fd, data + sent, count - sent);

    if(j >= 0) sent += j;

    if(j < 0 && errno != EAGAIN && errno != EWOULDBLOCK) {
      perror("send loop");
      break;
    }
  }
}

void send_string(int fd, char* data) {
  send_buffer(fd, data, strlen(data));
}

int create_connection(char* host, int port) {
  int sock = -1;
  sock = socket(AF_INET, SOCK_STREAM, 0);
  if(sock < 0) {
    return -1;
  }

  struct sockaddr_in serv_addr;

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(port);

  if(inet_pton(AF_INET, host, &serv_addr.sin_addr)<0)
  {
    perror("Invalid address/Address not supported");
    return -1;
  }

  debug_printf("Connecting to host %s port %d...", host, port);

  int result = -1;
  result = connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
  if( result < 0 ) {
    debug_printf("Failed connecting to host %s:%d!", host, port);
    return -1;
  }

  setup_socket_flags(sock);

  return sock;

}

int create_server_socket(int port) {
  int server_fd;
  struct sockaddr_in address;
  int addrlen = sizeof(address);
  int opt = 1;
  int result = 0;

  errno = 0;

  server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if(server_fd <= 0) {
    perror("socket");
    return -1;
  }

  // set socket options
  result = setsockopt(server_fd, SOL_SOCKET, SO_KEEPALIVE, &opt, sizeof(opt));
  if(result) {
    perror("setsockopt");
    return -1;
  }

  // Make server socket non-blocking
  setup_socket_flags(server_fd);

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons( port );

  debug_printf("Binding server socket to port %d...", port);

  result = bind(server_fd, (struct sockaddr *)&address, sizeof(address));

  if(result < 0) { 
    perror("bind");
    return -1;
  }

  debug_print("Enabling listen...");

  result = listen(server_fd, 3);
  if(result < 0) {
    perror("listen");
    return -1;
  }

  errno = 0;

  return server_fd;
}

#endif