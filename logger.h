// The MIT License (MIT)
// Copyright (C) 2017 John A. Tullos. All rights reserved.
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
#include <sstream> // std::ostringstream
#include <fstream> // std::ofstream
#include <map>
#include <thread>
#include <mutex> // std::mutex & std::lock_guard for thread safety

#define LOGSTAMP xeekworx::logger::create_stamp(__FILE__, __FUNCTION__, __LINE__)

namespace xeekworx {

	enum  logtype { FATAL = -1, ERR = 0, EMPTY, NOTICE, DEBUG, DEBUG2, DEBUG3, INFO, WARNING };
	struct logstamp { std::string file, function; long line; std::thread::id thread_id; };

	class logger {
	public:
		std::mutex logging_mutex;
		struct version_t { unsigned int major, minor, revision; };
		// Note: Before Visual Studio Update 2 this would cause an "illegal indirection" error.
		static constexpr version_t version = version_t { 1, 17, 819 };

		struct config {
			bool enable;
			bool enable_timestamp;
			bool enable_thread_id;
			bool enable_source_fullpath;
			bool enable_function;
			bool enable_function_full;
			bool enable_line;
			bool enable_type;
			bool file_append;
			bool output_to_console;
			bool output_to_cerr;
			bool output_to_vs;
			bool output_to_file;
			bool colorize;
			std::string file;
		};

	private:
		struct log_state {
			log_state() : current_logtype(NOTICE) {}
			std::ostringstream stream;
			logstamp current_logstamp;
			logtype current_logtype;
		};

		static const char directory_separator;
		static const char path_separator;

		logger::config m_config;
		std::map<std::thread::id, log_state> log_states;
		bool m_msgonly;
		// At some point this may need to change to make this class exportable.
		// std::ofstream cannot be exported; though this isn't a problem when
		// this object is private, it still causes several warnings in
		// Visual Studio.
		std::ofstream log_file_stream;

	public:
		logger(void);
		logger(const logger::config& config);
		~logger();

		static logger::config default_config();
		void enable(const bool value = true);
		void enable(const logger::config& use_config);
		const bool& is_enabled() const { return m_config.enable; }
		void set_msgonly(const bool msgonly) { this->m_msgonly = msgonly; }

		template <typename T>
		logger& operator<<(const T &t) { 
			if(is_enabled()) { 
				std::lock_guard<std::mutex> lock(logging_mutex); // Added for thread safety
				log_state& state = log_states[std::this_thread::get_id()];
				state.stream << t;
			}
			return *this;
		}
		logger& operator<<(std::ostream&(*f)(std::ostream&));
		logger& operator<<(logtype type);
		logger& operator<<(logstamp stmp);

		static inline logstamp create_stamp(const char* file, const char* function, const long line)
		{
			logstamp stamp = {};
			stamp.file = file;
			stamp.function = function;
			stamp.line = line;
			stamp.thread_id = std::this_thread::get_id();
			return stamp;
		}

		static std::string get_version();

	private:
		static std::string logtype_to_string(const logtype type);
		static bool logtype_is_empty(const logtype type);
		static std::string remove_function_owners(const std::string function_name);
		static std::string get_timestamp();
		static bool path_exists(const std::string& path);
		static std::string get_app_path(const std::string& modify_extension = std::string());
		static std::string path_filespec(const std::string& filepath);
	};

	std::wstring to_unicode(const std::string& s);
	std::string to_multibyte(const std::wstring& s);

#ifndef DISABLE_LOG_OBJECT
	extern xeekworx::logger log;
#endif
}
