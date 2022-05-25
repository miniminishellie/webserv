/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jihoolee <jihoolee@student.42SEOUL.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/21 15:39:29 by jihoolee          #+#    #+#             */
/*   Updated: 2022/05/25 21:43:56 by jihoolee         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP_
# define REQUEST_HPP_

# include <sys/time.h>
# include <string>
# include <map>

class Server;
class Connection;

class Request {
 public:
  enum Phase {
    READY,
    ON_HEADER,
    ON_BODY,
    COMPLETE
  };

  enum Method {
    DEFAULT,
    GET,
    HEAD,
    POST,
    DELETE
  };

  enum URIType { DIRECTORY, FILE, CGI };
  enum TransferType { GENERAL, CHUNKED };
 private:
  Server*                             m_server_;
  Connection*                         m_connection_;
  timeval                             m_start_at_;
  Phase                               m_phase_;
  Method                              m_method_;
  URIType                             m_uri_type_;
  TransferType                        m_transfer_type_;
  std::string                         m_uri_;
  std::string                         m_protocol_;
  std::map<std::string, std::string>  m_headers_;
  int                                 m_special_header_count_;
  std::string                         m_query_;
  std::string                         m_script_translated_;
  std::string                         m_parth_translated_;
  std::string                         m_path_info_;
};
#endif  //  REQUEST_HPP_
