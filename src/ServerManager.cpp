/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bylee <bylee@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/20 22:18:23 by bylee             #+#    #+#             */
/*   Updated: 2022/05/22 21:15:41 by bylee            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/ServerManager.hpp"

ServerManager::ServerManager(){

}
ServerManager::ServerManager(const ServerManager& ref){

}
ServerManager& ServerManager::operator=(const ServerManager& ref){

}
ServerManager::~ServerManager(){

}

void ServerManager::exitServer(const std::string& error_message){
  std::cout << error_message << std::endl;
  exit(1);
}

bool ServerManager::splitConfigString(std::string& config_string, std::string& config_block,\
  std::vector<std::string>& server_block){
  std::vector<std::string> lines = splitStringByChar(config_string);

  for (size_t i = 0; i < lines.size(); ++i)
    lines[i] = rtrimString(lines[i], " \t");
  server_block = groupLineWithCondition(lines, "server {", "}", INCLUDE_NOT);
  config_block = containerToString(lines, "\n");
  return (!config_block.empty() && server_block.size() != 0);
}

bool ServerManager::isValidConfigBlock(std::string& config_block){
  std::map<std::string, std::string>map_block = stringVectorToMap(splitStringByChar(config_block, '\n'), ' ');
  std::string key[4] = {"SOFTWARE_NAME", "SOFTWARE_VERSION", "HTTP_VERSION", "CGI_VERSION"};

  if (map_block.size() != 4)
    return (false);
  for (int i = 0; i < 4; ++i){
    if (!hasKey(map_block, key[i]))
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
  std::map<std::string, std::string>map_block = stringVectorToMap(splitStringByChar(server_block, '\n'), ' ');
  std::string key[6] = {"host", "port","REQUEST_URI_LIMIT_SIZE", "REQUEST_HEADER_LIMIT_SIZE", "DEFAULT_ERROR_PAGE", "LIMIT_CLIENT_BODY_SIZE"};

  if (map_block.size() < 6 || map_block.size() > 7)
    return (false);
  for (int i = 0; i < 6; ++i){
    if (!hasKey(map_block, key[i]))
      return (false);
  }
  if (map_block.size() == 7 && !hasKey(map_block, "server_name"))
    return (false);
  
  std::vector<std::string> ip_tokens = splitStringByChar(map_block.find(key[0])->second, '.');
  if (ip_tokens.size() != 4 || !std::all_of(ip_tokens.begin(), ip_tokens.end(), isValidIpByte))
    return (false);
  
  std::vector<std::string> ports = splitStringByChar(trimString(map_block["port"], "[]"), ',');

  return (true);
}

bool ServerManager::splitServerString(std::string server_string,\
  std::string& server_block, std::vector<std::string>& location_blocks){
  std::vector<std::string> lines = splitStringByChar(server_string);

  for (size_t i = 0; i < lines.size(); ++i)
    lines[i] = trimString(lines[i], " \t");
  location_blocks = groupLineWithCondition(lines, "location ", "}", INCLUDE_START);
  server_block = containerToString(lines, "\n");
  return (!server_block.empty() && location_blocks.size() != 0);
}

void  ServerManager::createServer(const std::string& config_file_path, char **env){
  std::string config_string = getStringFromFile(config_file_path);
  std::string config_block;
  std::vector<std::string> server_strings;

  if (!splitConfigString(config_string, config_block, server_strings))
    throw (std::invalid_argument("Failed to split config strings"));
  if (!isValidConfigBlock(config_block))
    throw(std::invalid_argument("Config block is not valid"));
  m_config = Config(config_block, env);
  for (size_t i = 0; i < server_strings.size(); ++i){
    std::string server_block;
    std::vector<std::string> location_blocks;
    if (!splitServerString(server_strings[i], server_block, location_blocks))
      throw(std::invalid_argument("Failed to split Server string"));
    if (!isValidServerBlock(server_block))
      throw(std::invalid_argument("Server block is not valid"));
  }
}
