/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plee <plee@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/23 16:26:19 by plee              #+#    #+#             */
/*   Updated: 2022/05/23 21:15:28 by plee             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONNECTION_HPP_
# define CONNECTION_HPP_

#include <map>
#include "libft.hpp"
#include "Request.hpp"
#include <sys/types.h>
#include <sys/socket.h>


# define BUFFER_SIZE 65536
# define REQUEST_URI_LIMIT_SIZE 1024
# define REQUEST_URI_LIMIT_SIZE_MIN 64
# define REQUEST_URI_LIMIT_SIZE_MAX 8192
# define REQUEST_HEADER_LIMIT_SIZE 8192
# define REQUEST_HEADER_LIMIT_SIZE_MIN 64
# define REQUEST_HEADER_LIMIT_SIZE_MAX 8192
# define LIMIT_CLIENT_BODY_SIZE 200000000

class Connection {
 public:
  enum Status { ON_WAIT, TO_SEND, ON_SEND, TO_EXECUTE, ON_EXECUTE, ON_RECV };
  Connection();
  Connection(const Connection &connection);
  Connection &operator=(const Connection &connection);
  ~Connection();
  void RecvRequest();
  int RecvWithoutBody(char *buf, int buf_size);
  bool ParseStartLine();
  void Parse(std::string start_line);
  bool ParseMethod(std::string method);
  void addReadbufferClient(const char* str, int size);
  bool ParseHeader();
  bool IsValidHeader(std::string header);
  void AddHeader(std::string header);
  
 private:
  int m_client_fd_;
  timeval m_last_request_at_;
  std::string m_client_ip_;
  int m_client_port_;
  Status m_status_;
  Request m_request_;
  std::string m_read_buffer_client_;
  int readed_size;

};
#endif