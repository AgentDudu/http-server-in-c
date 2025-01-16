# C HTTP Server

This is a project for building a basic HTTP server in C. It implements core functionalities to handle client requests and deliver responses.

## Features

### Core HTTP Request Handling
- Parses HTTP requests (methods, URL, headers, body).
- Maps requests to appropriate handlers/routes.
- Supports basic HTTP methods (GET, POST).

### HTTP Response Generation
- Generates valid HTTP responses (status codes, headers, and body).
- Serves static files (HTML, CSS, images).

### Connection Handling
- Listens for incoming connections.
- Handles multiple requests sequentially (current implementation) or with basic concurrency (planned future feature).

### Error Handling
- Returns appropriate HTTP status codes (e.g., 404 for Not Found, 400 for Bad Request).

### Protocol Compliance
- Supports basic HTTP/1.1 standards (HTTP/2.0 support planned in future).

### Logging
- Logs incoming requests and responses for debugging.

## Getting Started

### Prerequisites
- C compiler (e.g., GCC)
- Basic understanding of C and network programming

### Building

```bash
make
```

This will create an executable file named `server`.

### Running

```bash
./server <port_number>
```

Replace `<port_number>` with the desired port number (default is 8080).

### Static Files

Place static files (HTML, CSS, images) in a directory named `public`. The server will attempt to serve files from this directory based on the requested URL.

## Contributing

We welcome contributions to this project! Please feel free to fork the repository, make changes, and submit a pull request.

## Notes

Please note: This server is for educational purposes and is not intended for production use. It lacks advanced features and security considerations necessary for a robust web server.
