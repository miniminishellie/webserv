/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bylee <bylee@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/17 16:57:11 by bylee             #+#    #+#             */
/*   Updated: 2022/05/21 15:38:57 by bylee            ###   ########.fr       */
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
  Config& operator=(const Config& ref);
  virtual ~Config();
};

#endif
