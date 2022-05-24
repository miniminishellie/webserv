/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jihoolee <jihoolee@student.42SEOUL.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/17 16:04:25 by jihoolee          #+#    #+#             */
/*   Updated: 2022/05/24 14:41:01 by jihoolee         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerManager.hpp"

int main(int argc, char *argv[], char *env[]) {
  ServerManager manager;

  try {
    if (argc > 2)
      throw(std::invalid_argument(
              "Too many arguments\n USAGE: ./webserv CONF_FILE_PATH"));
    else if (argc == 2)
      manager.createServers(argv[1], env);
    else
      manager.createServers(DEFAULT_CONFIG_PATH, env);
  } catch(std::exception& e) {
    manager.exitWebserv(e.what());
  }
  try {
    manager.runServers();
  } catch(std::exception& e) {
    manager.exitWebserv(e.what());
  }
  return 0;
}
