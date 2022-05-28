/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jihoolee <jihoolee@student.42SEOUL.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/25 21:44:42 by jihoolee          #+#    #+#             */
/*   Updated: 2022/05/28 20:08:20 by jihoolee         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Connection.hpp"

Connection::Connection(void) {}

Connection::Connection(const Connection &c)
    : m_status_(c.m_status_),
      m_client_fd_(c.m_client_fd_),
      m_last_request_at_(c.m_last_request_at_),
      m_client_ip_(c.m_client_ip_),
      m_client_port_(c.m_client_port_),
      m_readed_size_(c.m_readed_size_),
      m_read_buffer_client_(c.m_read_buffer_client_),
      m_request_(c.m_request_) {}

Connection &Connection::operator=(const Connection &operand) {
  m_status_ = operand.m_status_;
  m_client_fd_ = operand.m_client_fd_;
  m_last_request_at_ = operand.m_last_request_at_;
  m_client_ip_ = operand.m_client_ip_;
  m_client_port_ = operand.m_client_port_;
  m_readed_size_ = operand.m_readed_size_;
  m_read_buffer_client_ = operand.m_read_buffer_client_;
  m_request_ = operand.m_request_;
}

Connection(int client_fd, std::string& client_ip, int client_port) {}

void Connection::RecvRequest(void) {
  char buf[BUFFER_SIZE];
  int fd;
  int read_size = BUFFER_SIZE - m_read_buffer_client_.size();
  Request::Phase phase = m_request_.get_m_phase();
  m_status_ = ON_RECV;

  if (phase == Request::READY && (read_size = RecvWithoutBody(buf, sizeof(buf))) > 0)
    addReadbufferClient(buf, read_size);
  if (phase == Request::READY && ParseStartLine())
    phase = Request::ON_HEADER;
  if (phase == Request::ON_HEADER && ParseHeader()) {
    phase = Request::ON_BODY;
    if (!IsRequestHasBody())
      return ;
  }
  if (phase == Request::ON_BODY && (read_size = RecvBody(buf, sizeof(buf))) > 0)
    addReadbufferClient(buf, read_size);
  if (phase == Request::ON_BODY && ParseBody())
    phase = Request::COMPLETE;
  if (phase == Request::COMPLETE)
    set_m_last_request();
  m_request_.set_m_phase(phase);
}

Connection::~Connection() {}
