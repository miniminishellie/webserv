/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Libft.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bylee <bylee@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/21 15:14:43 by bylee             #+#    #+#             */
/*   Updated: 2022/05/21 22:12:49 by bylee            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../include/Libft.hpp"


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

std::vector<std::string> splitStringByChar(std::string& str, char c){
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

std::vector<std::string> groupLineWithCondition(std::vector<std::string>& lines, \
  const std::string& start_line, const std::string& end_line, IncludeMode mode){
  bool is_group_line = false;
  std::vector<std::string> result;
  std::vector<std::string> remain;

  for (size_t i = 0; i < lines.size(); ++i){
    std::string line = lines[i];
    if (line.empty())
      continue ;
    if (is_group_line == true){
      if (line.size() >= end_line.size() && line.substr(0, end_line.size()) == end_line){
        if (mode == INCLUDE_BOTH || mode == INCLUDE_END)
          result[result.size() - 1].append(line + "\n");
        is_group_line = false;
      }
      else
        result[result.size() - 1].append(line + "\n");
    }
    else if (line.size() >= start_line.size() && line.substr(0, start_line.size()) == start_line){
      is_group_line = true;
      if (mode == INCLUDE_BOTH || mode == INCLUDE_START)
        result.push_back(line + "\n");
      else
        result.push_back("");
    }
    else
      remain.push_back(line);
  }
  lines = remain;
  return (result);
}
