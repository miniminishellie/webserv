/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jihoolee <jihoolee@student.42SEOUL.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/18 21:23:44 by jihoolee          #+#    #+#             */
/*   Updated: 2022/06/09 21:58:28 by jihoolee         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONNECTION_HPP_
# define CONNECTION_HPP_

# include <sys/types.h>
# include <sys/socket.h>
# include <sys/time.h>
# include <dirent.h>
# include <vector>
# include <map>
# include <string>
# include <iostream>
# include "Request.hpp"
# include "Response.hpp"
# include "HtmlWriter.hpp"

# define BUFFER_SIZE 65536
# define CGI_SUCCESS_CODE 299
# define CGI_META_VARIABLE_COUNT 15

typedef std::vector<std::string> headers_t;

class WebservConfig;
class ServerConfig;
class ServerManager;

class Connection {
 public:
  enum Status { ON_WAIT, TO_SEND, ON_SEND, TO_EXECUTE, ON_EXECUTE, ON_RECV };

  /* Connection constructor & destructor */
  Connection(void);
  Connection(ServerManager* sm, ServerConfig* sc, int client_fd,
              std::string& client_ip, int client_port);
  Connection(const Connection &c);

  Connection &operator=(const Connection &operand);

  ~Connection(void);

  /* getter function */
  ServerConfig* get_m_server_config() const;
  Status      get_m_status() const;
  int         get_m_client_fd() const;
  timeval    get_m_last_request_at() const;
  std::string get_m_client_ip() const;
  int         get_m_client_port() const;

  int         get_m_read_from_server_fd() const;
  int         get_m_write_to_server_fd() const;

  int         get_m_wbuf_data_size() const;
  int         get_m_send_data_size() const;
  int                get_m_readed_size() const;
  std::string        get_m_read_buffer_client() const;
  const Request&     get_m_request() const;

  int         get_m_child_pid() const;

  /* setter function*/
  void set_m_status(Status status);
  void set_m_client_fd(int fd);
  void  set_m_last_request_at();
  void set_m_client_ip(std::string ip);
  void set_m_client_port(int port);

  void set_m_read_from_server_fd(int fd);
  void set_m_write_to_server_fd(int fd);

  void set_m_wbuf_data_size(int size);
  void set_m_sent_data_size(int size);
  void set_m_readed_size(int size);
  void set_m_read_buffer_client(std::string read_buffer);

  void set_m_wbuf_for_send(std::string wbuf_string = "");

  void set_m_child_pid(int pid);

  /* member function */
  bool  RunRecvAndSolve();
  void  RecvRequest();
  int   RecvWithoutBody(char *buf, int buf_size);
  bool  ParseStartLine();
  void  Parse(std::string start_line);
  void  addReadbufferClient(const char* str, int size);
  bool  ParseHeader();
  bool  IsValidHeader(std::string header);
  bool  IsRequestHasBody();
  int   RecvBody(char *buf, int buf_size);
  bool  ParseBody();
  int   getChunkedSize(std::string& str, std::string& len);
  bool  ReadGeneralBody();
  bool  ReadChunkedBody();
  void  makeResponse401();
  void  CreateResponse(int status, headers_t headers = headers_t(), std::string body = "");
  void  ReportCreateNewRequestLog(int status);
  std::string GetDateHeader();
  std::string GetServerHeader();
  void CreateCGIResponse(int& status, headers_t& headers, std::string& body);
  void SolveRequest();
  void WriteCreateNewRequestLog(const Request& request);
  void ExecuteAutoindex(const Request& request);
  void ExecuteGet(const Request& request);
  void ExecuteCGI(const Request& request);
  std::string GetExtension(std::string path);
  std::string  GetMimeTypeHeader(std::string path);
  time_t GetLastModified(std::string path);
  std::string GetLastModifiedHeader(std::string path);
  char** CreateCGIEnv(const Request& request);
  int	SetEnv(char **env, int idx, std::string key, std::string val);
  char** DupBaseEnvWithExtraSpace(const Request& request);
  std::string GetCGIEnvValue(const Request& request, std::string token);
  void Closes(int fd1 = -1, int fd2 = -1, int fd3 = -1, int fd4 = -1, int fd5 = -1);
  void ExecveCGI(const Request& request, char **env, int *parent_write_fd, int *child_write_fd);
  void ExecutePost(const Request& request);
  void ExecuteDelete(const Request& request);

  void ExecuteHead(const Request& request);
  void ExecutePut(const Request& request);

  bool runSend();
  void sendFromWbuf();
  void writeSendResponseLog(const Response& response);
  void clear();

  void addReadbufferServer(const char* str, int size);
  void writeChunkedBodyToCGIScript();
  void writeSavedBodyToCGIScript();
  bool runExecute(int mode);

 private:
  ServerManager*  m_server_manager_;
  WebservConfig*  m_webserv_config_;
  ServerConfig*   m_server_config_;

  Status          m_status_;
  int             m_client_fd_;
  timeval         m_last_request_at_;
  std::string     m_client_ip_;
  int             m_client_port_;
  int             m_read_from_server_fd_;
  int             m_write_to_server_fd_;
  std::string     m_wbuf_;
  int             m_wbuf_data_size_;
  int             m_send_data_size_;
  int             m_readed_size_;
  std::string     m_read_buffer_client_;
  std::string     m_read_buffer_server_;
  Request         m_request_;
  Response        m_response_;
  int             m_child_pid_;
};  //  class Connection
#endif  //  CONNECTION_HPP
