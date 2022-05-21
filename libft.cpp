#include "libft.hpp"

void ft_strjoin(std::string& str, const std::string& buf, size_t n) {
  str.append(buf.c_str(), n);
}

void ft_str_index_join(std::string& str, const std::string& buf, size_t i) {
  str.append(buf.c_str());
  str = str.substr(i);
}

std::vector<std::string> Split(std::string s, char c) {
  std::vector<std::string> result;
  size_t pos = 0;

  while ((pos = s.find(c)) != std::string::npos) {
    if (pos != 0)
      result.push_back(s.substr(0, pos));
    s.erase(0, pos + 1);
  }
  if (s.length() != 0)
    result.push_back(s);
  return result;
}

std::string LTrim(std::string s, std::string seps) {
  std::string result;
  int size = s.size();
  int idx = 0;

  if (size == 0)
    return (result);
  while (idx < size && seps.find(s[idx]) != std::string::npos)
    ++idx;
  if (idx == size)
    return result;
  return s.substr(idx);
}

std::string RTrim(std::string s, std::string seps) {
  std::string result;
  int size = s.size();
  int idx = size - 1;

  if (size == 0)
    return (result);
  while (idx >= 0 && seps.find(s[idx]) != std::string::npos)
    --idx;
  if (idx == -1)
    return (result);
  return s.substr(0, idx + 1);
}

std::string Trim(std::string s, std::string seps) {
  return RTrim(LTrim(s, seps), seps);
}

int ft_stoi(std::string str, size_t base) {
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
    return 0;
  while (!str.empty() && digit.find(str[0]) < base) {
    value *= base;
    value += digit.find(str[0]);
    if ((sign == 1 && value > int_max) || (sign == -1 && value > int_max + 1))
      throw (std::overflow_error("stoi overflow"));
    str.erase(str.begin());
  }
  return static_cast<int>(sign * value);
}

int getNewLine(std::string& data, std::string& line) {
  if (data.find("\n") == std::string::npos)
    return 0;
  int pos = data.find("\n");
  line = data.substr(0, pos);
  line = RTrim(line, "\r");
  data.erase(0, pos + 1);
    return 1;
  }