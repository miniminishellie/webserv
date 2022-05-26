/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jihoolee <jihoolee@student.42SEOUL.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/18 21:23:44 by jihoolee          #+#    #+#             */
/*   Updated: 2022/05/26 14:52:23 by jihoolee         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONNECTION_HPP_
# define CONNECTION_HPP_

# include <sys/types.h>
# include <sys/socket.h>
# include <sys/time.h>
# include <map>
# include <string>
# include "Request.hpp"

class Connection {
 public:
  enum Status { ON_WAIT, TO_SEND, ON_SEND, TO_EXECUTE, ON_EXECUTE, ON_RECV };

  Connection(void);
  Connection(const Connection &c);

  ~Connection(void);

  Connection &operator=(const Connection &operand);

  // GETTER && setter만들기..!
  void RecvRequest(void);

 private:
  Status      m_status_;
  int         m_client_fd_;
  timeval     m_last_request_at_;
  std::string m_client_ip_;
  int         m_client_port_;

  int         m_readed_size_;
  std::string m_read_buffer_client_;
  Request     m_request_;
};
#endif
