/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Libft.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plee <plee@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/21 15:11:56 by bylee             #+#    #+#             */
/*   Updated: 2022/05/26 20:20:54 by plee             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBFT_HPP_
# define LIBFT_HPP_

# include <unistd.h>
# include <iostream>
# include <fcntl.h>
# include <string>
# include <map>
# include <set>
# include <vector>
#include <sys/stat.h>

namespace ft {
int   stoi(std::string str, size_t base = 10);
void  bzero(void *data, size_t len);
void  strjoin(std::string& str, const std::string& buf, size_t n);
void  str_index_join(std::string& str, const std::string& buf, size_t i);
int   stoi(std::string str, size_t base = 10);
int   getLine(std::string& str, std::string &line, size_t buffer_size);
int   getNewLine(std::string& data, std::string& line);
bool  isFile(std::string path);
bool  isDirectory(std::string path);

std::string                         ltrimString(const std::string& str,
                                                const std::string& seps = " ");
std::string                         rtrimString(const std::string& str,
                                                const std::string& seps = " ");
std::string                         trimString(const std::string& str,
                                                const std::string& seps = " ");
std::string                         getStringFromFile(
                                      const std::string& file_path,
                                      int max_size = -1);
std::vector<std::string>            splitStringByChar(std::string str,
                                                      char c = '\n');
std::map<std::string, std::string>  stringVectorToMap(
                                      std::vector<std::string> stringVector,
                                      char sep = ':');
std::set<std::string>               stringVectorToSet(
                                      std::vector<std::string> stringVector);

template <typename T, typename V>
bool hasKey(T container, V value) {
  return (container.find(value) != container.end());
}

template <typename T>
std::string containerToString(T container, std::string sep) {
  std::string result;
  for (typename T::iterator it = container.begin();
        it != container.end();
        it++) {
    result.append(*it);
    if (++it != container.end())
      result.append(sep);
    --it;
  }
  return (result);
}
}  //  namespace ft

#endif  // endif
