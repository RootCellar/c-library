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
#include <poll.h>

// Local libraries

#include "memory.h"


// Struct(s)


struct receiving_buffer {

  // Used by user and implementation

  void* buffer;
  int buffer_size;

  // Used by implementation
  char* actual_buffer;
  int actual_size;
  
  int message_size;
  int message_size_received;
  
  int received;
};


// Functions


int has_flag(short value, short flag) {
  return value & flag;
}

struct receiving_buffer make_receive_buffer(int size) {
  
  struct receiving_buffer buffer;
  buffer.buffer = NULL;
  buffer.buffer_size = 0;
  
  if(size < 8) {
    return buffer;
  }

  int actual_size = size + sizeof(int);
  
  void* ptr = tMalloc(actual_size);
  if(ptr == NULL) return buffer;

  buffer.actual_buffer = ptr;
  buffer.actual_size = actual_size;
  
  buffer.message_size = 0;
  buffer.message_size_received = 0;
  
  buffer.received = 0;

  buffer.buffer = ptr + sizeof(int);
  buffer.buffer_size = size;
  
  return buffer;
}

int is_connected(int fd) {
  errno = 0;

  struct pollfd poll_data;
  poll_data.fd = fd;
  poll_data.events = ~0;
  int result = poll(&poll_data, 1, 1);

  if(result == 0) {
    // Timed out
    return 0;
  }
  else if(result < 0) {
    // Error
    perror("is_connected");
    return -1;
  }
  else if(has_flag(poll_data.revents, POLLERR)) {
    // Socket error
    return -1;
  }

  return 1;
  
}

int has_data(int fd) {
  errno = 0;

  struct pollfd poll_data;
  poll_data.fd = fd;
  poll_data.events = POLLIN;
  int result = poll(&poll_data, 1, 1);

  if(result == 0) {
    // Timed out
    return 0;
  }
  else if(result < 0) {
    // Error
    perror("is_connected");
    return -1;
  }
  else if(has_flag(poll_data.revents, POLLERR)) {
    // Socket error
    return -1;
  }

  return 1;
  
}

int read_buffer(int fd, struct receiving_buffer* buffer) {
  errno = 0;

  if(buffer->message_size_received < sizeof(int)) {
  
    // Work on getting the size of the message
    if(!has_data(fd)) return 0;
    int amount_read = read(fd, buffer->actual_buffer + buffer->message_size_received, sizeof(int) - buffer->message_size_received);
    debug_printf("Read %d", amount_read);

    if(errno != 0 && errno != EAGAIN && errno != EWOULDBLOCK) {
      perror("read_buffer");
      return -1;
    }

    buffer->message_size_received += amount_read;
    
    if(buffer->message_size_received == sizeof(int)) {
      for(int i = sizeof(int) - 1; i >= 0; i--) {
        buffer->message_size <<= 8;
        buffer->message_size |= buffer->actual_buffer[i] & 0xFF;
      }
      debug_printf("Message size: %d", buffer->message_size);
    }
    
  }

  if(buffer->message_size > 0) {

    // Work on reading the message
    if(!has_data(fd)) return 0;
    int amount_left = buffer->message_size - buffer->received;
    debug_printf("amount_left %d", amount_left);
    int amount_read = read(fd, buffer->buffer + buffer->received, amount_left);
    debug_printf("Read %d", amount_read);

    if(errno != 0 && errno != EAGAIN && errno != EWOULDBLOCK) {
      perror("read_buffer");
      return -1;
    }

    buffer->received += amount_read;

    if(buffer->received == buffer->message_size) {
      // We have the full message
      int size = buffer->message_size;

      // Clear out buffer read data
      buffer->received = 0;
      buffer->message_size = 0;

      return size;
    }
    
    // Don't have a full message yet
    return 0;
    
  }
  
}

/*
 * Setup the given socket file descriptor
 * by enabling non-blocking usage and setting TCP_NODELAY
*/
int setup_socket_flags(int fd) {
  int flags = fcntl(fd, F_GETFL, 0);
  fcntl(fd, F_SETFL, flags | O_NONBLOCK | O_NDELAY);

  // flags = 1;
  // setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (char *) &flags, sizeof(int));

  return 1;
}

int send_buffer(int fd, char* data, int count) {
  errno = 0;

  int sent = 0;
  int total_sent = 0;

  // Send message size
  
  while(total_sent < sizeof(int)) {
    debug_printf("Count: %d", count);
    sent = write(fd, ((void*)&count) + total_sent, sizeof(int) - total_sent);

    if(sent >= 0) total_sent += sent;

    if(sent < 0 && errno != EAGAIN && errno != EWOULDBLOCK) {
      perror("send_buffer");
      return 1;
    }
  }

  // Send buffer

  sent = 0;
  total_sent = 0;
  
  while(total_sent < count) {
    sent = write(fd, data + total_sent, count - total_sent);

    if(sent >= 0) total_sent += sent;

    if(sent < 0 && errno != EAGAIN && errno != EWOULDBLOCK) {
      perror("send_buffer");
      return 1;
    }
  }

  return 0;
}

int send_string(int fd, char* data) {
  return send_buffer(fd, data, strlen(data));
}

int accept_connection(int fd) {
  errno = 0;

  struct sockaddr address;
  socklen_t addrlen = sizeof(address);
  
  int new_socket = accept(fd, &address, &addrlen);
  if(errno == EAGAIN || errno == EWOULDBLOCK) {
    return -1;
  }
  else if(errno != 0) {
    perror("accept");
    return -2;
  }

  debug_printf("Accepted a connection to fd %d", new_socket);

  return new_socket;
}

int create_connection(char* host, int port) {
  errno = 0;
  
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

  setup_socket_flags(sock);

  debug_printf("Connecting to host %s port %d...", host, port);

  int result = -1;
  result = connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
  if( result < 0 && errno != EINPROGRESS) {
    debug_printf("Failed connecting to host %s:%d!", host, port);
    perror("connect");
    return -1;
  }

  debug_printf("Returning fd %d", sock);

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
