/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plee <plee@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/23 16:26:37 by plee              #+#    #+#             */
/*   Updated: 2022/05/23 16:26:37 by plee             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "libft.hpp"
#include "Connection.hpp"
#include "Server.hpp"
#include <map>

// # define BUFF_SIZE 65536
// # define REQUEST_URI_LIMIT_SIZE 1024
// # define REQUEST_URI_LIMIT_SIZE_MIN 64
// # define REQUEST_URI_LIMIT_SIZE_MAX 8192
// # define REQUEST_HEADER_LIMIT_SIZE 8192
// # define LIMIT_CLIENT_BODY_SIZE 200000000

class Request
{
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
  Request();
  Request(const Request &request);
  Request(Connection *connection, Server *server, std::string start_line);
  Request &operator=(const Request &request);
  ~Request();
  bool ParseMethod(std::string method);
  void AddContent(std::string added_content);
  bool AssignLocationMatchingUri(std::string uri);
  std::string ParseUri();

  //getter
  //std::string get_without_body() const;
  Phase get_m_phase() const;
  Method get_m_method() const;
  URIType get_m_uri_type() const;
  TransferType get_m_transfer_type() const;
  std::string get_m_uri() const;
  std::string get_m_protocol() const;
  std::map<std::string, std::string>& get_m_headers();
  std::string get_m_content() const;
  int get_m_content_length() const;
  timeval& get_m_start_at();

  //setter
  void set_m_phase(Phase phase);
  void set_m_method(Method method);
  void set_m_uri_type(URIType uritype);
  void set_m_transfer_type(TransferType transfertype);
  void set_m_uri(std::string uri);
  void set_m_protocol(std::string protocol);
  void set_m_headers(std::map<std::string, std::string> headers);
  void set_m_content(std::string content);
  void set_m_content_length(int content_length);

 private:
  Phase m_phase_;
  Method m_method_;
  URIType m_uri_type_;
  TransferType m_transfer_type_;
  timeval m_start_at_;
  Connection* m_connection_;
	Server *m_server_;
  std::string m_uri_;
  std::string m_protocol_;
  std::map<std::string, std::string> m_headers_;
  int m_special_header_count_;
  std::string m_content_;
  int m_content_length_;
};

template <typename T, typename V>
  bool hasKey(T container, V value) { return (container.find(value) != container.end()); }

#endif