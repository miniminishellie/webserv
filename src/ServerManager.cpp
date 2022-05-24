/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bylee <bylee@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/20 22:18:23 by bylee             #+#    #+#             */
/*   Updated: 2022/05/24 21:36:50 by bylee            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/ServerManager.hpp"

ServerManager::ServerManager(){

}
ServerManager::ServerManager(const ServerManager& ref){
  m_servers = ref.m_servers;
  m_config = ref.m_config;
}
ServerManager& ServerManager::operator=(const ServerManager& ref){
  if (this == &ref)
    return (*this);
  m_servers = ref.m_servers;
  m_config = ref.m_config;
  return (*this);
}
ServerManager::~ServerManager(){

}

namespace
{
enum IncludeMode { INCLUDE_START, INCLUDE_END, INCLUDE_BOTH, INCLUDE_NOT, };

std::vector<std::string> groupLineWithCondition(std::vector<std::string>& lines, \
  const std::string& start_line, const std::string& end_line, IncludeMode mode){
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
} //namespace

bool ServerManager::splitConfigString(std::string& config_string, std::string& config_block,\
  std::vector<std::string>& server_block){
  std::vector<std::string> lines = ft::splitStringByChar(config_string);

  for (size_t i = 0; i < lines.size(); ++i)
    lines[i] = ft::rtrimString(lines[i], " \t");
  server_block = groupLineWithCondition(lines, "server {", "}", INCLUDE_NOT);
  config_block = ft::containerToString(lines, "\n");
  return (!config_block.empty() && server_block.size() != 0);
}

bool ServerManager::splitServerString(std::string server_string,\
  std::string& server_block, std::vector<std::string>& location_blocks){
  std::vector<std::string> lines = ft::splitStringByChar(server_string);

  for (size_t i = 0; i < lines.size(); ++i)
    lines[i] = ft::trimString(lines[i], " \t");
  location_blocks = groupLineWithCondition(lines, "location ", "}", INCLUDE_START);
  server_block = ft::containerToString(lines, "\n");
  return (!server_block.empty() && location_blocks.size() != 0);
}

bool ServerManager::isValidConfigBlock(std::string& config_block){
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

bool ServerManager::isValidServerBlock(std::string& server_block){
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

  // for (std::vector<Server>::iterator it = m_servers.begin(); it != m_servers.end(); ++it) {
  //   if (it->get_m_port() == port)
  //     return (false);
  // }

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

bool ServerManager::isValidLocationBlock(std::string& location_block){
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

void ServerManager::exitServer(const std::string& error_message){
  std::cout << error_message << std::endl;
  exit(1);
}

void  ServerManager::createServer(const std::string& config_file_path, char **env){
  std::string config_string = ft::getStringFromFile(config_file_path); //config_string에 .conf 파일 통째로 string에 담김
  std::string config_block;
  std::vector<std::string> server_strings;

  if (!splitConfigString(config_string, config_block, server_strings)) // config_block에 nginx directives가 string에 통쨰로, server_strings에 nginx directives 제외한 나머지들이 string container에 server block별로 담김
    throw (std::invalid_argument("Failed to split config strings"));
  if (!isValidConfigBlock(config_block)) // config_block 유효성 검사
    throw(std::invalid_argument("Config block is not valid"));
  m_config = Config(config_block, env); // 유효성 검사 마친 config_block m_config에 생성자 호출해 정보 저장
  for (size_t i = 0; i < server_strings.size(); ++i){
    std::string server_block;
    std::vector<std::string> location_blocks;
    if (!splitServerString(server_strings[i], server_block, location_blocks)) // server_block에 location block 들을 제외한 server directives가 string에 통째로, location_blocks에 locations block들이 한 블록씩 통째로 string으로 담김(특이하게 마지막 '}' 이 놈을 안담음)
      throw(std::invalid_argument("Failed to split Server string"));
    if (!isValidServerBlock(server_block)) // server_block 유효성 검사
      throw(std::invalid_argument("Server block is not valid"));
    for (size_t j = 0; j < location_blocks.size(); ++j){
      if (!isValidLocationBlock(location_blocks[j])) // location_blocks 유효성 검사
        throw (std::invalid_argument("Location block is not valid"));
    }
    std::cout << "count : " << i + 1 << std::endl;
    m_servers.push_back(Server(this, server_block, location_blocks, &this->m_config));
    std::cout << Server(this, server_block, location_blocks, &this->m_config);
  }
}
