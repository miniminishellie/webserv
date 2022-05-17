/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jihoolee <jihoolee@student.42SEOUL.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/17 18:42:34 by jihoolee          #+#    #+#             */
/*   Updated: 2022/05/17 23:02:17 by jihoolee         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerManager.hpp"

void ServerManager::createServer(const std::string& conf_file_path,
                                  char* env[]) {

}

void ServerManager::exitServer(const std::string& what) {
  std::cerr << what << std::endl;
  exit(EXIT_SUCCESS);
}
