#include <iostream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <regex>
#include <unordered_map>

#include "helpers.cpp"

const int16_t BUFFER_SIZE = 1024;

enum http_Code
{
  OK = 200,
  NOT_FOUND = 404
};

std::unordered_map<http_Code, std::string> HTTP_MESSAGE = {
    {OK, "HTTP/1.1 200 OK"},
    {NOT_FOUND, "HTTP/1.1 404 Not Found"}};

bool handle_client(int server_fd, int client_fd)
{

  char buffer[BUFFER_SIZE];

  ssize_t client_request = read(client_fd, buffer, BUFFER_SIZE);

  if (client_request <= 0)
  {
    std::cerr << "Client error" << std::endl;
  }

  std::string str_buffer(buffer);

  // std::cout << str_buffer << std::endl;  // Log to check request

  std::string path_request = "";
  std::string user_agent_request = "";
  
  std::smatch m;
  std::regex e("/([A-Za-z0-9\\-/]*)");

  std::vector<std::string> request_array = split_string(str_buffer, "\r\n");

  std::string start_path = request_array[0];

  for (int i = 0; i < request_array.size(); i++)
  {
    const std::string request_line = request_array[i];

    if (request_line.find("User-Agent:") != std::string::npos)
    {
      user_agent_request = request_line.substr(request_line.find_first_not_of("User-Agent:"));

      ltrim(user_agent_request);
    }
  }

  std::regex_search(start_path, m, e);

  path_request = m[0];

  // for (int i = 0; i < str_buffer.size(); i++)
  // {

  //   if (str_buffer[i] == '\n')
  //   {
  //     if (i - 1 > 0 && str_buffer[i - 1] != '\r')
  //     {
  //       std::cerr << "Incorrect request" << std::endl;
  //       return -1;
  //     }

  //     std::string start_path = str_buffer.substr(0, i - 1);

  //     std::regex_search(start_path, m, e);

  //     path_request = m[0];

  //     break;
  //   }
  // }

  http_Code code;
  std::string http_status_message;
  std::string http_body_message = "";
  std::string_view command = path_request.substr(0, path_request.find("/", 1));

  if (command == "/")
  {
    code = OK;
    http_status_message = HTTP_MESSAGE.at(code) + "\r\n";
  }
  else if (command == "/echo")
  {
    code = OK;

    http_body_message = path_request.substr(path_request.find("/", 1) + 1);

    std::cout << "body: " << http_body_message << std::endl;

    std::string size_echo_message = std::to_string(http_body_message.size());
    http_status_message = HTTP_MESSAGE.at(code) + "\r\n" + "Content-Type: text/plain\r\n" + "Content-Length: " + size_echo_message + "\r\n";
  }
  else if (command == "/user-agent")
  {
    code = OK;

    http_body_message = user_agent_request;

    std::cout << "body: " << http_body_message << std::endl;

    std::string size_user_agent_message = std::to_string(http_body_message.size());
    http_status_message = HTTP_MESSAGE.at(code) + "\r\n" + "Content-Type: text/plain\r\n" + "Content-Length: " + size_user_agent_message + "\r\n";
  }
  else
  {
    code = NOT_FOUND;
    http_status_message = HTTP_MESSAGE.at(code) + "\r\n";
  }

  std::string response = http_status_message + "\r\n" + http_body_message;

  ssize_t server_send = send(client_fd, response.c_str(), response.size(), 0);

  if (server_send == -1)
  {
    std::cerr << "Server failed to send buffer" << std::endl;
    return -1;
  }

  return 0;
}

int main(int argc, char **argv)
{
  // You can use print statements as follows for debugging, they'll be visible when running tests.
  std::cout << "Logs from your program will appear here!\n";

  // Uncomment this block to pass the first stage
  //
  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd < 0)
  {
    std::cerr << "Failed to create server socket\n";
    return 1;
  }

  // Since the tester restarts your program quite often, setting REUSE_PORT
  // ensures that we don't run into 'Address already in use' errors
  int reuse = 1;
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(reuse)) < 0)
  {
    std::cerr << "setsockopt failed\n";
    return 1;
  }

  struct sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(4221);

  if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) != 0)
  {
    std::cerr << "Failed to bind to port 4221\n";
    return 1;
  }

  int connection_backlog = 5;
  if (listen(server_fd, connection_backlog) != 0)
  {
    std::cerr << "listen failed\n";
    return 1;
  }

  struct sockaddr_in client_addr;
  int client_addr_len = sizeof(client_addr);

  std::cout << "Waiting for a client to connect...\n";

  ssize_t client_fd = accept(server_fd, (struct sockaddr *)&client_addr, (socklen_t *)&client_addr_len);

  if (client_fd < 0)
  {
    std::cerr << "Client failed to connect" << std::endl;
    close(client_fd);
    return 1;
  }

  std::cout << "Client connected\n";

  handle_client(server_fd, client_fd);

  std::cout << "Http status send correctly" << std::endl;

  close(server_fd);

  return 0;
}
