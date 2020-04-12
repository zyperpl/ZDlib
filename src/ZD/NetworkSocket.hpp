#pragma once

#include <string_view>
#include <vector>
#include <memory>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

enum class SocketType
{
  TCP,
  UDP,
  Invalid
};

class NetworkSocket
{
public:
  ~NetworkSocket();

  static std::shared_ptr<NetworkSocket> server(SocketType type, int port);

  static std::shared_ptr<NetworkSocket> client(
    SocketType type, std::string_view ip, int port);

  int send(std::vector<uint8_t> data);
  std::vector<uint8_t> read();

  inline bool is_server() { return ip == ""; }

private:
  NetworkSocket(int socket_fd, SocketType type);
  SocketType type { SocketType::Invalid };
  std::string_view ip;
  int socket_fd { -1 };
  int port { -1 };
};
