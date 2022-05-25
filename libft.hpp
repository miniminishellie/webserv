/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   libft.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plee <plee@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/23 16:26:48 by plee              #+#    #+#             */
/*   Updated: 2022/05/25 21:00:03 by plee             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBFT_HPP
#define LIBFT_HPP

#include <string>
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <vector>
#include <set>
#include <sys/stat.h>

// namespace ft
// {
  void ft_strjoin(std::string& str, const std::string& buf, size_t n);
  void ft_str_index_join(std::string& str, const std::string& buf, size_t i);
  std::vector<std::string> split(std::string s, char c);
  std::string ltrim(std::string s, std::string seps = " ");
  std::string rtrim(std::string s, std::string seps = " ");
  std::string trim(std::string s, std::string seps = " ");
  int ft_stoi(std::string str, size_t base = 10);
  int getLine(std::string& str, std::string &line, size_t buffer_size);
  int getNewLine(std::string& data, std::string& line);
  bool isFile(std::string path);
  bool isDirectory(std::string path);
  
//}

#endif