/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Libft.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jihoolee <jihoolee@student.42SEOUL.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/21 15:11:56 by bylee             #+#    #+#             */
/*   Updated: 2022/05/31 17:11:00 by jihoolee         ###   ########.fr       */
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
void  bzero(void *data, size_t len);
void  *memcpy(void *dest, const void *src, size_t len);
void  strjoin(std::string& str, const std::string& buf, size_t n);
char  *strsjoin(std::string s1 = "", std::string s2 = "", std::string s3 = "", std::string s4 = "", std::string s5 = "");
void  str_index_join(std::string& str, const std::string& buf, size_t i);
int   stoi(std::string str, size_t base = 10);
char  *strdup(const char *s);
int   getLine(std::string& str, std::string &line, size_t buffer_size);
int   getNewLine(std::string& data, std::string& line);
bool  isFile(std::string path);
bool  isDirectory(std::string path);
void  log(int log_fd, std::string text);
void  convertTimespecToTm(time_t s, struct tm* t);
int   lenDoubleStr(char **str);
int   free(void *ptr);
int   freeStr(char **str);
int   freeDoublestr(char ***doublestr_addr);
std::string	getTimestamp(void);


std::string                         itos(std::string number, size_t from, size_t to);
long long int                       abs(long long int num);
std::string                         to_string(long long int n);
std::string                         ltrimString(const std::string& str,
                                                const std::string& seps = " ");
std::string                         rtrimString(const std::string& str,
                                                const std::string& seps = " ");
std::string                         trimString(const std::string& str,
                                                const std::string& seps = " ");
std::string                         getStringFromFile(
                                      const std::string& file_path,
                                      int max_size = -1);
std::string                         getStringFromFd(int fd, int max_size = -1);
std::vector<std::string>            splitStringByChar(std::string str,
                                                      char c = '\n');
std::map<std::string, std::string>  stringVectorToMap(
                                      std::vector<std::string> stringVector,
                                      char sep = ':');
std::set<std::string>               stringVectorToSet(
                                      std::vector<std::string> stringVector);
std::string to_string(long long int n);
std::string inet_ntoa(unsigned int address);

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
