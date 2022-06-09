/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jihoolee <jihoolee@student.42SEOUL.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/17 18:42:34 by jihoolee          #+#    #+#             */
/*   Updated: 2022/06/09 19:15:55 by jihoolee         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerManager.hpp"
#include "Libft.hpp"

int ServerManager::log_fd = -1;

ServerManager::ServerManager(void)
    : m_config_(),
      m_server_configs_(),
      m_connections_(),
      m_cgi_connection_map_(),
      m_kqueue_(-1),
      m_fd_set_(),
      m_change_list_() {
  memset(m_returned_events_, 0, sizeof(struct kevent) * 1024);
}

ServerManager::ServerManager(const ServerManager& ref)
    : m_config_(ref.m_config_),
      m_server_configs_(ref.m_server_configs_),
      m_connections_(ref.m_connections_),
      m_cgi_connection_map_(),
      m_kqueue_(ref.m_kqueue_),
      m_fd_set_(),
      m_change_list_(ref.m_change_list_) {
  memcpy(m_returned_events_, ref.m_returned_events_,
          sizeof(struct kevent) * 1024);
}

ServerManager::~ServerManager(void) {
  m_server_configs_.clear();
  m_connections_.clear();
  m_cgi_connection_map_.clear();
  m_kqueue_ = -1;
  m_fd_set_.clear();
  m_change_list_.clear();
}

ServerManager& ServerManager::operator=(const ServerManager& ref) {
  if (this == &ref)
    return *this;
  m_config_ = ref.m_config_;
  m_server_configs_ = ref.m_server_configs_;
  m_connections_ = ref.m_connections_;
  m_cgi_connection_map_ = ref.m_cgi_connection_map_;
  m_kqueue_ = ref.m_kqueue_;
  m_fd_set_ = ref.m_fd_set_;
  memcpy(m_returned_events_, ref.m_returned_events_,
          sizeof(struct kevent) * 1024);
  m_change_list_ = ref.m_change_list_;
  return *this;
}

WebservConfig* ServerManager::get_m_config() const {
  return const_cast<WebservConfig*>(&m_config_);
}

ServerManager::IOError::IOError() throw() : std::exception() {}

ServerManager::IOError::IOError(const char* msg) throw(): std::exception() {
  m_msg_ = std::string(msg);
}

ServerManager::IOError::IOError(const IOError& ioe) throw(): std::exception() {
  m_msg_ = ioe.m_msg_;
}

ServerManager::IOError::~IOError() throw(){}

ServerManager::IOError& ServerManager::IOError::operator=(
    const ServerManager::IOError& operand) throw() {
  if (this != &operand)
    m_msg_ = operand.m_msg_;
  return *this;
}

const char* ServerManager::IOError::what() const throw() {
  return "read/write operation return fail:";
}

std::string ServerManager::IOError::location() const throw() {
  return "read/write operation return fail: " + m_msg_;
}

bool  g_is_running;

void ServerManager::createServers(const std::string& config_file_path, char* env[]) {
  std::string config_string = ft::getStringFromFile(config_file_path); //config_string에 .conf 파일 통째로 string에 담김
  std::string config_block;
  std::vector<std::string> server_strings;

  if (!splitConfigString_(config_string, config_block, server_strings)) // config_block에 nginx directives가 string에 통쨰로, server_strings에 nginx directives 제외한 나머지들이 string container에 server block별로 담김
    throw (std::invalid_argument("Failed to split config strings"));
  if (!isValidConfigBlock_(config_block)) // config_block 유효성 검사
    throw(std::invalid_argument("Config block is not valid"));
  m_config_ = WebservConfig(config_block, env); // 유효성 검사 마친 config_block m_config에 생성자 호출해 정보 저장
  if ((m_kqueue_ = kqueue()) == -1)
    std::runtime_error("kqueue() Error");
  for (size_t i = 0; i < server_strings.size(); ++i){
    std::string server_block;
    std::vector<std::string> location_blocks;

    if (!splitServerString_(server_strings[i], server_block, location_blocks)) // server_block에 location block 들을 제외한 server directives가 string에 통째로, location_blocks에 locations block들이 한 블록씩 통째로 string으로 담김(특이하게 마지막 '}' 이 놈을 안담음)
      throw std::invalid_argument("Failed to split Server string");
    if (!isValidServerBlock_(server_block)) // server_block 유효성 검사
      throw std::invalid_argument("Server block is not valid");
    for (size_t j = 0; j < location_blocks.size(); ++j){
      if (!isValidLocationBlock_(location_blocks[j])) // location_blocks 유효성 검사
        throw std::invalid_argument("Location block(" + ft::to_string(i) + "-" +
                                      ft::to_string(j) + ") is not valid");
    }
    ServerConfig new_server =
        ServerConfig(&this->m_config_, server_block, location_blocks);
    addServer_(new_server);
  }
  writeCreateServerLog_();
}

void ServerManager::exitWebserv(const std::string& what) {
  std::cerr << what << std::endl;
  close(ServerManager::log_fd);
  exit(EXIT_SUCCESS);
}

void changeSignal(int sig) {
  (void)sig;
  ft::log(ServerManager::log_fd, "signal execute\n");
  g_is_running = false;
}

void ServerManager::runServers(void) {
  signal(SIGINT, changeSignal);
  g_is_running = true;

  struct timespec timeout;
  timeout.tv_nsec = 0;
  timeout.tv_sec = 0;
  int new_events;
  struct kevent* curr_event;

  while (g_is_running) {
    if ((new_events = kevent(m_kqueue_, &m_change_list_[0],
                              m_change_list_.size(), m_returned_events_,
                              1024, &timeout)) == -1) {
      ft::log(ServerManager::log_fd, "kqueue function failed\n");
      // throw std::runtime_error("kevent() Error");
      exitWebserv("kevent() ended\n");
    }
    m_change_list_.clear();
    for (int i = 0; i < new_events; ++i) {
      curr_event = &m_returned_events_[i];
      std::cout << "Connections: " << m_connections_.size() << std::endl;
      if (curr_event->flags & EV_ERROR) {
        throw std::runtime_error(("socket error in kevent with fd:"
                  + ft::to_string(curr_event->ident)).c_str());
      }
      else if (curr_event->filter == EVFILT_READ) {
        switch (m_fd_set_[curr_event->ident]) {
          case FD_SERVER: {
            if (m_connections_.size() + m_server_configs_.size() >= 1024) {
              int fd = getUnusedConnectionFd();

              if (fd == -1)
                continue;
              closeConnection(fd);
            }
            if (!acceptNewConnection_(curr_event->ident)) {
              std::cerr << "[Failed][Connection][Server:"
                      + m_server_configs_[curr_event->ident].get_m_server_name()
                      + "] Host: "
                      + m_server_configs_[curr_event->ident].get_m_host()
                      + "] Failed to create new connection.\n";
              continue;
            }
            break;
          }
          case FD_CLIENT: {
            Connection& curr_connection = m_connections_[curr_event->ident];
            Connection::Status connection_status = curr_connection.get_m_status();

            try {
              if (connection_status == Connection::ON_WAIT ||
                  connection_status == Connection::ON_RECV)
                curr_connection.RunRecvAndSolve();
              std::cout << "RunRecvAndSolve done" << std::endl;
            } catch(ServerManager::IOError& e) {
              ft::log(ServerManager::log_fd, ft::getTimestamp() + e.location() + std::string("\n"));
              closeConnection(curr_event->ident);
            } catch (...) {
              ft::log(ServerManager::log_fd, ft::getTimestamp() + "detected some error" + std::string("\n"));
              closeConnection(curr_event->ident);
            }
            break;
          }
          case FD_CGI: {
            Connection* curr_connection = m_cgi_connection_map_[curr_event->ident];
            Connection::Status connection_status = curr_connection->get_m_status();

            try {
              if (connection_status == Connection::ON_EXECUTE &&
                  curr_connection->get_m_read_from_server_fd() != 1)
                curr_connection->runExecute(CGI_READ);
            } catch(ServerManager::IOError& e) {
              ft::log(ServerManager::log_fd, ft::getTimestamp() + e.location() + std::string("\n"));
              closeConnection(curr_event->ident);
            } catch (...) {
              ft::log(ServerManager::log_fd, ft::getTimestamp() + "detected some error" + std::string("\n"));
              closeConnection(curr_event->ident);
            }
            break;
          }
        }
      } else if (curr_event->filter == EVFILT_WRITE) {
        switch (m_fd_set_[curr_event->ident]) {
          case FD_CLIENT: {
            Connection& curr_connection = m_connections_[curr_event->ident];
            Connection::Status connection_status = curr_connection.get_m_status();

            try {
              if (connection_status == Connection::TO_SEND ||
                  connection_status == Connection::ON_SEND)
                curr_connection.runSend();
            } catch(ServerManager::IOError& e) {
              ft::log(ServerManager::log_fd, ft::getTimestamp() + e.location() + std::string("\n"));
              closeConnection(curr_event->ident);
            } catch (...) {
              ft::log(ServerManager::log_fd, ft::getTimestamp() + "detected some error" + std::string("\n"));
              closeConnection(curr_event->ident);
            }
            break;
          }
          case FD_CGI: {
            Connection* curr_connection = m_cgi_connection_map_[curr_event->ident];
            Connection::Status connection_status = curr_connection->get_m_status();

            try {
              if (connection_status == Connection::ON_EXECUTE &&
                  curr_connection->get_m_read_from_server_fd() != 1)
                curr_connection->runExecute(CGI_READ);
            } catch(ServerManager::IOError& e) {
              ft::log(ServerManager::log_fd, ft::getTimestamp() + e.location() + std::string("\n"));
              closeConnection(curr_event->ident);
            } catch (...) {
              ft::log(ServerManager::log_fd, ft::getTimestamp() + "detected some error" + std::string("\n"));
              closeConnection(curr_event->ident);
            }
            break;
          }
          default:
            throw std::exception();
        }
      }
    }
    usleep(5);
  }
  exitWebserv("Webserv exited\n");
}

void ServerManager::insertFd(int fd, FdType type) {
  m_fd_set_[fd] = type;
}

void ServerManager::addEvent(uintptr_t ident,
                              int16_t filter,
                              uint16_t flags,
                              uint32_t fflags,
                              intptr_t data,
                              void* udata) {
  changeEvents_(m_change_list_, ident, filter, flags, fflags, data, udata);
}

void ServerManager::openLog(void) {
  std::string date = ft::getTimestamp();
  date = date.substr(1, date.size() - 2);
  std::string log_path = "log/" + date + "_log";
  if ((ServerManager::log_fd = open(log_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0755)) == -1)
    return;
}

int ServerManager::getUnusedConnectionFd(void) {
  for (std::map<int, Connection>::iterator it = m_connections_.begin();
        it != m_connections_.end();
        ++it) {
    if (it->second.get_m_status() == Connection::ON_WAIT)
      return it->first;
  }
  return -1;
}

void ServerManager::closeConnection(int client_fd) {
  writeCloseConnectionLog_(client_fd);
  if (close(client_fd) == -1)
    ft::log(ServerManager::log_fd,
            "[Failed]close function failed in closeConnection");
  this->changeEvents_(m_change_list_, client_fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);

  int cgi_read_fd = m_connections_[client_fd].get_m_read_from_server_fd();
  int cgi_write_fd = m_connections_[client_fd].get_m_write_to_server_fd();

  if (cgi_read_fd > 0) {
    this->changeEvents_(m_change_list_, cgi_read_fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
    close(cgi_read_fd);
    m_fd_set_.erase(cgi_read_fd);
  }
  if (cgi_write_fd > 0) {
    this->changeEvents_(m_change_list_, cgi_write_fd, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
    close(cgi_write_fd);
    m_fd_set_.erase(cgi_write_fd);
  }
  m_connections_.erase(client_fd);
  m_fd_set_.erase(client_fd);
}

void ServerManager::addCGIConnectionMap(int fd, Connection* connection) {
  m_cgi_connection_map_[fd] = connection;
}

void ServerManager::addServer_(const ServerConfig& new_server) {
  int server_socket_fd;

  if ((server_socket_fd = socket(PF_INET, SOCK_STREAM, 0)) == -1)
    throw std::runtime_error("socket() Error");

  int opt = 1;
  struct sockaddr_in serv_addr;

  setsockopt(server_socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = inet_addr(new_server.get_m_host().c_str());
  serv_addr.sin_port = htons(new_server.get_m_port());
  if (bind(server_socket_fd,
            reinterpret_cast<struct sockaddr*>(&serv_addr),
            sizeof(serv_addr)) == -1)
    throw std::runtime_error("bind() Error");
  if (listen(server_socket_fd, 256) == -1)  //  backlog 크기는 나중에 테스트 후 수정
    throw std::runtime_error("listen() Error");
  if (fcntl(server_socket_fd, F_SETFL, O_NONBLOCK) == -1)
    throw std::runtime_error("fcntl() Error");
  changeEvents_(m_change_list_, server_socket_fd, EVFILT_READ,
                  EV_ADD | EV_ENABLE, 0, 0, NULL);
  insertFd(server_socket_fd, FD_SERVER);
  m_server_configs_[server_socket_fd] = new_server;
}

void ServerManager::changeEvents_(std::vector<struct kevent>& change_list,
                                  uintptr_t ident,
                                  int16_t filter,
                                  uint16_t flags,
                                  uint32_t fflags,
                                  intptr_t data,
                                  void* udata) {
  struct kevent temp_event;

  EV_SET(&temp_event, ident, filter, flags, fflags, data, udata);
  change_list.push_back(temp_event);
}

bool ServerManager::acceptNewConnection_(int server_socket_fd) {
  struct sockaddr_in  client_addr;
  socklen_t           client_addr_size = sizeof(struct sockaddr);
  int                 client_fd;
  std::string         client_ip;
  int                 client_port;

  bzero(&client_addr, client_addr_size);
  if ((client_fd = accept(server_socket_fd,
        (struct sockaddr*)&client_addr, &client_addr_size)) == -1) {
    std::cerr << "accpt() function for client_fd failed" << std::endl;
    return false;
  }
  if (fcntl(client_fd, F_SETFL, O_NONBLOCK) == -1)
    return false;
  //  TO_CHECK
  // setsockopt(client_fd, SOL_SOCKET, SO_RCVTIMEO, (struct timeval*)&tv, sizeof(struct timeval));
  // setsockopt(client_fd, SOL_SOCKET, SO_SNDTIMEO, (struct timeval*)&tv, sizeof(struct timeval));
  addEvent(client_fd, EVFILT_READ, EV_ADD | EV_ENABLE,
            0, 0, NULL);
  client_ip = ft::inet_ntoa(client_addr.sin_addr.s_addr);
  client_port = static_cast<int>(client_addr.sin_port);
  m_connections_[client_fd] =
    Connection(this, &this->m_server_configs_[server_socket_fd],
                client_fd, client_ip, client_port);
  insertFd(client_fd, FD_CLIENT);
  std::cout << "Client Connection with fd: " << client_fd << " created! \n";
  std::cout << "client_ip: " << client_ip << "\n";
  std::cout << "client_port: " << client_port << "\n";
  return true;
}

void ServerManager::writeCreateServerLog_(void) {
  std::string msg = "[Created][Servers]" + ft::to_string(m_server_configs_.size()) + " servers created successfully.\n";
	ft::log(ServerManager::log_fd, msg);
}

void ServerManager::writeCloseConnectionLog_(int client_fd) {
	std::string text = "[Deleted][Connection][Server:" + \
  m_connections_[client_fd].get_m_server_config()->get_m_server_name() + "][CFD:" \
	+ ft::to_string(client_fd) + "] Connection closed.\n";
	ft::log(ServerManager::log_fd, text);
}

namespace {
enum IncludeMode { INCLUDE_START, INCLUDE_END, INCLUDE_BOTH, INCLUDE_NOT };

std::vector<std::string> groupLineWithCondition(std::vector<std::string>& lines,
                                                const std::string& start_line,
                                                const std::string& end_line,
                                                IncludeMode mode) {
  bool is_group_line = false;
  std::vector<std::string> result;
  std::vector<std::string> remain;

  for (size_t i = 0; i < lines.size(); ++i){
    std::string line = lines[i];
    if (line.empty())
      continue ;
    if (is_group_line == true){
      if (line.size() >= end_line.size() && line.substr(0, end_line.size()) == end_line){
        if (mode == INCLUDE_BOTH || mode == INCLUDE_END)
          result[result.size() - 1].append(line + "\n");
        is_group_line = false;
      }
      else
        result[result.size() - 1].append(line + "\n");
    }
    else if (line.size() >= start_line.size() && line.substr(0, start_line.size()) == start_line){
      is_group_line = true;
      if (mode == INCLUDE_BOTH || mode == INCLUDE_START)
        result.push_back(line + "\n");
      else
        result.push_back("");
    }
    else
      remain.push_back(line);
  }
  lines = remain;
  return (result);
}

bool isValidIpByte(std::string s) { return ((ft::stoi(s) >= 0) && (ft::stoi(s) <= 255)); }
bool isValidCgi(std::string data) { return (data[0] == '.'); }
bool isDigit(char c) { return (c >= '0' && c <= '9'); }

}  //  anonymous namespace

bool ServerManager::splitConfigString_(std::string& config_string, std::string& config_block,\
  std::vector<std::string>& server_block){
  std::vector<std::string> lines = ft::splitStringByChar(config_string);

  for (size_t i = 0; i < lines.size(); ++i)
    lines[i] = ft::rtrimString(lines[i], " \t");
  server_block = groupLineWithCondition(lines, "server {", "}", INCLUDE_NOT);
  config_block = ft::containerToString(lines, "\n");
  return (!config_block.empty() && server_block.size() != 0);
}

bool ServerManager::splitServerString_(std::string server_string,\
  std::string& server_block, std::vector<std::string>& location_blocks){
  std::vector<std::string> lines = ft::splitStringByChar(server_string);

  for (size_t i = 0; i < lines.size(); ++i)
    lines[i] = ft::trimString(lines[i], " \t");
  location_blocks = groupLineWithCondition(lines, "location ", "}", INCLUDE_START);
  server_block = ft::containerToString(lines, "\n");
  return (!server_block.empty() && location_blocks.size() != 0);
}

bool ServerManager::isValidConfigBlock_(std::string& config_block){
  std::map<std::string, std::string>map_block = ft::stringVectorToMap(ft::splitStringByChar(config_block, '\n'), ' ');
  std::string key[4] = {"SOFTWARE_NAME", "SOFTWARE_VERSION", "HTTP_VERSION", "CGI_VERSION"};

  if (map_block.size() != 4)
    return (false);
  for (int i = 0; i < 4; ++i){
    if (!ft::hasKey(map_block, key[i]))
      return (false);
  }
  if (map_block[key[0]].empty())
    return (false);
  if (map_block[key[1]].empty())
    return (false);
  if (map_block[key[2]] != "1.1")
    return (false);
  if (map_block[key[3]] != "1.1")
    return (false);
  return (true);
}

bool ServerManager::isValidServerBlock_(std::string& server_block){
  std::map<std::string, std::string>map_block = ft::stringVectorToMap(ft::splitStringByChar(server_block, '\n'), ' ');
  std::string key[6] = {"host", "port","REQUEST_URI_LIMIT_SIZE",\
    "REQUEST_HEADER_LIMIT_SIZE", "DEFAULT_ERROR_PAGE", "LIMIT_CLIENT_BODY_SIZE"};

  if (map_block.size() < 6 || map_block.size() > 7)
    return (false);
  for (int i = 0; i < 6; ++i){
    if (!ft::hasKey(map_block, key[i]))
      return (false);
  }
  if (map_block.size() == 7 && !ft::hasKey(map_block, "server_name"))
    return (false);

  std::vector<std::string> ip_tokens = ft::splitStringByChar(map_block.find(key[0])->second, '.');
  if (ip_tokens.size() != 4 || !std::all_of(ip_tokens.begin(), ip_tokens.end(), isValidIpByte))
    return (false);

  int port = std::atoi(map_block.find(key[1])->second.c_str());
  if (port != PORT_HTTP && port != PORT_HTTPS && (port < PORT_REGISTERED_MIN || PORT_REGISTERED_MAX > 49151))
    return (false);

  for (std::map<int, ServerConfig>::iterator it = m_server_configs_.begin();
        it != m_server_configs_.end();
        ++it)
    if (it->second.get_m_port() == port)
      return false;

  int uri_limit = std::atoi(map_block.find(key[2])->second.c_str());
  if (uri_limit < REQUEST_URI_LIMIT_SIZE_MIN || uri_limit > REQUEST_URI_LIMIT_SIZE_MAX)
    return (false);

  int header_limit = std::atoi(map_block.find(key[3])->second.c_str());
  if (header_limit < REQUEST_HEADER_LIMIT_SIZE_MIN || header_limit > REQUEST_HEADER_LIMIT_SIZE_MAX)
    return (false);

  int fd;
  if ((fd = open(map_block.find(key[4])->second.c_str(), O_RDONLY)) == -1)
    return (false);
  close(fd);

  int body_limit = std::atoi(map_block.find(key[5])->second.c_str());
  if (body_limit < 0 || body_limit > LIMIT_CLIENT_BODY_SIZE_MAX)
    return (false);
  return (true);
}

bool ServerManager::isValidLocationBlock_(std::string& location_block){
  std::map<std::string, std::string> map_block = \
    ft::stringVectorToMap(ft::splitStringByChar(location_block, '\n'), ' ');

  std::string key[] = {"location", "root", "allow_method", "auth_basic_realm",\
    "auth_basic_file", "index", "cgi", "autoindex", "limit_client_body_size"};
  std::set<std::string> key_set(key, key + sizeof(key) / sizeof(key[0]));

  if (map_block.size() < 2 || map_block.size() > 9)
    return (false);
  if (!ft::hasKey(map_block, "location") || !ft::hasKey(map_block, "root"))
    return (false);
  for (std::map<std::string, std::string>::iterator it = map_block.begin(); it != map_block.end(); ++it){
    if (!ft::hasKey(key_set, it->first) || it->second.empty())
      return (false);
  }

  std::vector<std::string> location = ft::splitStringByChar(ft::rtrimString(map_block[key[0]], " \t{"), ' ');
  if (location.size() != 1 || location[0].empty() || location[0][0] != '/')
    return (false);

  struct stat buf;
  std::string root = map_block[key[1]];
  stat(root.c_str(), &buf);
  if (!S_ISDIR(buf.st_mode) || root.empty() || (root != "/" && root.size() > 1 && root[root.size() - 1] == '/'))
    return (false);
  if ((ft::hasKey(map_block, key[3]) && !ft::hasKey(map_block, key[4]))
  || (!ft::hasKey(map_block, key[3]) && ft::hasKey(map_block, key[4])))
   return (false);
  if (ft::hasKey(map_block, key[4]))
  {
   stat(map_block[key[4]].c_str(), &buf);
   if (!S_ISREG(buf.st_mode))
     return (false);
  }
  if (ft::hasKey(map_block, key[2]))
  {
    std::set<std::string> data_set = ft::stringVectorToSet(ft::splitStringByChar(map_block[key[2]], ' '));
    std::string method[] = {"GET", "POST", "HEAD", "PUT", "DELETE", "TRACE", "OPTIONS"};
    std::set<std::string> method_set(method, method + sizeof(method) / sizeof(method[0]));
    if (data_set.empty())
      return (false);
    for (std::set<std::string>::iterator it = data_set.begin(); it != data_set.end(); ++it) {
      if ((*it).empty() || !ft::hasKey(method_set, *it))
        return (false);
    }
  }
  if (ft::hasKey(map_block, key[6])) {
    std::set<std::string> cgi_set = ft::stringVectorToSet(ft::splitStringByChar(map_block[key[6]], ' '));
    if (cgi_set.empty() || !std::all_of(cgi_set.begin(), cgi_set.end(), isValidCgi))
      return (false);
  }
  if (ft::hasKey(map_block, key[7])) {
    std::string autoindex = map_block[key[7]];
    if (autoindex != "on" && autoindex != "off")
      return (false);
  }
  if (ft::hasKey(map_block, key[8])) {
    std::string size = map_block[key[8]];
    if (size.empty() || !std::all_of(size.begin(), size.end(), isDigit))
      return (false);
  }
  return (true);
}
