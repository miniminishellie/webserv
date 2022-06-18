/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bylee <bylee@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/20 22:17:14 by bylee             #+#    #+#             */
/*   Updated: 2022/06/18 17:40:41 by bylee            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERMANAGER_HPP_
# define SERVERMANAGER_HPP_

# include <sys/time.h>
# include <sys/event.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <iostream>
# include <map>
# include <set>
# include <vector>
# include <string>
# include <exception>
# include "WebservConfig.hpp"
# include "ServerConfig.hpp"
# include "Connection.hpp"


# define DEFAULT_CONFIG_PATH  "config/default.conf"
# define PORT_HTTP 80
# define PORT_HTTPS 443
# define PORT_REGISTERED_MIN 1024
# define PORT_REGISTERED_MAX 49151

# define REQUEST_URI_LIMIT_SIZE_MIN 64
# define REQUEST_URI_LIMIT_SIZE_MAX 8192
# define REQUEST_HEADER_LIMIT_SIZE_MIN 64
# define REQUEST_HEADER_LIMIT_SIZE_MAX 8192
# define LIMIT_CLIENT_BODY_SIZE_MAX 200000000

class ServerManager {
 public:
  static int log_fd;
  /*
  ServerManager constructor & destructor declaration
  */
  enum FdType { FD_SERVER, FD_CLIENT, FD_CGI};
  enum CGIMode {CGI_READ, CGI_WRITE};
  enum SetType {
    WRITE_SET,
    WRITE_COPY_SET,
    READ_SET,
    READ_COPY_SET,
    ERROR_SET,
    ERROR_COPY_SET,
    ALL_SET
  };
  ServerManager();
  ServerManager(const ServerManager& ref);

  virtual ~ServerManager();

  ServerManager& operator=(const ServerManager& ref);

  WebservConfig* get_m_config() const;

  void  createServers(const std::string& config_file_path, char* env[]);
  void  exitWebserv(const std::string& error_message);
  void  runServers(void);

  void  openLog(void);
  int   getUnusedConnectionFd(void);
  void  closeConnection(int client_fd);

  void  fdSet(int fd, SetType fdset);
  void  fdZero(SetType fdset);
  void  fdClear(int fd, SetType fdset);
  bool  fdIsset(int fd, SetType fdset);
  void  fdCopy(SetType fdset);
  void  resetMaxFd(int new_max_fd = -1);


  class IOError: public std::exception{
   public:
    IOError() throw();
    IOError(const char* msg) throw();
    IOError(const IOError& ioe) throw();

    virtual ~IOError() throw();

    IOError& operator=(const IOError& operand) throw();

    virtual const char* what() const throw();
    std::string location() const throw();
   private:
    std::string m_msg_;
  };  //  class IOError

 private:
  void  addServer_(const ServerConfig& new_server);
  bool  acceptNewConnection_(int server_socket_fd);
  void  closeOldConnection_();

  void  writeCreateServerLog_(void);
  void  writeCloseConnectionLog_(int client_fd);

  bool  splitConfigString_(std::string& config_string,
          std::string& config_block,
          std::vector<std::string>& serveral_strings);
  bool splitServerString_(std::string server_string, std::string& server_block,
          std::vector<std::string>& location_blocks);
  bool isValidConfigBlock_(std::string& config_block);
  bool isValidServerBlock_(std::string& server_block);
  bool isValidLocationBlock_(std::string& location_block);

  WebservConfig                       m_config_;
  std::map<int, ServerConfig>         m_server_configs_;
  std::map<int, Connection>           m_connections_;


  std::set<int> m_fdset_;
  int           m_max_fd_;
  fd_set        m_read_set_;
  fd_set        m_read_copy_set_;
  fd_set        m_write_set_;
  fd_set        m_write_copy_set_;
  fd_set        m_error_copy_set_;

};  // class ServerManager

#endif  //  SERVER_MANGER_HPP_
