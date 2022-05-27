/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jihoolee <jihoolee@student.42SEOUL.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/17 18:42:34 by jihoolee          #+#    #+#             */
/*   Updated: 2022/05/27 20:07:50 by jihoolee         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerManager.hpp"
#include "Libft.hpp"

ServerManager::ServerManager(void)
    : m_is_running_(false),
      m_config_(),
      m_servers_(),
      m_kqueue_(-1),
      m_fd_set_(),
      m_change_list_() {
  memset(m_returned_events_, 0, sizeof(struct kevent) * 1024);
}

ServerManager::ServerManager(const ServerManager& ref)
    : m_is_running_(ref.m_is_running_),
      m_config_(ref.m_config_),
      m_servers_(ref.m_servers_),
      m_kqueue_(ref.m_kqueue_),
      m_fd_set_(),
      m_change_list_(ref.m_change_list_) {
  memcpy(m_returned_events_, ref.m_returned_events_,
          sizeof(struct kevent) * 1024);
}

ServerManager::~ServerManager(void) {
  m_is_running_ = false;
  m_servers_.clear();
  m_kqueue_ = -1;
  m_fd_set_.clear();
  m_change_list_.clear();
}

ServerManager& ServerManager::operator=(const ServerManager& ref) {
  if (this == &ref)
    return *this;
  m_is_running_ = ref.m_is_running_;
  m_config_ = ref.m_config_;
  m_servers_ = ref.m_servers_;
  m_kqueue_ = ref.m_kqueue_;
  m_fd_set_ = ref.m_fd_set_;
  memcpy(m_returned_events_, ref.m_returned_events_,
          sizeof(struct kevent) * 1024);
  m_change_list_ = ref.m_change_list_;
  return *this;
}

void  ServerManager::createServers(const std::string& config_file_path, char* env[]) {
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
        throw std::invalid_argument("Location block is not valid");
    }
    Server new_server =
        Server(this, &this->m_config_, server_block, location_blocks);

    addServer_(new_server);
    changeEvents_(m_change_list_, new_server.get_m_socket_fd(), EVFILT_READ,
                  EV_ADD | EV_ENABLE, 0, 0, NULL);\
    insertFd(new_server.get_m_socket_fd(), FD_SERVER);
    // std::cout << Server(this, &this->m_config_, server_block, location_blocks);
  }
}

void ServerManager::exitWebserv(const std::string& what) {
  std::cerr << what << std::endl;
  exit(EXIT_SUCCESS);
}

void ServerManager::runServers(void) {
  signal(SIGINT, this->changeSignal_);

  struct timeval  timeout;

  timeout.tv_sec = 0;
  timeout.tv_usec = 0;
  m_is_running_ = true;

  int new_events;
  struct kevent* curr_event;

  while (m_is_running_) {
    if ((new_events = kevent(m_kqueue_, &m_change_list_[0],
                              m_change_list_.size(), m_returned_events_,
                              1024, NULL)) == -1)
      throw std::runtime_error("kevent() Error");
    m_change_list_.clear();
    for (int i = 0; i < new_events; ++i) {
      curr_event = &m_returned_events_[i];
      if (curr_event->flags & EV_ERROR)
        throw std::runtime_error("socket error in kevent");
      else if (curr_event->flags & EVFILT_READ) {
        switch (m_fd_set_[curr_event->ident]) {
          case FD_SERVER: {
          // if (m_connections.size() >= (1024 / m_manager->get_m_servers().size())) {
          //    int fd = getUnuseConnectionFd();
          //    if (fd == -1)
          //      return ;
          //    closeConnection(fd);
          // }
            if (!m_servers_[curr_event->ident].acceptNewConnection()) {
              // std::cerr << "[Failed][Connection][Server:"
              //               + m_server_name + "][Host:" + m_host
	            //               + "] Failed to create new connection.\n"
              std::cerr << "accept new connection failed";
            }
            break;
          }
          case FD_CLIENT: {

            break;
          }
        }
      }
    }
  }
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

void ServerManager::addServer_(const Server& s) {
  if (m_servers_.count(s.get_m_socket_fd()) == 0)
    m_servers_[s.get_m_socket_fd()] = s;
}

void ServerManager::changeSignal_(int sig) {
  (void)sig;
  m_is_running_ = false;
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

  // for (std::vector<Server>::iterator it = m_servers_.begin(); it != m_servers_.end(); ++it) {
  //   if (it->get_m_config().get_m_port() == port)
  //     return (false);
  // }

  int uri_limit = std::atoi(map_block.find(key[2])->second.c_str());
  if (uri_limit < REQUEST_URI_LIMIT_SIZE_MIN || uri_limit > REQUEST_URI_LIMIT_SIZE_MAX)
    return (false);

  int header_limit = std::atoi(map_block.find(key[3])->second.c_str());
  if (header_limit < REQUEST_HEADER_LIMIT_SIZE_MIN || header_limit > REQUEST_HEADER_LIMIT_SIZE_MAX)
    return (false);

  // int fd;
  // if ((fd = open(map_block.find(key[4])->second.c_str(), O_RDONLY)) == -1)
  //   return (false);
  // close(fd);

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

  // struct stat buf;
  // std::string root = map_block[key[1]];
  // stat(root.c_str(), &buf);
  // if (!S_ISDIR(buf.st_mode) || root.empty() || (root != "/" && root.size() > 1 && root[root.size() - 1] == '/'))
  //   return (false);
  // if ((ft::hasKey(map_block, key[3]) && !ft::hasKey(map_block, key[4]))
  // || (!ft::hasKey(map_block, key[3]) && ft::hasKey(map_block, key[4])))
  //  return (false);
  // if (ft::hasKey(map_block, key[4]))
  // {
  //  stat(map_block[key[4]].c_str(), &buf);
  //  if (!S_ISREG(buf.st_mode))
  //    return (false);
  // }
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
