/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationConfig.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jihoolee <jihoolee@student.42SEOUL.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/21 15:38:35 by jihoolee          #+#    #+#             */
/*   Updated: 2022/06/05 21:39:42 by jihoolee         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATION_CONFIG_HPP_
# define LOCATION_CONFIG_HPP_

# include <iostream>
# include <string>
# include <map>
# include <set>

class LocationConfig {
 public:
  LocationConfig(void);
  LocationConfig(const std::string& location_uri,
                  std::string location_block,
                  size_t server_limit_client_body_size);
  LocationConfig(const LocationConfig& ref);

  virtual ~LocationConfig(void);

  LocationConfig& operator=(const LocationConfig& ref);

  std::string                               get_m_uri() const;
  std::string                               get_m_root_path() const;
  std::string                               get_m_auth_basic_realm() const;
  const std::map<std::string, std::string>& get_m_auth_basic_file() const;
  std::set<std::string>                     get_m_allow_method() const;
  const std::set<std::string>&              get_m_index() const;
  const std::set<std::string>&              get_m_cgi() const;
  const bool&                               get_m_autoindex() const;
  size_t                                    get_m_limit_client_body_size()
                                              const;

 private:
  std::string                         m_uri_;
  std::string                         m_root_path_;
  std::string                         m_auth_basic_realm_;
  std::map<std::string, std::string>  m_auth_basic_file_;
  std::set<std::string>               m_allow_method_;
  std::set<std::string>               m_index_;
  std::set<std::string>               m_cgi_;
  size_t                              m_limit_client_body_size_;
  bool                                m_autoindex_;
};  //  class LocationConfig

std::ostream&	operator<<(std::ostream& out, const LocationConfig& location);

#endif  //  LOCATION_CONFIG_HPP_
