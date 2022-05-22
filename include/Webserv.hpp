/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bylee <bylee@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/17 17:15:09 by bylee             #+#    #+#             */
/*   Updated: 2022/05/22 16:28:32 by bylee            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
# define WEBSERV_HPP

enum IncludeMode { INCLUDE_START, INCLUDE_END, INCLUDE_BOTH, INCLUDE_NOT, };
# define DEFAULT_CONFIG_FILE_PATH "../conf/default.conf"

# include <string>
# include <iostream>
# include <exception>
# include <vector>
# include <map>

# include <fcntl.h>
# include <unistd.h>

#endif
