/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jihoolee <jihoolee@student.42SEOUL.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/23 14:40:53 by jihoolee          #+#    #+#             */
/*   Updated: 2022/05/25 17:53:24 by jihoolee         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_CONFIG_HPP_
# define SERVER_CONFIG_HPP_

# include <iostream>
# include <string>

class ServerConfig {
 public:
  ServerConfig();
  ServerConfig(const std::string& server_block);
  ServerConfig(const ServerConfig& sc);

  virtual ~ServerConfig(void);

  ServerConfig& operator=(const ServerConfig& operand);

  std::string                   get_m_server_name(void) const;
  std::string                   get_m_host(void) const;
  int                           get_m_port(void) const;
  size_t                        get_m_request_uri_size_limit(void) const;
  size_t                        get_m_request_header_size_limit(void) const;
  size_t                        get_m_client_body_size_limit(void) const;
  const std::string&            get_m_default_error_page_path(void) const;

 private:
  std::string m_server_name_;
  std::string m_host_;
  int         m_port_;
  size_t      m_request_uri_size_limit_;
  size_t      m_request_header_size_limit_;
  size_t      m_client_body_size_limit_;
  std::string m_default_error_page_path_;
};  //  class ServerConfig

std::ostream& operator<<(std::ostream& out, const ServerConfig& config);

#endif //  SERVER_CONFIG_HPP_
