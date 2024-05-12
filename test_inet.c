#include <time.h>

#include "test.h"
#include "inet.h"

#define PORT 40001

void main() {
  int result = 0;
  debug_printf("CLOCKS_PER_SEC: %ld", CLOCKS_PER_SEC);

  int client_socket = create_connection("localhost", PORT);
  TEST( client_socket >= 0, "client socket can be created" );
  time_t start = clock();
  time_t now = start;
  while(now - start < CLOCKS_PER_SEC && result == 0) {
    result = is_connected(client_socket);
    now = clock();
  }

  debug_printf("connect result: %d", result);
  TEST( result < 0, "server can't be connected to" );

  int server_socket = create_server_socket(PORT);
  TEST( server_socket >= 0, "server socket created and bound" );

  TEST( accept_connection(server_socket) == -1, "accept_connection while no clients are connecting");

  client_socket = create_connection("localhost", PORT);
  TEST( client_socket >= 0, "client socket can be created" );
  result = 0;
  start = clock();
  now = start;
  while(now - start < CLOCKS_PER_SEC && result == 0) {
    result = is_connected(client_socket);
    now = clock();
  }

  debug_printf("connect result: %d", result);
  TEST( result == 1, "server can be connected to" );

  int server_fd_to_client = -1;
  start = clock();
  now = start;
  while(now - start < CLOCKS_PER_SEC && server_fd_to_client == -1) {
    server_fd_to_client = accept_connection(server_socket);
    now = clock();
  }
  TEST( server_fd_to_client >= 0, "accept_connection accepted a connection");

  char* to_write = "hello from the client!";
  TEST( send_string(client_socket, to_write) == 0, "client successfully sends a string to the server");

  struct receiving_buffer server_buffer = make_receive_buffer(128);
  TEST( server_buffer.buffer != NULL, "a valid server side buffer is created");
}
