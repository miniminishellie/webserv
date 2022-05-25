/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebservConfig.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jihoolee <jihoolee@student.42SEOUL.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/17 18:36:01 by jihoolee          #+#    #+#             */
/*   Updated: 2022/05/25 17:19:07 by jihoolee         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_CONFIG_HPP_
# define WEBSERV_CONFIG_HPP_

# include <iostream>
# include <string>

class WebservConfig {
 public:
  WebservConfig();
  WebservConfig(const WebservConfig& ref);
  WebservConfig(std::string config_block, char **env);

  virtual ~WebservConfig();

  WebservConfig& operator=(const WebservConfig& ref);

  std::string get_m_software_name() const;
  std::string get_m_software_version() const;
  std::string get_m_http_version() const;
  std::string get_m_cgi_version() const;
  char        **get_m_base_env() const;

 private:
  std::string m_software_name_;
  std::string m_software_version_;
  std::string m_http_version_;
  std::string m_cgi_version_;
  char**      m_base_env_;
};  //  class WebservConfig

std::ostream& operator<<(std::ostream& out, const WebservConfig& config);

#endif  //  WEBSERV_CONFIG_HPP_
