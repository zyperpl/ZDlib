#include <cassert>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "NetworkSocket.hpp"

#define MAX_BUFFER_SIZE 4096

NetworkSocket::NetworkSocket(int socket_fd, SocketType type)
: type { type }
, socket_fd { socket_fd }
{
}

NetworkSocket::~NetworkSocket()
{
  shutdown(socket_fd, SHUT_RDWR);
  close(socket_fd);
}

std::shared_ptr<NetworkSocket> NetworkSocket::server(SocketType type, int port)
{
  int t = type == SocketType::TCP ? SOCK_STREAM : SOCK_DGRAM;
  int fd = socket(AF_INET, t, 0);
  if (fd < 0)
  {
    fprintf(stderr, "Cannot create socket (fd=%d type=%d)\n", fd, t);
    return {};
  }

  auto ns = std::shared_ptr<NetworkSocket>(new NetworkSocket(fd, type));
  ns->port = port;

  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(port);

  if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
  {
    fprintf(stderr, "Socket bind error (fd=%d type=%d port=%d)\n", fd, t, port);
    return {};
  }

  if (SocketType::TCP == type)
  {
    listen(fd, SOMAXCONN);
  }

  return ns;
}

std::shared_ptr<NetworkSocket> NetworkSocket::client(
  SocketType type, std::string_view ip, int port)
{
  int t = type == SocketType::TCP ? SOCK_STREAM : SOCK_DGRAM;
  int fd = socket(AF_INET, t, 0);
  if (fd < 0)
  {
    fprintf(stderr, "Cannot create socket (fd=%d type=%d)\n", fd, t);
    return {};
  }

  auto ns = std::shared_ptr<NetworkSocket>(new NetworkSocket(fd, type));
  ns->ip = ip;
  ns->port = port;

  if (type == SocketType::TCP)
  {
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    if (inet_pton(AF_INET, ip.data(), &addr.sin_addr) <= 0)
    {
      fprintf(stderr, "Unknown address \"%s\"!\n", ip.data());
    }
    if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
      fprintf(
        stderr,
        "Cannot connect to %s at port %d (fd=%d)\n",
        ip.data(),
        port,
        fd);
      return {};
    }
  }

  return ns;
}

int NetworkSocket::send(std::vector<uint8_t> data)
{
  assert(socket_fd != -1);

  int ret = -1;

  switch (type)
  {
    case SocketType::TCP:
      ret = ::send(socket_fd, data.data(), data.size(), 0);
      break;
    case SocketType::UDP:
    {
      struct sockaddr_in addr;
      addr.sin_family = AF_INET;
      addr.sin_port = htons(port);
      addr.sin_addr.s_addr = INADDR_ANY;
      if (!ip.empty())
      {
        if (inet_pton(AF_INET, ip.data(), &addr.sin_addr) <= 0)
        {
          fprintf(stderr, "Unknown address \"%s\"!\n", ip.data());
        }
      }
      ret = ::sendto(
        socket_fd,
        data.data(),
        data.size(),
        0,
        (struct sockaddr *)&addr,
        sizeof(struct sockaddr));
    }
    break;
    default:
      puts("Invalid socket type");
      assert(false);
      break;
  }

  return ret;
}

std::vector<uint8_t> NetworkSocket::read()
{
  std::vector<uint8_t> data;
  data.resize(MAX_BUFFER_SIZE);
  int ret = -1;
  struct sockaddr_in addr;

  switch (type)
  {
    case SocketType::TCP:
      puts("not implemented");
      assert(false);
      break;
    case SocketType::UDP:
      {
        socklen_t len = sizeof(addr);
        ret = ::recvfrom(socket_fd, data.data(), data.size(), 0, (struct sockaddr*)&addr, &len);
      }
      break;
    default:
      puts("Invalid socket type");
      assert(false);
      break;
  }
  data.resize(ret);

  return data;
}
