/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bylee <bylee@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/20 21:33:56 by bylee             #+#    #+#             */
/*   Updated: 2022/05/21 16:52:38 by bylee            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/ServerManager.hpp"

int main(int argc, char *av[], char **env)
{
  ServerManager manager;

  if (argc > 2)
    manager.exitServer("too many arguments!");
  else {
    try {
      if (argc == 2)
        manager.createServer(av[1], env);
      else
        manager.createServer(DEFAULT_CONFIG_FILE_PATH, env);
    }
    catch(const std::exception& e) {
      manager.exitServer(e.what());
    }
  }
  return (0);
}
