/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plee <plee@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/18 21:23:44 by jihoolee          #+#    #+#             */
/*   Updated: 2022/05/27 19:41:18 by plee             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONNECTION_HPP_
# define CONNECTION_HPP_

# include <sys/types.h>
# include <sys/socket.h>
# include <sys/time.h>
# include <map>
# include <string>
# include <iostream>
# include "Request.hpp"
# include "ServerConfig.hpp"

# define BUFFER_SIZE 65536

typedef std::vector<std::string> headers_t;

class Connection {
 public:
  enum Status { ON_WAIT, TO_SEND, ON_SEND, TO_EXECUTE, ON_EXECUTE, ON_RECV };

  /* Connection constructor & destructor */
  Connection(void);
  Connection(const Connection &c);
  ~Connection(void);

  Connection &operator=(const Connection &operand);

  bool  RunRecvAndSolve();
  void  RecvRequest();
  int   RecvWithoutBody(char *buf, int buf_size);
  bool  ParseStartLine();
  void  Parse(std::string start_line);
  void  addReadbufferClient(const char* str, int size);
  bool  ParseHeader();
  bool  IsValidHeader(std::string header);
  void  AddHeader(std::string header);
  bool  IsRequestHasBody();
  int   RecvBody(char *buf, int buf_size);
  bool  ParseBody();
  int   getChunkedSize(std::string& str, std::string& len);
  bool  ReadGeneralBody();
  bool  ReadChunkedBody();
  void  CreateResponse(int status, headers_t headers = headers_t(), std::string body = "");
  

  /* getter function*/
  Status      get_m_status() const;
  int         get_m_client_fd() const;
  timeval    get_m_last_request_at() const;
  std::string get_m_client_ip() const;
  int         get_m_client_port() const;
  
  int                get_m_readed_size() const;
  std::string        get_m_read_buffer_client() const;
  const Request&     get_m_request() const;

  /* setter function*/
  void set_m_status(Status status);
  void set_m_client_fd(int fd);
  void  set_m_last_request_at();
  void set_m_client_ip(std::string ip);
  void set_m_client_port(int port);

  void set_m_readed_size(int size);
  void set_m_read_buffer_client(std::string read_buffer);

 private:
  Server*       m_server_;
  Status        m_status_;
  int           m_client_fd_;
  timeval       m_last_request_at_;
  std::string   m_client_ip_;
  int           m_client_port_;

  int           m_readed_size_;
  std::string   m_read_buffer_client_;
  Request       m_request_;
  ServerConfig  m_serverconfig_;
};
#endif
