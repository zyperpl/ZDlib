#include <assert.h>
#include <thread>
#include <vector>

#include "ZD/NetworkSocket.hpp"

#define PORT 6655

void tcp_server_loop()
{
  puts("Entered TCP server loop");

  auto server = NetworkSocket::server(SocketType::TCP, PORT);
  puts("Created TCP server");
  assert(server != nullptr);

  bool received = false;
  long iterations = 0;
  while (iterations++ < 10 && !received)
  {
    std::vector<uint8_t> data;
    auto d = server->read();
    if (d.size() > 0)
    {
      printf("Received data size (iter=%lu): %lu\n", iterations, d.size());
      for (size_t i = 0; i < d.size(); i++)
      {
        printf("%c(%d)  ", d[i], d[i]);
      }
      printf("\n");
      assert(d.size() > 3);
      received = true;
      assert(d[0] == 'h');
      assert(d[1] == 'e');
      assert(d[2] == 'l');
      assert(d[3] == 'l');
      assert(d[4] == 'o');
      break;
    }
    
    sleep(1);
  }
  assert(received);
}

void tcp_client_loop()
{
  puts("Entered TCP client loop");

  auto client = NetworkSocket::client(SocketType::TCP, "127.0.0.1", PORT);
  puts("Created TCP client");
  assert(client != nullptr);

  long iterations = 0;
  while (iterations++ < 3)
  {
    std::vector<uint8_t> data;
    data.push_back('h');
    data.push_back('e');
    data.push_back('l');
    data.push_back('l');
    data.push_back('o');
    int ret = client->send(data);
    assert(ret > 0);
    
    sleep(1);
  }
  puts("TCP Client: Done");
}

void udp_server_loop()
{
  puts("UDP Server loop entered");

  auto server = NetworkSocket::server(SocketType::UDP, PORT);
  assert(server != nullptr);

  bool received = false;
  long iterations = 0;
  while (iterations++ < 20 && !received)
  {
    auto d = server->read();
    if (d.size() > 0)
    {
      printf("Received data size (iter=%lu): %lu\n", iterations, d.size());
      for (size_t i = 0; i < d.size(); i++)
      {
        printf("%c(%d)  ", d[i], d[i]);
      }
      printf("\n");
      assert(d.size() > 3);
      received = true;
      assert(d[0] == 'h');
      assert(d[1] == 'e');
      assert(d[2] == 'l');
      assert(d[3] == 'l');
      assert(d[4] == 'o');
      break;
    }
    
    sleep(1);
  }
  assert(received);
}

void udp_client_loop()
{
  puts("UDP Client loop entered");
  auto client = NetworkSocket::client(SocketType::UDP, "", PORT);

  long iterations = 0;
  while (iterations++ < 3)
  {
    std::vector<uint8_t> data;
    data.push_back('h');
    data.push_back('e');
    data.push_back('l');
    data.push_back('l');
    data.push_back('o');
    int ret = client->send(data);
    assert(ret > 0);
    
    sleep(1);
  }
}

int network_test_main(int, char **)
{
  puts("Network test");

  std::thread udp_server_thread(udp_server_loop);
  std::thread udp_client_thread(udp_client_loop);
  puts("Threads initialized");

  udp_client_thread.join();
  udp_server_thread.join();
  puts("UDP Network test complete");
  
  std::thread tcp_server_thread(tcp_server_loop);
  std::thread tcp_client_thread(tcp_client_loop);
  puts("Threads initialized");

  tcp_client_thread.join();
  tcp_server_thread.join();
  puts("TCP Network test complete");
  return 0;
}
