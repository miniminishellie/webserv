/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bylee <bylee@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/17 16:57:15 by bylee             #+#    #+#             */
/*   Updated: 2022/05/24 14:29:03 by bylee            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATION_HPP
# define LOCATION_HPP

# include "Webserv.hpp"
# include "Libft.hpp"

class Location
{
private:
  std::string                         m_uri;
  std::string                         m_root_path;
  std::string                         m_auth_basic_realm;
  std::map<std::string, std::string>  m_auth_basic_file;
  std::set<std::string>               m_allow_method;
  std::set<std::string>               m_index;
  std::set<std::string>               m_cgi;
  size_t                              m_limit_client_body_size;
  bool                                m_autoindex;

public:
  /*
    Location constructor & destructor declaration
  */
  Location();
  Location(const std::string& location_uri, std::string location_block, size_t server_limit_client_body_size);
  Location(const Location& ref);
  Location& operator=(const Location& ref);
  virtual ~Location();

  /*
    Location getter declaration
  */
  std::string get_m_uri() const;
  std::string get_m_root_path() const;
  std::string get_m_auth_basic_realm() const;
  const std::map<std::string, std::string>& get_m_auth_basic_file() const;
  std::set<std::string> get_m_allow_method() const;
  const std::set<std::string>& get_m_index() const;
  const std::set<std::string>& get_m_cgi() const;
  const bool& get_m_autoindex() const;
  size_t get_m_limit_client_body_size() const;
};

std::ostream&	operator<<(std::ostream& out, const Location& location);

#endif
