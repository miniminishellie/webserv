/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plee <plee@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/21 15:40:04 by jihoolee          #+#    #+#             */
/*   Updated: 2022/05/28 19:33:37 by plee             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP_
# define RESPONSE_HPP_

# include <map>

class Connection;

class Response {
 public:
  enum ConnectionType { CLOSE, KEEP_ALIVE, };
  enum TransferType { GENERAL, CHUNKED };
  enum Phase { READY, ON_HEADER, ON_BODY, COMPLETE };
  static std::map<int, std::string> status;
  
  Response(void);
  Response(Connection* connection, int status_code, std::string start_line);
  Response(const Response& r);

  Response& operator=(const Response& ref);

  ~Response(void);

  /* getter function */
  Connection *get_m_connection() const;
  ConnectionType get_m_connection_type() const;
  int get_m_status_code() const;
  std::string get_m_status_description() const;
  const std::map<std::string, std::string>& get_m_headers() const;
  TransferType get_m_transfer_type() const;
  std::string get_m_content() const;
  Phase get_m_phase() const;

  /* setter function*/
  void addHeader(std::string header_key, std::string header_value);
  void addContent(const std::string& body);
  void set_m_status_code(int status_code);
  void set_m_transfer_type(TransferType transfer_type);
  void set_m_phase(Phase phase);
  void clear();

  /*member function*/
  std::string GetString() const;

 private:
  Connection* m_connection_;
  ConnectionType m_connection_type_;
  int m_status_code_;
  std::string m_status_description_;
  std::map<std::string, std::string> m_headers_;
  TransferType m_transfer_type_;
  std::string m_content_;
  Phase m_phase_;
};

std::map<int, std::string> make_status ();

#endif  //  RESPONSE_HPP_
