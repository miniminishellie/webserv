/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jihoolee <jihoolee@student.42SEOUL.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/23 20:24:20 by jihoolee          #+#    #+#             */
/*   Updated: 2022/05/23 21:03:23 by jihoolee         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerConfig.hpp"

ServerConfig::ServerConfig() {}

ServerConfig::ServerConfig(const ServerConfig& sc) {}

ServerConfig::~ServerConfig() {}

ServerConfig& ServerConfig::operator=(const ServerConfig& operand) {}

std::string ServerConfig::get_m_server_name(void) {
  return m_server_name_;
}

std::string ServerConfig::get_m_host(void) {
  return m_host_;
}

int ServerConfig::get_m_port(void) {
  return m_port_;
}

