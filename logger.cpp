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

#include "logger.h"
#include <ctime>
#include <time.h>
#include <algorithm> // transform
#include <iostream>
#include <iomanip>
#include <codecvt> // wstring_convert / codecvt_utf8_utf16
#ifdef _WIN32
# include <Windows.h>
#endif

using namespace xeekworx;

namespace xeekworx {
	// Global logger:
	xeekworx::logger log;

	// Utility variables:
#ifdef _WIN32
	constexpr size_t XEEKWORX_MAX_PATH = MAX_PATH;
	const wchar_t xeekworx::logger::directory_separator = L'\\';
	const wchar_t xeekworx::logger::path_separator = L';';
#elif __APPLE__
	constexpr size_t XEEKWORX_MAX_PATH = MAXPATHLEN;
	const wchar_t xeekworx::logger::directory_separator = L'/';
	const wchar_t xeekworx::logger::path_separator = L';';
#else
	constexpr size_t XEEKWORX_MAX_PATH = PATHMAX;
	const wchar_t xeekworx::logger::directory_separator = L'/';
	const wchar_t xeekworx::logger::path_separator = L';';
#endif
}

logger::logger(void) : m_config(logger::default_config()), m_current_logtype(NOTICE)
{

}

logger::logger(const logger::config& config) : m_config(config)
{
	enable(config);
}

logger::~logger(void)
{
	if(log_file_stream.is_open()) log_file_stream.close();
}

logger::config logger::default_config()
{
	logger::config config;
	config.enable = false;
	config.enable_timestamp = true;
	config.enable_source_fullpath = false;
	config.enable_function = true;
	config.enable_function_full = false;
	config.enable_line = true;
	config.file_append = false;
	config.output_to_console = true;
	config.output_to_cerr = false;
	config.output_to_vs = false;
	config.output_to_file = true;
	return config;
}

void logger::enable(const logger::config& use_config)
{
	this->m_config = use_config;
	this->enable(use_config.enable);
}

void logger::enable(const bool value)
{
	m_config.enable = value;

	if(m_config.enable) {
		// Determine the log file path if given; otherwise put it in the application's folder:
		std::wstring file = (!m_config.file.empty() ? m_config.file : logger::get_app_path(L".log"));
		// Open the log file:
		log_file_stream.open(file, m_config.file_append ? std::ios_base::app : std::ios_base::trunc);
		if(log_file_stream.is_open() && m_config.file_append && logger::path_exists(file)) {
			log_file_stream << std::endl;
		}
	}
	else {
		if(log_file_stream.is_open()) log_file_stream.close();
	}
}

logger& logger::operator<<(std::wostream&(*f)(std::wostream&))
{
	if(is_enabled()) {
		typedef std::basic_ostream<wchar_t>& (*ENDL_T) (std::basic_ostream<wchar_t>&);
		const ENDL_T l_ENDL(&std::endl);

		if(f == (ENDL_T) std::endl) {
			std::wostringstream out;

			// TIME STAMP:
			if(m_config.enable_timestamp) out << logger::get_timestamp() << L" ";

			// SOURCE FILE:
			if(m_config.enable_source_fullpath && !m_current_logstamp.file.empty()) out << m_current_logstamp.file << L":";
			else if(!m_current_logstamp.file.empty()) out << std::setw(18) << std::right << logger::path_filespec(m_current_logstamp.file) << L":";

			// LINE NUMBER:
			if(m_config.enable_line && m_current_logstamp.line >= 0) out << std::setw(4) << std::right << std::setfill(L'0') << m_current_logstamp.line << L":";
			out << std::setfill(L' ');

			// FUNCTION NAME:
			if(m_config.enable_function && !m_current_logstamp.function.empty()) {
				if(m_config.enable_function_full) out << m_current_logstamp.function << L":";
				else out << std::setw(18) << std::left << remove_function_owners(m_current_logstamp.function);
			}

			// LOG TYPE:
			out << std::setw(7) << std::left << logtype_to_string(m_current_logtype) << L" ";

			// THE LAST OF THE ERROR MESSAGE:
			out << log_stream.str() << std::endl;

			// COLORIZE CONSOLE OUTPUT:
			// Not available in this version.

			// OUTPUT:
			if(m_config.output_to_console) std::wcout << out.str();
			if(m_config.output_to_cerr) std::wcerr << out.str();
			if(m_config.output_to_file && log_file_stream.is_open()) log_file_stream << out.str();
#ifdef _WIN32
			if(m_config.output_to_vs) {
				::OutputDebugStringW(out.str().c_str());
			}
#endif

			// RESET:
			log_stream.str(L"");
			m_current_logtype = NOTICE;
		}
	}

	return *this;
}

