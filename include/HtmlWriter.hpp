/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HtmlWriter.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plee <plee@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/30 14:52:53 by plee              #+#    #+#             */
/*   Updated: 2022/05/30 20:47:48 by plee             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTMLWRITER_HPP
# define HTMLWRITER_HPP

# include <iostream>
# include <string>

class HtmlWriter {
 public:
  /* HtmlWriter constructor & destructor */
  HtmlWriter(void);
  HtmlWriter(const HtmlWriter& ref);
  HtmlWriter& operator=(const HtmlWriter& operand);
  ~HtmlWriter(void);

  /* getter function */
  std::string get_m_body() const;
  /* setter function */
  void set_m_body(std::string body);

  /* member function */
  void Add_title(std::string head_content);
  void Add_bgcolor(std::string bg_color);
  void Add_tag(std::string front_token, std::string tag, std::string content = "", bool newline = false);
  void Add_text(std::string front_toekn, std::string content, bool newline = false);
  std::string MakeLink(std::string address, std::string content = "");
  void Add_line(int line_idx, std::string line);

 private:
  std::string m_body_;
};

#endif