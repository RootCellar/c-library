#include <time.h>

#include "test.h"
#include "inet.h"

void main() {
  int result = 0;

  int client_socket = create_connection("localhost", 40000);
  time_t start = clock();
  time_t now = start;
  while(now - start < CLOCKS_PER_SEC && result >= 0) {
    result = is_connected(client_socket);
    now = clock();
  }

  debug_printf("connect result: %d", result);
  TEST( result < 0, "server can't be connected to" );

  int server_socket = create_server_socket(40000);
  TEST( server_socket >= 0, "server socket created and bound" );

  TEST( accept_connection(server_socket) == -1, "accept_connection while no clients are connecting");

  client_socket = create_connection("localhost", 40000);
  TEST( client_socket >= 0, "server can be connected to" );

  int server_fd_to_client;
  server_fd_to_client = accept_connection(server_socket);
  TEST( accept_connection(server_socket) >= 0, "accept_connection accepted a connection");

  char* to_write = "hello from the client!";
  TEST( send_string(client_socket, to_write) == 0, "client successfully sends a string to the server");

  
}
