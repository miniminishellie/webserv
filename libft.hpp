#ifndef LIBFT_HPP
#define LIBFT_HPP

#include <string>
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <vector>
#include <set>

// namespace ft
// {
  void ft_strjoin(std::string& str, const std::string& buf, size_t n);
  void ft_str_index_join(std::string& str, const std::string& buf, size_t i);
  std::vector<std::string> Split(std::string s, char c);
  std::string LRrim(std::string s, std::string seps = " ");
  std::string RTrim(std::string s, std::string seps = " ");
  std::string Trim(std::string s, std::string seps = " ");
  int ft_stoi(std::string str, size_t base = 10);
  int getNewLine(std::string& data, std::string& line);
//}

#endif