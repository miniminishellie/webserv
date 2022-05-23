/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bylee <bylee@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/17 16:57:11 by bylee             #+#    #+#             */
/*   Updated: 2022/05/23 16:16:42 by bylee            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
# define CONFIG_HPP

# include "Webserv.hpp"

class Config
{
private:
  std::string m_software_name;
  std::string m_software_version;
  std::string m_http_version;
  std::string m_cgi_version;
  char**      m_base_env;
public:
  /*
    Config Constructor & Destructor declaration
  */
  Config();
  Config(const Config& ref);
  Config(std::string config_block, char **env);
  Config& operator=(const Config& ref);
  virtual ~Config();

  /*
    Getter declaration
  */
  std::string get_m_software_name() const;
  std::string get_m_software_version() const;
  std::string get_m_http_version() const;
  std::string get_m_cgi_version() const;
  char        **get_m_base_env() const;
};

std::ostream& operator<<(std::ostream& out, const Config& config);

#endif
