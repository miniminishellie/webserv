/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Libft.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jihoolee <jihoolee@student.42SEOUL.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/21 15:14:43 by bylee             #+#    #+#             */
/*   Updated: 2022/05/31 17:11:23 by jihoolee         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "Libft.hpp"

namespace ft {
int stoi(std::string str, size_t base) {
  int sign = 1;
  __int64_t value = 0;
  __int64_t int_max = INT_MAX;
  // long long value = 0;
  // long long int_max = INT_MAX;
  std::string digit = "0123456789abcdefghijklmnopqrstuvwxyz";

  if (str.empty())
    return (0);
  if (str[0] == '-')
    sign = -1;
  if (str[0] == '-' || str[0] == '+')
    str.erase(str.begin());
  if (str.empty() || digit.find(str[0]) == std::string::npos)
    return (0);
  while (!str.empty() && digit.find(str[0]) < base) {
    value *= base;
    value += digit.find(str[0]);
    if ((sign == 1 && value > int_max) || (sign == -1 && value > int_max + 1))
      throw std::overflow_error("stoi overflow");
    str.erase(str.begin());
  }
  return (static_cast<int>(sign * value));
}

void bzero(void *data, size_t len) {
  unsigned char *str;

  str = (unsigned char *)data;
  while (len > 0)
    str[--len] = 0;
}

void *memcpy(void *dest, const void *src, size_t len) {
  char *d = reinterpret_cast<char *>(dest);
  const char *s = reinterpret_cast<const char *>(src);

  while (len--)
    *d++ = *s++;
  return (dest);
}

void strjoin(std::string& str, const std::string& buf, size_t n) {
  str.append(buf.c_str(), n);
}

void str_index_join(std::string& str, const std::string& buf, size_t i) {
  str.append(buf.c_str());
  str = str.substr(i);
}

char * strdup(const char *s) {
  size_t len = strlen(s) + 1;
  void *str = malloc(len);

  if (str == NULL)
    return NULL;
  return (reinterpret_cast<char *>(ft::memcpy(str, s, len)));
}

int getLine(std::string& str, std::string &line, size_t buffer_size) {
  if (str.find("\n") == std::string::npos || str.find("\n") > buffer_size) {
    if (str.size() >= buffer_size)
      throw (std::overflow_error("line size is greather than buffer size"));
    else
      return -1;
  }
  int pos = str.find("\n");
  line = str.substr(0, pos);
  line = rtrimString(line, "\r");
  str.erase(0, pos + 1);
  return line.size();
}

int getNewLine(std::string& data, std::string& line) {
  if (data.find("\n") == std::string::npos)
    return 0;
  int pos = data.find("\n");
  line = data.substr(0, pos);
  line = rtrimString(line, "\r");
  data.erase(0, pos + 1);
    return 1;
}

bool isFile(std::string path) {
  struct stat buf;
  stat(path.c_str(), &buf);
  return S_ISREG(buf.st_mode);
}

bool isDirectory(std::string path) {
  struct stat buf;
  stat(path.c_str(), &buf);
  return S_ISDIR(buf.st_mode);
}

std::string ltrimString(const std::string& str, const std::string& seps) {
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

std::string rtrimString(const std::string& str, const std::string& seps) {
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

std::string getStringFromFile(const std::string& file_path, int max_size) {
  int fd = -1;
  size_t read_byte = 0;
  char buf[1024];
  std::string result;

  if ((fd = open(file_path.c_str(), O_RDONLY)) == -1)
    throw(std::invalid_argument("Failed to open " + file_path));
  while ((read_byte = read(fd, buf, 1024)) > 0) {
    result.append(buf, read_byte);
    if (max_size != -1 && static_cast<int>(result.size()) > max_size)
      throw std::overflow_error("overflow max_size in getStringFromFile");
  }
  close(fd);
  return (result);
}

std::string getStringFromFd(int fd, int max_size) {
  int read_cnt = 0;
  char buff[1024];
  std::string ret;

  while ((read_cnt = read(fd, buff, 1024)) > 0) {
    ret.append(buff, read_cnt);
    if (max_size != -1 && static_cast<int>(ret.size()) > max_size)
      throw (std::overflow_error("overflow max_size in getStringFromFile"));
  }
  close(fd);
  return (ret);
}

std::vector<std::string> splitStringByChar(std::string str, char c) {
  std::vector<std::string> result;
  size_t pos = 0;

  while ((pos = str.find(c)) != std::string::npos) {
    if (pos != 0)
      result.push_back(str.substr(0, pos));
    str.erase(0, pos + 1);
  }
  if (str.length() != 0)
    result.push_back(str);
  return (result);
}

std::map<std::string, std::string> stringVectorToMap(
      std::vector<std::string> stringVector,
      char sep) {
  std::map<std::string, std::string> result;

  if (stringVector.size() == 0)
    return result;
  for (size_t i = 0; i < stringVector.size(); ++i) {
    if (stringVector[i].find(sep) == std::string::npos)
      throw std::invalid_argument("Not found sep in string");
    if (stringVector[i].find(sep) == 0)
      throw std::invalid_argument("Not found key in string");
    std::string data = trimString(stringVector[i], " \t");
    std::string key = data.substr(0, data.find(sep));
    std::string value;
    if (stringVector[i].find(sep) < stringVector[i].size() - 1)
      value = data.substr(key.size() + 1);
    result.insert(std::pair<std::string, std::string>(
                    trimString(key, " \t"),
                    trimString(value, " \t")));
  }
  return (result);
}

std::set<std::string>
stringVectorToSet(std::vector<std::string> stringVector) {
  std::set<std::string> ret;

  for (size_t i = 0; i < stringVector.size(); ++i) {
    ret.insert(stringVector[i]);
  }
  return (ret);
}

void log(int log_fd, std::string text) {
  if (log_fd != -1)
    write(log_fd, text.c_str(), text.size());
}

long long int abs(long long int num) {
  if (num < 0)
    return num * -1;
  return (num);
}

std::string to_string(long long int n) {
  long long int   nb;
  std::string		str;

  if (n == 0)
    return ("0");
  nb = n;
  while (nb != 0) {
    str.insert(str.begin(), static_cast<char>((ft::abs(nb % 10) + 48)));
    nb = nb / 10;
  }
  if (n < 0)
    str.insert(str.begin(), '-');
  return (str);
}

template<typename T>
void AddDevideResult(int& ret, T& data, int number) {
  ret += data / number;
  data %= number;
}
void MakeTime(struct tm* t) {
  t->tm_mday++;
  t->tm_year -= 1900;
}

void convertTimespecToTm(time_t s, struct tm* t) {
  ft::bzero(t, sizeof(struct tm));
  t->tm_gmtoff = 0;
  t->tm_isdst = 0;
  t->tm_zone = NULL;
  t->tm_year = 1970;
  t->tm_mon = 0;
  long data = s + 32400;
  if (data > 946684800) {
   	t->tm_year = 2000;
   	data -= 946684800;
  }
  ft::AddDevideResult(t->tm_yday, data, 86400);
  ft::AddDevideResult(t->tm_hour, data, 3600);
  ft::AddDevideResult(t->tm_min, data, 60);
  t->tm_sec = data;

  while (t->tm_yday > 365) {
    if (t->tm_year % 4 == 0 && (t->tm_year % 100 != 0 || t->tm_year % 400 == 0)) {
  	  if (t->tm_yday == 366)
        break ;
      t->tm_yday--;
    }
    t->tm_yday -= 365;
    t->tm_year++;
  }
  int months[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  bool leap = t->tm_year % 4 == 0 && (t->tm_year % 100 != 0 || t->tm_year % 400 == 0);
  t->tm_mday = t->tm_yday;
  while (t->tm_mday > months[t->tm_mon])
  {
    if (leap && t->tm_mon == 2) {
      if (t->tm_mday == 28)
        break ;
      t->tm_mday--;
    }
    t->tm_mday -= months[t->tm_mon];
    t->tm_mon++;
  }
  t->tm_wday = t->tm_mday % 7;
  MakeTime(t);
  return ;
}

std::string itos(std::string number, size_t from, size_t to) {
  std::string base = "0123456789abcdefghijklmnopqrstuvwxyz";
  std::string ret = "";
  bool sign = false;
  size_t data = ft::stoi(number, from);

  if (number.empty() || data == 0)
    return "0";
  if (data < 0) {
    data *= -1;
    sign = true;
  }
  while (data > 0) {
    ret.insert(ret.begin(), base[data % to]);
    data /= to;
  }
  return (ret);
}

std::string	getTimestamp(void) {
  std::time_t	t = std::time(0);
  std::tm* now = std::localtime(&t);
  std::string ret;
  std::string data;
  ret.append("[" + ft::to_string(now->tm_year + 1900) + "_");

  data = ft::to_string(now->tm_mon + 1);
  if (data.size() == 1)
    ret.append("0" + data);
  else
    ret.append(data);

  data = ft::to_string(now->tm_mday);
  if (data.size() == 1)
    ret.append("0" + data + "_");
  else
    ret.append(data + "_");

  data = ft::to_string(now->tm_hour);
  if (data.size() == 1)
    ret.append("0" + data);
  else
    ret.append(data);

  data = ft::to_string(now->tm_min);
  if (data.size() == 1)
    ret.append("0" + data + "_");
  else
    ret.append(data + "_");

  data = ft::to_string(now->tm_sec);
  if (data.size() == 1)
    ret.append("0" + data + "]");
  else
    ret.append(data + "]");
  return (ret);
}

char *strsjoin(std::string s1, std::string s2, std::string s3, std::string s4, std::string s5) {
  s1.append(s2);
  s1.append(s3);
  s1.append(s4);
  s1.append(s5);
  return (strdup(s1.c_str()));
}

int lenDoubleStr(char **str) {
  int		idx;

  idx = 0;
  if (!str || !(*str))
    return (0);
  while (*str++)
    idx++;
  return (idx);
}

int free(void *ptr) {
  if (ptr)
    std::free(ptr);
  return (1);
}

int freeStr(char **str) {
  if (!str || !(*str))
    return (0);
  ft::free(*str);
  *str = 0;
  return (1);
}

int freeDoublestr(char ***doublestr_addr) {
  int i;
  char  **doublestr;

  if (!doublestr_addr || !(*doublestr_addr))
    return (0);
  i = -1;
  doublestr = *doublestr_addr;
  while (doublestr[++i])
    ft::freeStr(&doublestr[i]);
  ft::free(doublestr);
  *doublestr_addr = 0;
  return (1);
}

std::string to_string(long long int n) {
  long long int   nb;
  std::string		str;

  if (n == 0)
    return ("0");
  nb = n;
  while (nb != 0) {
    str.insert(str.begin(), static_cast<char>((ft::abs(nb % 10) + 48)));
    nb = nb / 10;
  }
  if (n < 0)
    str.insert(str.begin(), '-');
  return (str);
}

std::string inet_ntoa(unsigned int address) {
  std::string ret;

  ret = ft::to_string(address & 0xFF) + ".";
  ret.append(ft::to_string ((address >> 8) & 0xFF) + ".");
  ret.append(ft::to_string ((address >> 16) & 0xFF) + ".");
  ret.append(ft::to_string ((address >> 24) & 0xFF) + ".");
  return ret;
}
}  //  namespace ft
