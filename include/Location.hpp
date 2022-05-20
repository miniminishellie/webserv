/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bylee <bylee@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/17 16:57:15 by bylee             #+#    #+#             */
/*   Updated: 2022/05/20 21:35:10 by bylee            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATION_HPP
# define LOCATION_HPP

# include "Webserv.hpp"

class Location
{
private:
  std::string m_uri;
  std::string m_root_path;
public:
  Location();
  Location(const Location& ref);
  Location& operator=(const Location& ref);
  virtual ~Location();
};

#endif
