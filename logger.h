// The MIT License (MIT)
// Copyright (C) 2016 John A. Tullos. All rights reserved.
// Website: http://wwww.xeekworx.com/
// Author E-mail: xeek@xeekworx.com
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
// documentation files(the "Software"), to deal in the Software without restriction, including without
// limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and / or sell copies of
// the Software, and to permit persons to whom the Software is furnished to do so, subject to the following
// conditions :
//
// The above copyright notice and this permission notice shall be included in all copies or substantial portions
// of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
// TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
// CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
#pragma once
#pragma warning(disable : 4275) // std::ios_base warning when exporting some things in STL, msdn says ignore.
#include <sstream> // std::wostringstream
#include <fstream> // std::wofstream

#define LOGSTAMP xeekworx::logger::create_stamp(__FILEW__, __FUNCTIONW__, __LINE__)

namespace xeekworx {

	enum  logtype { ERR = 0, EMPTY, NOTICE, DEBUG, INFO, WARNING };
	struct logstamp { std::wstring file, function; long line; };

	class logger {
	public:
		struct version_t { unsigned int major, minor, revision; };
		// Note: Before Visual Studio Update 2 this would cause an "illegal indirection" error.
		static constexpr version_t version = version_t { 1, 16, 54 };

		struct config {
			bool enable;
			bool enable_timestamp;
			bool enable_source_fullpath;
			bool enable_function;
			bool enable_function_full;
			bool enable_line;
			bool file_append;
			bool output_to_console;
			bool output_to_cerr;
			bool output_to_vs;
			bool output_to_file;
			std::wstring file;
		};

	private:
		static const wchar_t directory_separator;
		static const wchar_t path_separator;

		logger::config m_config;
		logstamp m_current_logstamp;
		logtype m_current_logtype;
		std::wostringstream log_stream;
		// At some point this may need to change to make this class exportable.
		// std::wofstream cannot be exported; though this isn't a problem when
		// this object is private, it still causes several warnings in
		// Visual Studio.
		std::wofstream log_file_stream;

	public:
		logger(void);
		logger(const logger::config& config);
		~logger();

		static logger::config default_config();
		void enable(const bool value = true);
		void enable(const logger::config& use_config);
		const bool& is_enabled() const { return m_config.enable; }

		template <typename T>
		logger& operator<<(const T &t) { if(is_enabled()) { log_stream << t; } return *this; }
		logger& operator<<(std::wostream&(*f)(std::wostream&));
		logger& operator<<(logtype type);
		logger& operator<<(logstamp stmp);

		static inline const logstamp& create_stamp(const wchar_t* file, const wchar_t* function, const long line)
		{
			static logstamp stamp = {};
			stamp.file = file;
			stamp.function = function;
			stamp.line = line;
			return stamp;
		}

		static std::wstring get_version();

	private:
		static std::wstring logtype_to_string(const logtype type);
		static bool logtype_is_empty(const logtype type);
		static std::wstring remove_function_owners(const std::wstring function_name);
		static std::wstring get_timestamp();
		static bool path_exists(const std::wstring& path);
		static std::wstring get_app_path(const std::wstring& modify_extension = std::wstring());
		static std::wstring path_filespec(const std::wstring& filepath);
		static std::string path_filespec(const std::string& filepath);
	};

	std::wstring to_unicode(const std::string& s);
	std::string to_multibyte(const std::wstring& s);

	extern xeekworx::logger log;
}
