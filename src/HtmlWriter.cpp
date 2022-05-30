/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HtmlWriter.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plee <plee@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/30 14:52:56 by plee              #+#    #+#             */
/*   Updated: 2022/05/30 15:17:24 by plee             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HtmlWriter.hpp"

HtmlWriter::HtmlWriter() : m_body_("<html>\n<body>\n</body>\n</html>\n") {}

HtmlWriter::HtmlWriter(const HtmlWriter& ref) : m_body_(ref.get_m_body()) {}

HtmlWriter::~HtmlWriter() { this->m_body_.clear(); }

HtmlWriter& HtmlWriter::operator=(const HtmlWriter& operand) {
  if (this == &operand)
    return *this;
  this->m_body_ = operand.get_m_body();
  return *this;
}

std::string HtmlWriter::get_m_body() const { return (this->m_body_); }

void HtmlWriter::set_m_body(std::string body) { this->m_body_ = body; }

/*
** add title tag in header section
** @param: tilte content
** @return: void
*/
void HtmlWriter::Add_title(std::string title_content) {
  std::string token = "<title>" + title_content + "</title>";

  if (m_body_.find("<head>") == std::string::npos) {
  	m_body_.insert(m_body_.find("\n") + 1, "<head></head>\n");
  }
  m_body_.insert(m_body_.find("<head>") + 6, token);
}

/*
** add inline color css in body open tag
** @param: color of background
** @return: void
*/
void HtmlWriter::Add_bgcolor(std::string bg_color) {
  int idx = m_body_.find("<body");
  std::string token = " bgcolor=\"" + bg_color + "\"";

  if (m_body_[idx + 5] == '>') {
    idx += 5;
  } else {
    while (m_body_[idx] != '>')
      ++idx;
  }
  m_body_.insert(idx, token);
}

/*
** add pair tag
** @param1: position to insert
** @param2: tag name
** @param3: content between tag
** @return: void
*/
void HtmlWriter::Add_tag(std::string front_token, std::string tag, std::string content, bool newline) {
  std::string token = "<" + tag + ">";

  if (newline)
    token.append("\n");
  token.append(content);
  if (newline && !content.empty())
    token.append("\n");
  token.append("</" + tag + ">\n");
  m_body_.insert(m_body_.find(front_token) + front_token.size(), token);
}

/*
** insert text with '\n' to back of token(using rfind)
** @param: text to insert
** @return: void
*/
void HtmlWriter::Add_text(std::string front_token, std::string content, bool newline) {
  std::string token = content;

  if (newline)
  	token.append("\n");
  m_body_.insert(m_body_.rfind(front_token) + front_token.size(), token);
}

/*
** make a tag token with address and content
** @param1: relative/absolute path
** @param2: content in a tag
** @return: <a> token
*/
std::string
HtmlWriter::MakeLink(std::string address, std::string content)
{
  std::string token = "<a href=\"" + address + "\">";
  if (!content.empty())
    token.append(content);
  else
    token.append(address);
  token.append("</a>");
  return token;
}

/*
** insert text with '\n' by line number
** @param1: line index to insert
** @param2: line to insert
** @return: void
*/
void HtmlWriter::Add_line(int line_idx, std::string line) {
  int idx = 1;
  int pos = 0;
  std::string body = m_body_;

  while (idx < line_idx) {
    pos += body.find("\n") + 1;
    body = body.substr(body.find("\n") + 1);
    ++idx;
  } 
  m_body_.insert(pos, line + "\n");
}