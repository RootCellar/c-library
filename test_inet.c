#include <signal.h>
#include <unistd.h>

#include "unit_testing.h"
#include "debug.h"
#include "time.h"
#include "inet.h"

#define BUFFER_SIZE 128

void main() {
  srand(time(NULL));
  signal(SIGPIPE,SIG_IGN);

  int port = (rand() % 2000) + 40000;
  debug_printf("Port: %d", port);

  int result = 0;
  debug_printf("CLOCKS_PER_SEC: %ld", CLOCKS_PER_SEC);

  int client_socket = create_connection("127.0.0.1", port);
  TEST( client_socket >= 0 || errno == ECONNREFUSED, "client socket can be created" );
  struct timespec start = get_time();
  struct timespec now = start;
  while(timespec_difference_seconds(start, now) < 2 && result == 0) {
    result = is_connected(client_socket);
    now = get_time();
    usleep(100);
  }

  debug_printf("connect result: %d", result);
  TEST( result <= 0, "server can't be connected to" );

  int server_socket = create_server_socket(port);
  TEST( server_socket >= 0, "server socket created and bound" );

  TEST( accept_connection(server_socket) == -1, "accept_connection while no clients are connecting");

  client_socket = create_connection("127.0.0.1", port);
  TEST( client_socket >= 0, "client socket can be created" );
  result = 0;
  start = get_time();
  now = start;
  while(timespec_difference_seconds(start, now) < 1 && result == 0) {
    result = is_connected(client_socket);
    now = get_time();
    usleep(100);
  }

  debug_printf("connect result: %d", result);
  TEST( result == 1, "server can be connected to" );

  int server_fd_to_client = -1;
  start = get_time();
  now = start;
  while(timespec_difference_seconds(start, now) < 1 && server_fd_to_client == -1) {
    server_fd_to_client = accept_connection(server_socket);
    now = get_time();
    usleep(100);
  }
  TEST( server_fd_to_client >= 0, "accept_connection accepted a connection");

  char* to_write = "hello from the client!";
  TEST( send_string(client_socket, to_write) == 0, "client successfully sends a string to the server");

  struct receiving_buffer server_buffer = make_receive_buffer(BUFFER_SIZE);
  TEST( server_buffer.buffer != NULL, "a valid server side buffer is created");

  result = 0;
  start = get_time();
  now = start;
  while(timespec_difference_seconds(start, now) < 1 && result == 0) {
    result = read_buffer(server_fd_to_client, &server_buffer);
    now = get_time();
    usleep(100);
  }
  TEST( result != -1, "server didn't get an error while reading");
  TEST( result == strlen(to_write) + 1, "server recieved correct number of bytes");

  TEST( strcmp(server_buffer.buffer, to_write) == 0, "server recieved correct data");

  // Client closes the connection, server would eventually send_nothing() and realize
  // connection is closed
  close(client_socket);
  send_nothing(server_fd_to_client);

  result = 0;
  start = get_time();
  now = start;
  while(timespec_difference_seconds(start, now) < 4 && result == 0) {
    result = read_buffer(server_fd_to_client, &server_buffer);
    now = get_time();
    usleep(100);
  }
  debug_printf("read result: %d", result);
  TEST( read_buffer(server_fd_to_client, &server_buffer) == -1, "server read_buffer() after socket is closed");
  TEST( send_string(client_socket, to_write) == 1, "client fails to send a string after close()");
  TEST( send_string(server_fd_to_client, to_write) == 1, "server fails to send a string after close()");

  free_receiving_buffer(&server_buffer);

  TEST( tGetTotalAllocs() == 0, "tGetTotalAllocs() after freeing memory");
  TEST( tGetTotalAllocSize() == 0, "tGetTotalAllocSize() after freeing memory");

  SHOW_TEST_RESULTS();

  tFreePointerList();

  exit(0);
  
}
