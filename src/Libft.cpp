/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Libft.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bylee <bylee@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/21 15:14:43 by bylee             #+#    #+#             */
/*   Updated: 2022/05/23 17:25:30 by bylee            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../include/Libft.hpp"

namespace ft
{
  int stoi(std::string str, size_t base)
  {
    int sign = 1;
    long long value = 0;
    long long int_max = INT_MAX;
    std::string digit = "0123456789abcdefghijklmnopqrstuvwxyz";

    if (str.empty())
      return (0);
    if (str[0] == '-')
      sign = -1;
    if (str[0] == '-' || str[0] == '+')
      str.erase(str.begin());
    if (str.empty() || digit.find(str[0]) == std::string::npos)
      return (0);
    while (!str.empty() && digit.find(str[0]) < base){
      value *= base;
      value += digit.find(str[0]);
      if ((sign == 1 && value > int_max) || (sign == -1 && value > int_max + 1))
        throw (std::overflow_error("stoi overflow"));
      str.erase(str.begin());
    }
    return (static_cast<int>(sign * value));
  }

  std::string ltrimString(const std::string& str, const std::string& seps)
  {
    std::string ret;
    int size = str.size();
    int idx = 0;

    if (size == 0)
      return (ret);
    while (idx < size && seps.find(str[idx]) != std::string::npos)
      ++idx;
    if (idx == size)
      return (ret);
    return (str.substr(idx));
  }

  std::string rtrimString(const std::string& str, const std::string& seps)
  {
    std::string ret;
    int size = str.size();
    int idx = size - 1;

    if (size == 0)
      return (ret);
    while (idx >= 0 && seps.find(str[idx]) != std::string::npos)
      --idx;
    if (idx == -1)
      return (ret);
    return (str.substr(0, idx + 1));
  }

  std::string trimString(const std::string& str, const std::string& seps) {
    return (rtrimString(ltrimString(str, seps), seps));
  }

  std::string getStringFromFile(const std::string& file_path, int max_size){
    int fd = -1;
    size_t read_byte = 0;
    char buf[1024];
    std::string result;

    if ((fd = open(file_path.c_str(), O_RDONLY)) == -1)
      throw(std::invalid_argument("Failed to open " + file_path));
    while ((read_byte = read(fd, buf, 1024)) > 0){
      result.append(buf, read_byte);
      if (max_size != -1 && static_cast<int>(result.size()) > max_size)
        throw (std::overflow_error("overflow max_size in getStringFromFile"));
    }
    close(fd);
    return (result);
  }

  std::vector<std::string> splitStringByChar(std::string str, char c){
    std::vector<std::string> result;
    size_t pos = 0;

    while ((pos = str.find(c)) != std::string::npos){
      if (pos != 0)
        result.push_back(str.substr(0, pos));
      str.erase(0, pos + 1);
    }
    if (str.length() != 0)
      result.push_back(str);
    return (result);
  }

  std::map<std::string, std::string> stringVectorToMap(std::vector<std::string> stringVector, char sep){
    std::map<std::string, std::string> result;

    if (stringVector.size() == 0)
      return result;
    for (size_t i = 0; i < stringVector.size(); ++i){
      if (stringVector[i].find(sep) == std::string::npos)
        throw (std::invalid_argument("Not found sep in string"));
      if (stringVector[i].find(sep) == 0)
        throw (std::invalid_argument("Not found key in string"));
      std::string data = trimString(stringVector[i], " \t");
      std::string key = data.substr(0, data.find(sep));
      std::string value;
      if (stringVector[i].find(sep) < stringVector[i].size() - 1)
        value = data.substr(key.size() + 1);
      result.insert(std::pair<std::string, std::string>(trimString(key, " \t"), trimString(value, " \t")));
    }
    return (result);
  }

  std::set<std::string>
  stringVectorToSet(std::vector<std::string> stringVector){
    std::set<std::string> ret;

    for (size_t i = 0; i < stringVector.size(); ++i) {
      ret.insert(stringVector[i]);
    }
    return (ret);
  }

} //namespace ft
