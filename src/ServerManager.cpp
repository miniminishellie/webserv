/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bylee <bylee@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/17 18:42:34 by jihoolee          #+#    #+#             */
/*   Updated: 2022/06/18 17:40:43 by bylee            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerManager.hpp"
#include "Libft.hpp"

int ServerManager::log_fd = -1;

ServerManager::ServerManager(void)
    : m_config_(),
      m_server_configs_(),
      m_connections_(),
      m_fdset_(),
      m_max_fd_(-1) {
  FD_ZERO(&m_read_set_);
  FD_ZERO(&m_read_copy_set_);
  FD_ZERO(&m_write_set_);
  FD_ZERO(&m_write_copy_set_);
  FD_ZERO(&m_error_copy_set_);
}

ServerManager::ServerManager(const ServerManager& ref)
    : m_config_(ref.m_config_),
      m_server_configs_(ref.m_server_configs_),
      m_connections_(ref.m_connections_),
      m_fdset_(ref.m_fdset_),
      m_max_fd_(ref.m_max_fd_),
      m_read_set_(ref.m_read_set_),
      m_read_copy_set_(ref.m_read_copy_set_),
      m_write_set_(ref.m_write_set_),
      m_write_copy_set_(ref.m_write_copy_set_),
      m_error_copy_set_(ref.m_error_copy_set_) { }

ServerManager::~ServerManager(void) {
  m_server_configs_.clear();
  m_connections_.clear();
  m_fdset_.clear();
}

ServerManager& ServerManager::operator=(const ServerManager& ref) {
  if (this == &ref)
    return *this;
  m_config_ = ref.m_config_;
  m_server_configs_ = ref.m_server_configs_;
  m_connections_ = ref.m_connections_;
  m_fdset_ = ref.m_fdset_;
  m_max_fd_ = ref.m_max_fd_;
  m_read_set_ = ref.m_read_set_;
  m_read_copy_set_ = ref.m_read_copy_set_;
  m_write_set_ = ref.m_write_set_;
  m_write_copy_set_ = ref.m_write_copy_set_;
  m_error_copy_set_ = ref.m_error_copy_set_;
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
  std::string config_string = ft::getStringFromFile(config_file_path); 
  std::string config_block;
  std::vector<std::string> server_strings;

  if (!splitConfigString_(config_string, config_block, server_strings)) 
    throw (std::invalid_argument("Failed to split config strings"));
  if (!isValidConfigBlock_(config_block)) 
    throw(std::invalid_argument("Config block is not valid"));
  m_config_ = WebservConfig(config_block, env); 
  for (size_t i = 0; i < server_strings.size(); ++i){
    std::string server_block;
    std::vector<std::string> location_blocks;

    if (!splitServerString_(server_strings[i], server_block, location_blocks))
      throw std::invalid_argument("Failed to split Server string");
    if (!isValidServerBlock_(server_block))
      throw std::invalid_argument("Server block is not valid");
    for (size_t j = 0; j < location_blocks.size(); ++j){
      if (!isValidLocationBlock_(location_blocks[j]))
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
  signal (SIGINT, changeSignal);
  g_is_running = true;

  struct timeval timeout;
  timeout.tv_sec = 0;
  timeout.tv_usec = 0;
  int count;

  resetMaxFd();
  while (g_is_running) {
    fdCopy(ALL_SET);
    if ((count = select(this->m_max_fd_ + 1, &m_read_copy_set_,
                        &m_write_copy_set_, NULL, &timeout)) == -1) {
      ft::log(ServerManager::log_fd, "Select function failed\n");
      throw std::runtime_error("select error");
    } else if (count == 0) {
      continue;
    }
    std::map<int, Connection>::iterator it = m_connections_.begin();

    while (it != m_connections_.end()) {
      std::map<int, Connection>::iterator it2 = it++;
      int fd = it2->first;

      try {
        if (it2->second.hasSendWork()) {
          it2->second.runSend();
        } else if (it2->second.hasExecuteWork()) {
          it2->second.runExecute();
        } else if (it2->second.hasRequest()) {
          it2->second.RunRecvAndSolve();
        }
      } catch (IOError& e) {
        ft::log(log_fd, ft::getTimestamp() + e.location() + std::string("\n"));
        closeConnection(fd);
      } catch (...) {
        ft::log(log_fd, ft::getTimestamp() + "detected unknown error\n");
        closeConnection(fd);
      }
    }
    for (std::map<int, ServerConfig>::iterator it = m_server_configs_.begin();
        it != m_server_configs_.end(); ++it) {
      if (fdIsset(it->first, READ_COPY_SET)) {
        if (m_connections_.size() + m_server_configs_.size() >= 1024) {
          int fd = getUnusedConnectionFd();

          if (fd == -1)
            continue;
          closeConnection(fd);
        }
        if (!acceptNewConnection_(it->first)) {
          ft::log(ServerManager::log_fd, "[Failed][Connection][Server:"
                      + m_server_configs_[it->first].get_m_server_name()
                      + "][Host: "
                      + m_server_configs_[it->first].get_m_host()
                      + "] Failed to create new connection.\n");
          throw std::runtime_error("acceptNewConnection error");
        }
      }
    }
    closeOldConnection_();
    resetMaxFd();
  }
  exitWebserv("Webserv exited\n");
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
  fdClear(client_fd, READ_SET);

  int cgi_read_fd = m_connections_[client_fd].get_m_read_from_server_fd();
  int cgi_write_fd = m_connections_[client_fd].get_m_write_to_server_fd();

  if (cgi_read_fd > 0) {
    fdClear(cgi_read_fd, READ_SET);
    fdClear(cgi_read_fd, READ_COPY_SET);
    close(cgi_read_fd);
  }
  if (cgi_write_fd > 0) {
    fdClear(cgi_write_fd, WRITE_SET);
    fdClear(cgi_write_fd, WRITE_COPY_SET);
    close(cgi_write_fd);
  }
  m_connections_.erase(client_fd);
}

void ServerManager::fdSet(int fd, SetType fdset) {
  switch (fdset) {
    case WRITE_SET: {
      FD_SET(fd, &m_write_set_);
      break;
    }
    case WRITE_COPY_SET: {
      FD_SET(fd, &m_write_copy_set_);
      break;
    }
    case READ_SET: {
      FD_SET(fd, &m_read_set_);
      break;
    }
    case READ_COPY_SET: {
      FD_SET(fd, &m_read_copy_set_);
      break;
    }
    case ERROR_COPY_SET: {
      FD_SET(fd, &m_error_copy_set_);
      break;
    }
    default:
      break;
  }
}

void ServerManager::fdZero(SetType fdset) {
  switch (fdset) {
    case WRITE_SET: {
      FD_ZERO(&m_write_set_);
      break;
    }
    case WRITE_COPY_SET: {
      FD_ZERO(&m_write_copy_set_);
      break;
    }
    case READ_SET: {
      FD_ZERO(&m_read_set_);
      break;
    }
    case READ_COPY_SET: {
      FD_ZERO(&m_read_copy_set_);
      break;
    }
    case ERROR_COPY_SET: {
      FD_ZERO(&m_error_copy_set_);
      break;
    }
    default:
      break;
  }
}

void ServerManager::fdClear(int fd, SetType fdset) {
  switch (fdset) {
    case WRITE_SET: {
      FD_CLR(fd, &m_write_set_);
      break;
    }
    case WRITE_COPY_SET: {
      FD_CLR(fd, &m_write_copy_set_);
      break;
    }
    case READ_SET: {
      FD_CLR(fd, &m_read_set_);
      break;
    }
    case READ_COPY_SET: {
      FD_CLR(fd, &m_read_copy_set_);
      break;
    }
    case ERROR_COPY_SET: {
      FD_CLR(fd, &m_error_copy_set_);
      break;
    }
    default:
      break;
  }
}

bool ServerManager::fdIsset(int fd, SetType fdset) {
  bool ret = false;

  switch (fdset) {
    case WRITE_SET: {
      ret = FD_ISSET(fd, &m_write_set_);
      break;
    }
    case WRITE_COPY_SET: {
      ret = FD_ISSET(fd, &m_write_copy_set_);
      break;
    }
    case READ_SET: {
      ret = FD_ISSET(fd, &m_read_set_);
      break;
    }
    case READ_COPY_SET: {
      ret = FD_ISSET(fd, &m_read_copy_set_);
      break;
    }
    case ERROR_COPY_SET: {
      ret = FD_ISSET(fd, &m_error_copy_set_);
      break;
    }
    default:
      break;
  }
  return ret;
}

void ServerManager::fdCopy(SetType fdset) {
  if (fdset == WRITE_SET || fdset == ALL_SET) {
    FD_ZERO(&m_write_copy_set_);
    m_write_copy_set_ = m_write_set_;
  }
  if (fdset == READ_SET || fdset == ALL_SET) {
    FD_ZERO(&m_read_copy_set_);
    m_read_copy_set_ = m_read_set_;
  }
  if (fdset == ERROR_SET || fdset == ALL_SET) {
    FD_ZERO(&m_error_copy_set_);
    m_error_copy_set_ = m_read_set_;
  }
}

void ServerManager::resetMaxFd(int new_max_fd) {
  if (new_max_fd != -1)
    m_max_fd_ = -1;
  else {
    for (int i = 512; i >= 0; --i) {
      if (fdIsset(i, READ_SET) || fdIsset(i, WRITE_SET)) {
        m_max_fd_ = i;
        break;
      }
    }
  }
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
  if (listen(server_socket_fd, 256) == -1) 
    throw std::runtime_error("listen() Error");
  if (fcntl(server_socket_fd, F_SETFL, O_NONBLOCK) == -1)
    throw std::runtime_error("fcntl() Error");
  fdSet(server_socket_fd, READ_SET);
  if (m_max_fd_ < server_socket_fd)
    m_max_fd_ = server_socket_fd;
  m_server_configs_[server_socket_fd] = new_server;
  m_fdset_.insert(server_socket_fd);
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
    ft::log(ServerManager::log_fd, "accept() function for client_fd failed\n");
    return false;
  }
  if (m_max_fd_ < client_fd)
    m_max_fd_ = client_fd;
  if (fcntl(client_fd, F_SETFL, O_NONBLOCK) == -1)
    return false;
  client_ip = ft::inet_ntoa(client_addr.sin_addr.s_addr);
  client_port = static_cast<int>(client_addr.sin_port);
  m_connections_[client_fd] =
    Connection(this, &this->m_server_configs_[server_socket_fd],
                client_fd, client_ip, client_port);
  fdSet(client_fd, READ_SET);
  return true;
}

void ServerManager::closeOldConnection_(void) {
  std::map<int, Connection>::iterator it = m_connections_.begin();
  while (it != m_connections_.end()) {
    int fd = it->first;
    if (!ft::hasKey(m_fdset_, fd) && it->second.isTimeOver()) {
      ++it;
      closeConnection(fd);
    } else {
      ++it;
    }
  }
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
    "auth_basic_file", "index", "cgi", "autoindex", "limit_client_body_size", "redirect"};
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
