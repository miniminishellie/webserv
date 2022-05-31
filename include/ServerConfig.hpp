/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jihoolee <jihoolee@student.42SEOUL.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/23 14:40:53 by jihoolee          #+#    #+#             */
/*   Updated: 2022/05/31 17:08:07 by jihoolee         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_CONFIG_HPP_
# define SERVER_CONFIG_HPP_

# include <iostream>
# include <string>
# include <vector>
# include "LocationConfig.hpp"

class WebservConfig;

class ServerConfig {
 public:
  ServerConfig();
  ServerConfig(WebservConfig* wc,
                const std::string& server_block,
                const std::vector<std::string>& location_blocks);
  ServerConfig(const ServerConfig& sc);

  virtual ~ServerConfig(void);

  ServerConfig& operator=(const ServerConfig& operand);

  WebservConfig*                      get_m_webserv_config(void) const;
  std::string                         get_m_server_name(void) const;
  std::string                         get_m_host(void) const;
  int                                 get_m_port(void) const;
  size_t                              get_m_request_uri_size_limit(void) const;
  size_t                              get_m_request_header_size_limit(void) const;
  size_t                              get_m_client_body_size_limit(void) const;
  const std::string&                  get_m_default_error_page_path(void) const;
  const std::vector<LocationConfig>&  get_m_locations(void) const;

 private:
  WebservConfig*              m_webserv_config_;
  std::string                 m_server_name_;
  std::string                 m_host_;
  int                         m_port_;
  size_t                      m_request_uri_size_limit_;
  size_t                      m_request_header_size_limit_;
  size_t                      m_client_body_size_limit_;
  std::string                 m_default_error_page_path_;
  std::vector<LocationConfig> m_locations_;
};  //  class ServerConfig

std::ostream& operator<<(std::ostream& out, const ServerConfig& config);

#endif //  SERVER_CONFIG_HPP_
