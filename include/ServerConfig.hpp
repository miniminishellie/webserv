/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jihoolee <jihoolee@student.42SEOUL.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/23 14:40:53 by jihoolee          #+#    #+#             */
/*   Updated: 2022/05/23 14:56:54 by jihoolee         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_CONFIG_HPP_
# define SERVER_CONFIG_HPP_

# include <string>

class ServerConfig {
 public:
  ServerConfig();
  ServerConfig(const ServerConfig& copy);

  virtual ~ServerConfig(void);

  ServerConfig& operator=(const ServerConfig& operand);

 private:
  std::string m_server_name_;
  std::string m_host_;
  int         m_port_;
  size_t      m_request_uri_size_;
  size_t      m_request_header_size_limit_;
  std::string m_default_error_page_path_;
  size_t      m_client_body_size_limit_;
};  //  class ServerConfig
#endif //  SERVER_CONFIG_HPP_
