/**
 *
 * Darian Marvel / RootCellar
 *
 * A simple library that provides packet-based messaging over TCP.
 *
 *
 * a few details:
 *   * This program assumes that the endian-ness of the client and server are the same
 *   * This program assumes that sizes for basic data types are the same on the client and server
 *   * The send and receive functions are only intended to work with both the server and the client using this same library
 *   * Function calls involving sockets may reset errno to 0
 *   * Reading is non-blocking
 *   * Initiating and accepting connections are non-blocking
 *   * Writing *may* block depending on kernel activity, but generally shouldn't for very long
 *
*/

#ifndef INET_H
#define INET_H

// Standard libraries

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

// Internet 

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>

// Utility

#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <poll.h>
#include <time.h>

// Local libraries

#include "memory.h"


// Constants


#define CONNECTION_TIMEOUT 10 // seconds

#define MESSAGE_SIZE_TYPE int
#define MESSAGE_SIZE_BYTES ((int)sizeof(MESSAGE_SIZE_TYPE))


// Struct(s)


struct receiving_buffer {

  // Used by user and implementation

  char* buffer;
  int buffer_size;

  // Used by implementation
  char* actual_buffer;
  int actual_size;
  
  int message_size;
  int message_size_received;
  
  int received;

  time_t last_received;
  
};


// Functions

/*
 * Checks whether or not the given value has the given flag.
 * This function can be used to check multiple flags
 * at the same time, like so: has_flag( my_value, VALUE_ONE | VALUE_TWO )
*/
int has_flag(short value, short flag) {
  return value & flag;
}

/*
 * Allocates memory for the given buffer and fills in the struct if it is successful.
 * If it is not successful, the function leaves the given buffer unchanged.
*/
void allocate_receiving_buffer(struct receiving_buffer* buffer, int size) {
  if(size < 8) {
    debug_printf("Refusing to make buffer with size %d: too small!", size);
    return;
  }

  int actual_size = size + MESSAGE_SIZE_BYTES;
  
  void* ptr = tMalloc(actual_size);
  if(ptr == NULL) {
    debug_print("Could not allocate space for buffer!");
    return;
  }

  buffer->actual_buffer = ptr;
  buffer->actual_size = actual_size;
  
  buffer->message_size = 0;
  buffer->message_size_received = 0;
  
  buffer->received = 0;

  buffer->buffer = (char*) ptr + MESSAGE_SIZE_BYTES;
  buffer->buffer_size = size;

  buffer->last_received = time(NULL);

  debug_printf("Allocated buffer with size %d (Actual: %d, %d for size)", size, actual_size, MESSAGE_SIZE_BYTES);
}


/*
 * Creates a receiving_buffer struct to be used in function calls like
 * read_buffer().
 * 
 * If the buffer pointer is a null pointer or the buffer_size is 0,
 * the function failed to create the buffer. Otherwise, it succeeded.
*/
struct receiving_buffer make_receive_buffer(int size) {
  
  struct receiving_buffer buffer;
  buffer.buffer = NULL;
  buffer.buffer_size = 0;
  
  allocate_receiving_buffer(&buffer, size);

  debug_printf("Created buffer with size %d", size);
  
  return buffer;
}

/*
 * Frees the memory of the given receiving_buffer.
 * This function does not clear the fields of the given buffer.
*/
void free_receiving_buffer(struct receiving_buffer* buffer) {
  tFree(buffer->actual_buffer);
}

