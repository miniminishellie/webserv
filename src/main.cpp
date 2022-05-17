/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jihoolee <jihoolee@student.42SEOUL.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/17 16:04:25 by jihoolee          #+#    #+#             */
/*   Updated: 2022/05/17 18:51:03 by jihoolee         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerManager.hpp"

int main(int argc, char *argv[], char *env[]) {
  ServerManager manager;

  if (argc > 2) {
    manager.exitServer("Too many arguments\n USAGE: ./webserv CONF_FILE_PATH");
  }
  return 0;
}
