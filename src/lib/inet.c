
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

#include "inet.h"
#include "memory.h"
#include "debug.h"

// Constants

#define MESSAGE_SIZE_TYPE int
#define MESSAGE_SIZE_BYTES ((int)sizeof(MESSAGE_SIZE_TYPE))

// Functions

int has_flag(short value, short flag) {
  return value & flag;
}

void allocate_receiving_buffer(struct receiving_buffer* buffer, size_t size) {
  if(size < 8) {
    debug_printf("Refusing to make buffer with size %zu: too small!", size);
    return;
  }

  size_t actual_size = size + MESSAGE_SIZE_BYTES;

  void* ptr = malloc(actual_size);
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

  debug_printf("Allocated buffer with size %zu (Actual: %zu, %d for size)", size, actual_size, MESSAGE_SIZE_BYTES);
}

struct receiving_buffer make_receive_buffer(size_t size) {
  struct receiving_buffer buffer;
  buffer.buffer = NULL;
  buffer.buffer_size = 0;

  allocate_receiving_buffer(&buffer, size);

  debug_printf("Created buffer with size %zu", size);

  return buffer;
}

void free_receiving_buffer(struct receiving_buffer* buffer) {
  free(buffer->actual_buffer);
}

static
int send_bytes(int fd, void* data, int count) {
  errno = 0;

  int total_sent = 0;

  while(total_sent < count) {
    int sent = write(fd, data + total_sent, count - total_sent);

    if(sent >= 0) total_sent += sent;

    if(sent < 0 && errno != EAGAIN && errno != EWOULDBLOCK) {
      perror("send_buffer");
      return 1;
    }
  }

  return 0;
}

int send_buffer(int fd, char* data, int count) {

  // Send message size

  if(send_bytes(fd, &count, MESSAGE_SIZE_BYTES)) return 1;

  // Send buffer

  if(data == NULL) return 0;

  if (send_bytes(fd, data, count)) return 1;

  return 0;
}

int send_nothing(int fd) {
  return send_buffer(fd, NULL, 0);
}

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

int connection_keepalive(int fd, struct receiving_buffer* buffer) {
  time_t now = time(NULL);

  if(now < buffer->last_received || (now - buffer->last_received) > CONNECTION_TIMEOUT) {
    buffer->last_received = time(NULL);
    int result = send_nothing(fd);
    if(result) return -1;
  }

  return 0;
}

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

int read_bytes(int fd, char* buf, int nbytes) {
  int status = has_data(fd);
  if(status < 1) {
    return -1;
  }

  int amount_read = read(fd, buf, nbytes);

  if (errno != 0 && errno != EAGAIN && errno != EWOULDBLOCK) {
    perror("read_buffer");
    return -1;
  }

  return amount_read;
}

int read_buffer(int fd, struct receiving_buffer* buffer) {
  errno = 0;

  if(connection_keepalive(fd, buffer) < 0) {
    return -1;
  }

  if(buffer->message_size_received < MESSAGE_SIZE_BYTES) {
    // Work on getting the size of the message
    int amount_read = read_bytes(fd, buffer->actual_buffer + buffer->message_size_received,
                                  MESSAGE_SIZE_BYTES - buffer->message_size_received);

    if(amount_read > 0) buffer->message_size_received += amount_read;
    else if(amount_read < 0) return -1;
  }

  if(buffer->message_size_received == MESSAGE_SIZE_BYTES) {
    int* message_size = (int*) buffer->actual_buffer;
    buffer->message_size = *message_size;

    if(buffer->message_size < 1) {
      buffer->message_size_received = 0;
    }

    if(buffer->message_size > buffer->buffer_size) {
      debug_printf("Message of size %d is too large for buffer (%zu)!", buffer->message_size, buffer->buffer_size);
      return -1;
    }
  }

  if(buffer->message_size > 0) {
    // Work on reading the message
    int amount_read = read_bytes(fd, buffer->buffer + buffer->received,
                                  buffer->message_size - buffer->received);

    if(amount_read > 0) buffer->received += amount_read;
    else if(amount_read < 0) return -1;

  }

  if(buffer->received == buffer->message_size) {
    // We have the full message
    int size = buffer->message_size;

    // Clear out buffer read data
    buffer->received = 0;
    buffer->message_size = 0;
    buffer->message_size_received = 0;

    return size;
  }

  // Don't have a full message yet
  return 0;
}

int setup_socket_flags(int fd) {
  int flags = fcntl(fd, F_GETFL, 0);
  fcntl(fd, F_SETFL, flags | O_NONBLOCK | O_NDELAY);

  return 1;
}

int send_string(int fd, char* data) {
  return send_buffer(fd, data, strlen(data) + 1);
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

  int sock = socket(AF_INET, SOCK_STREAM, 0);
  if(sock < 0) {
    return -1;
  }

  struct sockaddr_in serv_addr;

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(port);

  if(inet_pton(AF_INET, host, &serv_addr.sin_addr) < 0) {
    perror("Invalid address/Address not supported");
    close(sock);
    return -1;
  }

  setup_socket_flags(sock);

  debug_printf("Connecting to host %s port %d...", host, port);

  int result = connect(sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr));
  if(result < 0 && errno != EINPROGRESS) {
    debug_printf("Failed connecting to host %s:%d!", host, port);
    perror("connect");
    return -1;
  }

  debug_printf("Returning fd %d", sock);

  return sock;
}

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
  address.sin_port = htons(port);

  debug_printf("Binding server socket to port %d...", port);

  result = bind(server_fd, (struct sockaddr*) &address, sizeof(address));

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