logger& logger::operator<<(logtype type)
{
	if(is_enabled()) m_current_logtype = type;
	return *this;
}

logger& logger::operator<<(logstamp stamp)
{
	if(is_enabled()) m_current_logstamp = stamp;
	return *this;
}

std::wstring logger::get_version()
{
	std::wstringstream ss;
	ss << logger::version.major << L"." << logger::version.minor << L"." << logger::version.revision;
	return ss.str();
}

std::wstring logger::logtype_to_string(const logtype type)
{
	switch(type) {
	case ERR: return L"ERROR";
	case NOTICE: return L"NOTICE";
	case DEBUG: return L"DEBUG";
	case WARNING: return L"WARNING";
	case INFO: return L"INFO";
	default: return L"";
	}
}

bool logger::logtype_is_empty(const logtype type)
{
	switch(type) {
	case ERR:
	case NOTICE:
	case DEBUG:
	case INFO:
	case WARNING:
		return false;
	case EMPTY:
	default:
		return true;
	}
}

std::wstring logger::remove_function_owners(const std::wstring function_name)
{
	std::wstring::size_type pos = function_name.find_last_of(L':');
	if(pos == std::wstring::npos) return function_name;
	else return function_name.substr(pos + 1);
}

std::wstring logger::get_timestamp()
{
	wchar_t timestamp_buffer[_MAX_PATH] = {};
	std::time_t now = std::time(NULL);
	std::tm current_localtime = {};
	std::tm* current_localtime_ptr = nullptr;
#ifdef _WIN32
	localtime_s(&current_localtime, &now);
	current_localtime_ptr = &current_localtime;
#else
	current_localtime_ptr = std::localtime(&now);
#endif
	std::wcsftime(timestamp_buffer, 32, L"%Y%m%d %H:%M:%S", current_localtime_ptr);

	return std::wstring(timestamp_buffer);
}

bool logger::path_exists(const std::wstring& filepath)
{
	struct stat info = {};
	return stat(to_multibyte(filepath).c_str(), &info) == 0 ? true : false;
}

std::wstring logger::get_app_path(const std::wstring& modify_extension)
{
	wchar_t module_file[XEEKWORX_MAX_PATH] = {};

#ifdef __APPLE__
	char buffer[XENO_MAX_PATH];
	uint32_t size = sizeof(buffer);
	_NSGetExecutablePath(buffer, &size);
	wcsncpy(module_file, to_unicode(buffer).c_str(), wcslen(buffer));
#elif _WIN32
	::GetModuleFileNameW(0, module_file, XEEKWORX_MAX_PATH);
#else
	char buffer[XENO_MAX_PATH] = {};
	size_t count = readlink("/proc/self/exe", buffer, XENO_MAX_PATH);
	wcsncpy(module_file, to_unicode(buffer).c_str(), count);
#endif

	std::wstring result(module_file);
	if(modify_extension.length() > 0) {
		size_t folder_path_end = result.find_last_of(logger::directory_separator);
		size_t ext_start = result.find_last_of(L".");
		bool has_period = modify_extension.compare(0, 1, L".") == 0;
		if(ext_start != std::wstring::npos) result = result.substr(0, ext_start + (has_period ? 0 : 1)) + modify_extension;
		else result += L"." + (has_period ? modify_extension.substr(1) : modify_extension);
	}
	return result;
}

std::wstring logger::path_filespec(const std::wstring& filepath)
{
	size_t folder_path_end = filepath.find_last_of(logger::directory_separator);
	if(folder_path_end == std::wstring::npos) return filepath;
	else { return filepath.substr(folder_path_end + 1); }
}

std::string logger::path_filespec(const std::string& filepath)
{
	return xeekworx::to_multibyte(path_filespec(xeekworx::to_unicode(filepath)));
}

std::wstring xeekworx::to_unicode(const std::string& s)
{
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t> > c;
	return c.from_bytes(s);
}

std::string xeekworx::to_multibyte(const std::wstring& s)
{
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t> > c;
	return c.to_bytes(s);
}
