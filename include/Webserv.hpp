/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bylee <bylee@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/17 17:15:09 by bylee             #+#    #+#             */
/*   Updated: 2022/05/24 16:54:39 by bylee            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# define DEFAULT_CONFIG_FILE_PATH "../config/default.conf"
# define REQUEST_URI_LIMIT_SIZE_MIN 64
# define REQUEST_URI_LIMIT_SIZE_MAX 8192
# define REQUEST_HEADER_LIMIT_SIZE_MIN 64
# define REQUEST_HEADER_LIMIT_SIZE_MAX 8192
# define LIMIT_CLIENT_BODY_SIZE_MAX 200000000
# define PORT_REGISTERED_MIN 1024
# define PORT_REGISTERED_MAX 49151
# define PORT_HTTP 80
# define PORT_HTTPS 443

# include <string>
# include <iostream>
# include <exception>
# include <vector>
# include <map>
# include <set>

# include <fcntl.h>
# include <unistd.h>
# include <sys/stat.h>
# include <sys/socket.h>
# include <sys/types.h>
# include <netinet/in.h>
# include <arpa/inet.h>

#endif
