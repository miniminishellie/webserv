/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Libft.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bylee <bylee@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/21 15:11:56 by bylee             #+#    #+#             */
/*   Updated: 2022/05/23 17:24:16 by bylee            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBFT_HPP
# define LIBFT_HPP

# include "Webserv.hpp"

namespace ft
{
  int stoi(std::string str, size_t base = 10);

  std::string                         ltrimString(const std::string& str, const std::string& seps = " ");
  std::string                         rtrimString(const std::string& str, const std::string& seps = " ");
  std::string                         trimString(const std::string& str, const std::string& seps = " ");
  std::string                         getStringFromFile(const std::string& file_path, int max_size = -1);
  std::vector<std::string>            splitStringByChar(std::string str, char c = '\n');
  std::map<std::string, std::string>  stringVectorToMap(std::vector<std::string> stringVector, char sep = ':');
  std::set<std::string>               stringVectorToSet(std::vector<std::string> stringVector);
  
  template <typename T, typename V>
  bool hasKey(T container, V value) { return (container.find(value) != container.end()); }

  template <typename T>
  std::string containerToString(T container, std::string sep){
    std::string result;
    for (typename T::iterator it = container.begin(); it != container.end(); it++){
      result.append(*it);
      if (++it != container.end())
        result.append(sep);
      --it;
    }
    return (result);
  }
}

#endif
