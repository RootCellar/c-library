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

// Constants

#define CONNECTION_TIMEOUT 10 // seconds

// Struct(s)

struct receiving_buffer {
  // Used by user and implementation

  char* buffer;
  size_t buffer_size;

  // Used by implementation
  char* actual_buffer;
  size_t actual_size;

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
int has_flag(short value, short flag);

/*
 * Allocates memory for the given buffer and fills in the struct if it is successful.
 * If it is not successful, the function leaves the given buffer unchanged.
*/
void allocate_receiving_buffer(struct receiving_buffer* buffer, size_t size);


/*
 * Creates a receiving_buffer struct to be used in function calls like
 * read_buffer().
 * 
 * If the buffer pointer is a null pointer or the buffer_size is 0,
 * the function failed to create the buffer. Otherwise, it succeeded.
*/
struct receiving_buffer make_receive_buffer(size_t size);

/*
 * Frees the memory of the given receiving_buffer.
 * This function does not clear the fields of the given buffer.
*/
void free_receiving_buffer(struct receiving_buffer* buffer);

/*
 * Sends 'count' bytes of the buffer 'data' through the socket given by 'fd'
 * as a complete message.
 *
 * This function sends the size of the message just before sending the actual message.
 * This function blocks until the message has been fully sent.
 * 
*/
int send_buffer(int fd, char* data, int count);

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
int send_nothing(int fd);

/*
 * Checks to see if the socket given by the file descriptor 'fd' is connected.
 * 
 * Return values:
 * 1 : The socket is connected
 * -1 : The socket is not connected
 * 0 : The socket is in some other state. It could still be creating a connection, for example.
*/
int is_connected(int fd);

/*
 * Checks to see if the socket given by the file descriptor 'fd' has data that can be read.
 * 
 * Return values:
 * 1 : There is data to be read. A read() call on the socket should be non-blocking.
 * 0 : There is no data to read at the moment.
 * -1 : The socket has an error, and is probably disconnected.
*/
int has_data(int fd);

/*
 * Definitively tells whether or not the socket is disconnected.
 * Calls to this function will periodically call send_nothing() to check connectivity.
 *
 * Return values:
 *   -1 : Socket is disconnected.
 *   0 : The socket is still considered to be connected.
*/
int connection_keepalive(int fd, struct receiving_buffer* buffer);

/*
 * Checks to see if the socket given by the file descriptor 'fd' can be written to without blocking.
 * 
 * Return values:
 * 1 : A write() call on the socket should be non-blocking.
 * 0 : A write() call may block.
 * -1 : The socket has an error, and is probably disconnected.
*/
int can_write_non_blocking(int fd);

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
int read_buffer(int fd, struct receiving_buffer* buffer);

/*
 * Setup the given socket file descriptor
 * by enabling non-blocking usage and setting TCP_NODELAY
*/
int setup_socket_flags(int fd);

/*
 * Convenience function to send a regular string using send_buffer().
*/
int send_string(int fd, char* data);

/*
 * Attempts to accept an incoming connect on the listening socket given by 'fd'.
 *
 * Return values:
 * -1 : No connections to accept
 * -2 : The accept() function encountered an error.
 * >= 0 : The socket file descriptor of the newly received connection
*/
int accept_connection(int fd);

/*
 * Initiates a connection to the given host and port.
 * Note that the connection may not be fully established yet when this function returns.
 * Use is_connected() to check if the connection is established, or if connecting failed.
 *
 * Return values:
 * -1 : Error initiating the connection
 * >= 0 : The socket file descriptor of the newly created connection
*/
int create_connection(char* host, int port);

/*
 * Creates a listening socket on the given port, accepting connections from any interface.
 *
 * Return values:
 * -1 : Error while creating the listening socket
 * >= 0 : The socket file descriptor of the created listening socket
*/
int create_server_socket(int port);

#endif
