#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <memory>
#include <optional>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

class NetworkSocket;
struct SocketData;

enum class SocketType
{
  TCP,
  UDP,
  Invalid
};

struct SocketData
{
  std::shared_ptr<NetworkSocket> other_socket;
  std::vector<uint8_t> data;
};

class NetworkSocket : public std::enable_shared_from_this<NetworkSocket>
{
public:
  ~NetworkSocket();

  static std::shared_ptr<NetworkSocket> server(SocketType type, int port);

  static std::shared_ptr<NetworkSocket> client(
    SocketType type, std::string_view ip, int port);

  int send(std::vector<uint8_t> data);
  SocketData read();

  inline bool is_server() { return ip == ""; }

  int get_fd() const { return socket_fd; }
  int get_port() const { return port; }
  std::string_view get_ip() const { return ip; }

  static bool enable_broadcast;
private:
  NetworkSocket(int socket_fd, SocketType type);
  SocketType type { SocketType::Invalid };
  std::string ip;
  int socket_fd { -1 };
  int port { -1 };

  std::vector<std::shared_ptr<NetworkSocket>> other_sockets;

  std::optional<std::shared_ptr<NetworkSocket>> find_socket(std::string_view ip, int port);
};
