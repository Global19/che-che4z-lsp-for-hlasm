#ifndef HLASMPLUGIN_PARSERLIBRARY_PROTOCOL_H
#define HLASMPLUGIN_PARSERLIBRARY_PROTOCOL_H

#include <cstdint>
#include <cstring>

#include "parser_library_export.h"

namespace hlasm_plugin::parser_library {

struct PARSER_LIBRARY_EXPORT string_array 
{
	string_array(const char** arr, size_t size);

	const char** arr;
	size_t size;
};

using version_t = uint64_t;
using position_t = uint64_t;

namespace context {
	struct completion_item_s;
}

namespace semantics {
	struct position_uri_s;
	struct completion_list_s;
}
struct PARSER_LIBRARY_EXPORT completion_item
{
	completion_item(context::completion_item_s& info);

	const char * label();
	size_t kind();
	const char * detail();
	const char* documentation();
	bool deprecated();
	const char* insert_text();
private:
	context::completion_item_s& impl_;
};

struct PARSER_LIBRARY_EXPORT completion_list
{
	completion_list(semantics::completion_list_s& info);
	bool is_incomplete();
	completion_item item(size_t index);
	size_t count();
private:
	semantics::completion_list_s & impl_;
};

struct PARSER_LIBRARY_EXPORT position
{
	position() : line(0), column(0) {}
	position(position_t line, position_t column) : line(line), column(column) {}
	bool operator==(const position & l) const
	{
		return line == l.line && column == l.column;
	}
	position_t line;
	position_t column;
};


struct PARSER_LIBRARY_EXPORT position_uri
{
	position_uri(semantics::position_uri_s &);
	position pos();
	const char * uri();
private:
	semantics::position_uri_s & impl_;
};

struct PARSER_LIBRARY_EXPORT position_uris
{
	position_uris(semantics::position_uri_s * data, size_t size);

	position_uri get_position_uri(size_t index);
	size_t size();

private:
	semantics::position_uri_s * data_;
	size_t size_;
};

struct PARSER_LIBRARY_EXPORT range
{
	range() {}
	range(position start, position end) : start(start), end(end) {}
	bool operator==(const range & r) const
	{
		return start == r.start && end == r.end;
	}
	position start;
	position end;
};

struct range_uri_s;

struct PARSER_LIBRARY_EXPORT range_uri
{
	range_uri(range_uri_s & range);
	range get_range();
	const char * uri();
private:
	range_uri_s & impl_;
};

struct PARSER_LIBRARY_EXPORT document_change
{
	document_change(const char * new_text, size_t text_length) : 
		whole(true), text(new_text), text_length(text_length) {}
	document_change(range change_range, const char * new_text, size_t text_length) :
		whole(false), change_range(change_range), text(new_text), text_length(text_length) {}

	bool operator==(const document_change & ch) const
	{
		return whole == ch.whole && change_range == ch.change_range &&
			text_length == ch.text_length && std::memcmp(text, ch.text, text_length) == 0;
	}

	const bool whole;
	const range change_range;
	const char * const text;
	const size_t text_length;
};

struct PARSER_LIBRARY_EXPORT text_document_item
{
	char * document_uri;
	version_t version;
	char * text;
};

enum class PARSER_LIBRARY_EXPORT diagnostic_severity
{
	error = 1,
	warning = 2,
	info = 3,
	hint = 4,
	unspecified = 5
};

class diagnostic_s;
class diagnostic_related_info_s;

struct PARSER_LIBRARY_EXPORT diagnostic_related_info
{
	diagnostic_related_info(diagnostic_related_info_s &);

	range_uri location() const;
	const char * message() const;
private:
	diagnostic_related_info_s & impl_;
};

struct PARSER_LIBRARY_EXPORT diagnostic
{
	diagnostic(diagnostic_s &);

	const char * file_name();
	range get_range();
	diagnostic_severity severity();
	const char * code();
	const char * source();
	const char * message();
	const diagnostic_related_info related_info(size_t index) const;
	size_t related_info_size();

private:
	diagnostic_s & impl_;

};


struct PARSER_LIBRARY_EXPORT diagnostic_list
{
	diagnostic_list();
	diagnostic_list(diagnostic_s * begin, size_t size);

	diagnostic diagnostics(size_t index);
	size_t diagnostics_size();

private:
	diagnostic_s * begin_;
	size_t size_;
};

namespace semantics {
	struct highlighting_info;
	struct symbol_range;
	enum class PARSER_LIBRARY_EXPORT hl_scopes { label, instruction, remark, ignored, comment, continuation, seq_symbol, var_symbol, operator_symbol, string, number, operand };
}

struct PARSER_LIBRARY_EXPORT token_info
{
	token_info(size_t line_start, size_t column_start, size_t line_end, size_t column_end, semantics::hl_scopes scope);
	token_info(const semantics::symbol_range & range, semantics::hl_scopes scope);
	range token_range;
	semantics::hl_scopes scope;
};

struct PARSER_LIBRARY_EXPORT file_highlighting_info
{
	file_highlighting_info(semantics::highlighting_info & info);

	const char * document_uri();
	version_t document_version();
	size_t continuation_count();
	position continuation(size_t index);
	token_info token(size_t index);
	size_t token_count();
	size_t continuation_column();
	size_t continue_column();

private:
	semantics::highlighting_info & info;
};

class processor_file;
using file_id = processor_file * ;

struct PARSER_LIBRARY_EXPORT all_highlighting_info
{
	all_highlighting_info(file_id * files, size_t files_count);

	file_id * files();
	size_t files_count();
	file_highlighting_info file_info(file_id);
private:
	file_id * files_;
	size_t files_count_;

};

}

#endif // !HLASMPLUGIN_PARSERLIBRARY_PROTOCOL_H