/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HtmlWriter.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jihoolee <jihoolee@student.42SEOUL.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/30 14:52:56 by plee              #+#    #+#             */
/*   Updated: 2022/05/31 17:46:22 by jihoolee         ###   ########.fr       */
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

void HtmlWriter::Add_title(std::string title_content) {
  std::string token = "<title>" + title_content + "</title>";

  if (m_body_.find("<head>") == std::string::npos) {
  	m_body_.insert(m_body_.find("\n") + 1, "<head></head>\n");
  }
  m_body_.insert(m_body_.find("<head>") + 6, token);
}

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

void HtmlWriter::Add_text(std::string front_token, std::string content, bool newline) {
  std::string token = content;

  if (newline)
  	token.append("\n");
  m_body_.insert(m_body_.rfind(front_token) + front_token.size(), token);
}

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
