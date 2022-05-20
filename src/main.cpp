/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bylee <bylee@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/20 21:33:56 by bylee             #+#    #+#             */
/*   Updated: 2022/05/20 22:20:29 by bylee            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/ServerManager.hpp"

int main(int argc, char *av[], char **env)
{
  ServerManager manager;

  if (argc > 2)
    exit_server("too many arguments!");
  else {
    try {
      if (argc == 2)
        manager.parse_config(av[1], env);
      else
        manager.parse_config(DEFAULT_CONFIG_FILE_PATH, env);
    }
    catch(const std::exception& e) {
      exit_server(e.what());
    }
  }
  return (0);
}