/*
 * Sends 'count' bytes of the buffer 'data' through the socket given by 'fd'
 * as a complete message.
 *
 * This function sends the size of the message just before sending the actual message.
 * This function blocks until the message has been fully sent.
 * 
*/
int send_buffer(int fd, char* data, int count) {
  errno = 0;

  int sent;
  int total_sent = 0;

  // Send message size
  
  while(total_sent < MESSAGE_SIZE_BYTES) {
    sent = write(fd, ((char*)&count) + total_sent, MESSAGE_SIZE_BYTES - total_sent);

    if(sent >= 0) total_sent += sent;

    if(sent < 0 && errno != EAGAIN && errno != EWOULDBLOCK) {
      perror("send_buffer");
      return 1;
    }
  }

  // Send buffer

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

/*
 * Sends nothing through the given socket. send_buffer() will still send the message size,
 * which will be received as an ignored message on the other end of the socket.
 *
 * This is a sure method of determining whether or not the socket is still connected.
 *
 * Return values:
 *   1 : Could not send a message. Socket is disconnected.
 *   0 : Sending was successful. Socket seems to be connected.
*/
int send_nothing(int fd) {
  return send_buffer(fd, NULL, 0);
}

/*
 * Checks to see if the socket given by the file descriptor 'fd' is connected.
 * 
 * Return values:
 * 1 : The socket is connected
 * -1 : The socket is not connected
 * 0 : The socket is in some other state. It could still be creating a connection, for example.
*/
int is_connected(int fd) {
  errno = 0;

  struct pollfd poll_data;
  poll_data.fd = fd;
  poll_data.events = POLLOUT | POLLIN;
  int result = poll(&poll_data, 1, 1);

  if(result == 0) {
    // Timed out
    return 0;
  }
  else if(result < 0 || has_flag(poll_data.revents, POLLERR | POLLNVAL | POLLHUP)) {
    // Socket error
    perror("is_connected");
    return -1;
  }

  return 1;
}

/*
 * Checks to see if the socket given by the file descriptor 'fd' has data that can be read.
 * 
 * Return values:
 * 1 : There is data to be read. A read() call on the socket should be non-blocking.
 * 0 : There is no data to read at the moment.
 * -1 : The socket has an error, and is probably disconnected.
*/
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
  else if(result < 0 || has_flag(poll_data.revents, POLLERR | POLLNVAL | POLLHUP)) {
    // Socket error
    perror("has_data");
    return -1;
  }

  return 1;
}

/*
 * Definitively tells whether or not the socket is disconnected.
 * Calls to this function will periodically call send_nothing() to check connectivity.
 *
 * Return values:
 *   -1 : Socket is disconnected.
 *   0 : The socket is still considered to be connected.
*/
int connection_keepalive(int fd, struct receiving_buffer* buffer) {
  time_t now = time(NULL);

  if(now < buffer->last_received || (now - buffer->last_received) > CONNECTION_TIMEOUT) {
    buffer->last_received = time(NULL);
    int result = send_nothing(fd);
    if(result) return -1;
  }

  return 0;
}

/*
 * Checks to see if the socket given by the file descriptor 'fd' can be written to without blocking.
 * 
 * Return values:
 * 1 : A write() call on the socket should be non-blocking.
 * 0 : A write() call may block.
 * -1 : The socket has an error, and is probably disconnected.
*/
int can_write_non_blocking(int fd) {
  errno = 0;

  struct pollfd poll_data;
  poll_data.fd = fd;
  poll_data.events = POLLOUT;
  int result = poll(&poll_data, 1, 1);

  if(result == 0) {
    // Timed out
    return 0;
  }
  else if(result < 0 || has_flag(poll_data.revents, POLLERR | POLLNVAL | POLLHUP)) {
    // Socket error
    perror("can_write_non_blocking");
    return -1;
  }

  return 1;
}

