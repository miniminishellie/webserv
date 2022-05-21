/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bylee <bylee@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/20 22:18:23 by bylee             #+#    #+#             */
/*   Updated: 2022/05/21 22:13:53 by bylee            ###   ########.fr       */
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
  std::vector<std::string>& serveral_strings){
  std::vector<std::string> lines = splitStringByChar(config_string);

  for (size_t i = 0; i < lines.size(); ++i)
    lines[i] = rtrimString(lines[i], " \t");
  serveral_strings = groupLineWithCondition(lines, "server {", "}", INCLUDE_NOT);
  config_block = containerToString(lines, "\n");
  return (!config_block.empty() && serveral_strings.size() != 0);
}

void  ServerManager::createServer(const std::string& config_file_path, char **env){
  std::string config_string = getStringFromFile(config_file_path);
  std::string config_block;
  std::vector<std::string> server_strings;

  if (!splitConfigString(config_string, config_block, server_strings))
    throw (std::invalid_argument("Failed to split config strings"));
}
