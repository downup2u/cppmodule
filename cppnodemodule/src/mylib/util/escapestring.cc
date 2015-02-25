#include "escapestring.h"
#include <string>
#include <sstream>
#include <iomanip>
#include <cassert>

#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/insert_linebreaks.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <boost/archive/iterators/ostream_iterator.hpp>
#include <sstream>
#include <string>

#include "../../EncryptWrap.h"

const std::string encryptkey = "iteasysofttask";
std::string encodeString(const std::string& source)
{
	std::string src = EncryptWrap::get_mutable_instance().encryptString(source, encryptkey);
	using namespace boost::archive::iterators;
	std::stringstream os;
	typedef
		insert_linebreaks<         // insert line breaks every 72 characters
		base64_from_binary<    // convert binary values to base64 characters
		transform_width<   // retrieve 6 bit integers from a sequence of 8 bit bytes
		const char *,
		6,
		8
		>
		>
		, 72
		>
		base64_text; // compose all the above operations in to a new iterator

	std::copy(
		base64_text(src.c_str()),
		base64_text(src.c_str() + src.size()),
		ostream_iterator<char>(os)
		);
	std::string dst = os.str();
	return dst;
}

std::string decodeString(const std::string& source)
{
	return "";
}
//====================================
std::string escapeSql(const std::string& source){
	//http://www.codecodex.com/wiki/Escape_sequences_and_escape_characters#SQL
	std::string dst = escapeString(source);
	std::string result;
	for (size_t i = 0; i < dst.length(); ++i) {
		if (dst[i] == '\'') {
			result += "''";
		}
		else
		{
			result.push_back(dst[i]);
		}
	}
	return result;
}

std::string escapeString(const std::string & s) {
  std::string result;
  for (size_t i = 0; i < s.length(); ++i) {
    switch (s[i]) {
      case '\\': 
        result += "\\\\";
        break;
      case '"':
        result += "\\\"";
        break;
      case '\a':
        result += "\\a";
        break;
      case '\f':
        result += "\\f";
        break;
      case '\n':
        result += "\\n";
        break;
      case '\r':
        result += "\\r";
        break;
      case '\t':
        result += "\\t";
        break;
      case '\v':
        result += "\\v";
        break;
      default:       
          result.push_back(s[i]);       
        break;
    }
  }
  return result;
}

std::string unescapeString(const std::string& s) {
  std::string result;
  for (size_t i = 0; i < s.length(); ++i) {
    // Body of this loop can change i
    if (s[i] == '\\') {
      assert (i + 1 < s.length());
      switch (s[i + 1]) {
        case 'a': result += '\a'; break;
        case 'e': result += '\033'; break;
        case 'f': result += '\f'; break;
        case 'n': result += '\n'; break;
        case 'r': result += '\r'; break;
        case 't': result += '\t'; break;
        case 'v': result += '\v'; break;        
        default: ++i; result += s[i]; break;
      }
    } else if (s[i] == '"') {
      return result;
    } else {
      result += s[i];
    }
  }
  return result;
}