/*
 * Reads from the socket given by 'fd' into the given buffer.
 *
 * - This function reads full messages.
 * - This function is non-blocking.
 *
 * Return values:
 * -1 : An error occurred during read
 * 0 : The function has not received a complete message yet.
 * > 0 : The function has received a full message. The return value is the size of the message in bytes.
*/
int read_buffer(int fd, struct receiving_buffer* buffer) {
  errno = 0;

  if(buffer->message_size_received < MESSAGE_SIZE_BYTES) {
  
    // Work on getting the size of the message
    if(connection_keepalive(fd, buffer) < 0) {
      return -1;
    }
    int status = has_data(fd);
    if(status < 1) {
      return status;
    }

    int amount_read = read(fd, buffer->actual_buffer + buffer->message_size_received, MESSAGE_SIZE_BYTES - buffer->message_size_received);

    if(errno != 0 && errno != EAGAIN && errno != EWOULDBLOCK) {
      perror("read_buffer");
      return -1;
    }

    if(amount_read > 0) buffer->message_size_received += amount_read;
    
    if(buffer->message_size_received == MESSAGE_SIZE_BYTES) {
      for(int i = MESSAGE_SIZE_BYTES - 1; i >= 0; i--) {
        buffer->message_size <<= 8;
        buffer->message_size |= buffer->actual_buffer[i] & 0xFF;
      }
      
      if(buffer->message_size == 0) {
        buffer->message_size_received = 0;
      }

      if(buffer->message_size > buffer->buffer_size) {
        debug_printf("Message of size %d is too large for buffer (%d)!", buffer->message_size, buffer->buffer_size);
        return -1;
      }
    }
    
  }

  if(buffer->message_size > 0) {

    // Work on reading the message
    if(connection_keepalive(fd, buffer) < 0) {
      return -1;
    }
    int status = has_data(fd);
    if(status < 1) {
      return status;
    }

    int amount_left = buffer->message_size - buffer->received;
    int amount_read = read(fd, buffer->buffer + buffer->received, amount_left);

    if(errno != 0 && errno != EAGAIN && errno != EWOULDBLOCK) {
      perror("read_buffer");
      return -1;
    }

    if(amount_read > 0) buffer->received += amount_read;

    if(buffer->received == buffer->message_size) {
      // We have the full message
      int size = buffer->message_size;

      // Clear out buffer read data
      buffer->received = 0;
      buffer->message_size = 0;
      buffer->message_size_received = 0;

      return size;
    }

  }

  // Don't have a full message yet
  return 0;

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

/*
 * Convenience function to send a regular string using send_buffer().
*/
int send_string(int fd, char* data) {
  return send_buffer(fd, data, strlen(data) + 1);
}

/*
 * Attempts to accept an incoming connect on the listening socket given by 'fd'.
 *
 * Return values:
 * -1 : No connections to accept
 * -2 : The accept() function encountered an error.
 * >= 0 : The socket file descriptor of the newly received connection
*/
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

/*
 * Initiates a connection to the given host and port.
 * Note that the connection may not be fully established yet when this function returns.
 * Use is_connected() to check if the connection is established, or if connecting failed.
 *
 * Return values:
 * -1 : Error initiating the connection
 * >= 0 : The socket file descriptor of the newly created connection
*/
int create_connection(char* host, int port) {
  errno = 0;

  int sock = socket(AF_INET, SOCK_STREAM, 0);
  if(sock < 0) {
    return -1;
  }

  struct sockaddr_in serv_addr;

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(port);

  if(inet_pton(AF_INET, host, &serv_addr.sin_addr)<0)
  {
    perror("Invalid address/Address not supported");
    close(sock);
    return -1;
  }

  setup_socket_flags(sock);

  debug_printf("Connecting to host %s port %d...", host, port);

  int result = connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
  if( result < 0 && errno != EINPROGRESS) {
    debug_printf("Failed connecting to host %s:%d!", host, port);
    perror("connect");
    return -1;
  }

  debug_printf("Returning fd %d", sock);

  return sock;
}

/*
 * Creates a listening socket on the given port, accepting connections from any interface.
 *
 * Return values:
 * -1 : Error while creating the listening socket
 * >= 0 : The socket file descriptor of the created listening socket
*/
int create_server_socket(int port) {
  struct sockaddr_in address;
  int opt = 1;

  errno = 0;

  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if(server_fd <= 0) {
    perror("socket");
    close(server_fd);
    return -1;
  }

  // set socket options
  int result = setsockopt(server_fd, SOL_SOCKET, SO_KEEPALIVE, &opt, sizeof(opt));
  if(result) {
    perror("setsockopt");
    close(server_fd);
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
