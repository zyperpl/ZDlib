#include <assert.h>
#include <thread>
#include <vector>

#include "ZD/NetworkSocket.hpp"

#define PORT 6655

void server_loop()
{
  puts("Server loop entered");

  auto server = NetworkSocket::server(SocketType::UDP, PORT);
  assert(server != nullptr);

  bool received = false;
  long iterations = 0;
  while (iterations++ < 1000 && !received)
  {
    auto d = server->read();
    if (d.size() > 0)
    {
      printf("Received data size: %lu\n", d.size());
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

void client_loop()
{
  puts("Client loop entered");
  auto client = NetworkSocket::client(SocketType::UDP, "", PORT);
  sleep(1);

  long iterations = 0;
  while (iterations++ < 4)
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

  std::thread server_thread(server_loop);
  std::thread client_thread(client_loop);
  puts("Threads initialized");

  client_thread.join();
  server_thread.join();
  puts("Network test complete");
  return 0;
}
