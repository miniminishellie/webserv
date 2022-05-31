/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HtmlWriter.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jihoolee <jihoolee@student.42SEOUL.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/30 14:52:53 by plee              #+#    #+#             */
/*   Updated: 2022/05/31 17:38:21 by jihoolee         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTML_WRITER_HPP
# define HTML_WRITER_HPP

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

#endif  //  HTML_WRITER_HPP_
