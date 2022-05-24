/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebservConfig.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bylee <bylee@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/17 18:36:01 by jihoolee          #+#    #+#             */
/*   Updated: 2022/05/24 22:08:44 by bylee            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_CONFIG_HPP_
# define WEBSERV_CONFIG_HPP_

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
