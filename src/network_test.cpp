#include <assert.h>
#include <thread>
#include <vector>

#include "ZD/NetworkSocket.hpp"

#define PORT 6655

void tcp_server_loop()
{
  puts("Entered TCP server loop");

  auto server = NetworkSocket::server(SocketType::TCP, PORT);
  assert(server != nullptr);
  puts("Created TCP server");

  bool received = false;
  long iterations = 0;
  while (iterations++ < 6 && !received)
  {
    puts("TCP Server reading...");
    std::vector<uint8_t> data;
    data.resize(2048);
    auto sdata = server->read(data.data(), data.size());
    auto other = sdata.other_socket;
    assert(!data.empty());
    if (data.size() > 0)
    {
      printf("Received data size (iter=%lu): %lu\n", iterations, data.size());
      /*for (size_t i = 0; i < data.size(); i++)
      {
        printf("%c(%d)  ", data[i], data[i]);
      }*/
      printf("\n");
      assert(data.size() > 3);
      received = true;
      assert(data[0] == 'h');
      assert(data[1] == 'e');
      assert(data[2] == 'l');
      assert(data[3] == 'l');
      assert(data[4] == 'o');

      assert(other != nullptr);
      // send some data back
      std::vector<uint8_t> answer;
      answer.push_back('p');
      answer.push_back('o');
      answer.push_back('n');
      answer.push_back('g');
      puts("TCP server: sending back data...");
      int sent = other->send(answer.data(), answer.size());
      assert(sent > 0);

      break;
    }

    sleep(1);
  }
  assert(received);
}

void tcp_client_loop()
{
  puts("Entered TCP client loop");

  sleep(1);
  auto client = NetworkSocket::client(SocketType::TCP, "127.0.0.1", PORT);
  assert(client != nullptr);
  puts("Created TCP client");
  sleep(1);

  bool received = false;
  long iterations = 0;
  while (iterations++ < 5 && !received)
  {
    std::vector<uint8_t> data;
    data.push_back('h');
    data.push_back('e');
    data.push_back('l');
    data.push_back('l');
    data.push_back('o');
    data.resize(1024*16);
    data.push_back('c');
    data.push_back('a');
    data.push_back('t');
    puts("TCP client sending...");
    int ret = client->send(data.data(), data.size());
    assert(ret > 0);
    sleep(1);

    puts("TCP client reading...");
    std::vector<uint8_t> response;
    response.resize(2048);
    auto sdata = client->read(response.data(), response.size());
    assert(sdata.data_length > 0);
    if (sdata.data_length > 0)
    {
      received = true;
      auto vdata = sdata.data;
      auto other = sdata.other_socket;

      assert(vdata[0] == 'p');
      assert(vdata[1] == 'o');
      assert(vdata[2] == 'n');
      assert(vdata[3] == 'g');
      break;
    }

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
  while (iterations++ < 10 && !received)
  {
    std::vector<uint8_t> data;
    data.resize(2048);
    SocketData sdata = server->read(data.data(), data.size());
    data.resize(sdata.data_length);
    auto other = sdata.other_socket;
    assert(other);
    if (sdata.data_length > 0)
    {
      printf("Received data size (iter=%lu): %lu\n", iterations, data.size());
      for (size_t i = 0; i < data.size(); i++)
      {
        printf("%c(%d)  ", data[i], data[i]);
      }
      printf("\n");
      assert(data.size() > 3);
      received = true;
      assert(data[0] == 'h');
      assert(data[1] == 'e');
      assert(data[2] == 'l');
      assert(data[3] == 'l');
      assert(data[4] == 'o');

      // send some data back
      std::vector<uint8_t> answer;
      answer.push_back('p');
      answer.push_back('o');
      answer.push_back('n');
      answer.push_back('g');
      printf("UDP server sending..\n");
      other->send(answer.data(), answer.size());

      break;
    }

    sleep(1);
  }
  assert(received);
  puts("UDP server done.");
}

void udp_client_loop()
{
  puts("UDP Client loop entered");
  sleep(1);
  auto client = NetworkSocket::client(SocketType::UDP, "127.0.0.1", PORT);

  bool received = false;
  long iterations = 0;
  while (iterations++ < 3 && !received)
  {
    std::vector<uint8_t> data;
    data.push_back('h');
    data.push_back('e');
    data.push_back('l');
    data.push_back('l');
    data.push_back('o');
    int ret = client->send(data.data(), data.size());
    assert(ret > 0);
    if (ret > 0)
    {
      std::vector<uint8_t> response;
      response.resize(2048);
      auto sdata = client->read(response.data(), response.size());
      assert(sdata.data_length > 0);
      assert(sdata.other_socket != nullptr);
      auto data = sdata.data;
      assert(data[0] == 'p');
      assert(data[3] == 'g');
      received = true;
    }

    sleep(1);
  }
  assert(received);
  puts("UDP client done.");
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

  sleep(1);
  puts("-");

  std::thread tcp_server_thread(tcp_server_loop);
  std::thread tcp_client_thread(tcp_client_loop);
  puts("Threads initialized");

  tcp_client_thread.join();
  tcp_server_thread.join();
  puts("TCP Network test complete");
  return 0;
}
