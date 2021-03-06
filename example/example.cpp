// example.cpp : Defines the entry point for the console application.
//
#include <logger.h>
#include <chrono>
#include <thread>

void call_from_thread();

int main()
{
	// Use default log:
	xeekworx::log.enable();
	xeekworx::log << LOGSTAMP << xeekworx::logtype::INFO << "Logger Version: " << xeekworx::logger::get_version() << std::endl;
	xeekworx::log << LOGSTAMP << xeekworx::logtype::NOTICE << "Test notice logtype" << std::endl;
	xeekworx::log << LOGSTAMP << xeekworx::logtype::DEBUG << "Test debug logtype" << std::endl;
	xeekworx::log << LOGSTAMP << xeekworx::logtype::DEBUG2 << "Test debug2 logtype" << std::endl;
	xeekworx::log << LOGSTAMP << xeekworx::logtype::DEBUG3 << "Test debug3 logtype" << std::endl;
	xeekworx::log << LOGSTAMP << xeekworx::logtype::WARNING << "Test warning logtype" << std::endl;
	xeekworx::log << LOGSTAMP << xeekworx::logtype::FATAL << "Test fatal logtype" << std::endl;
	xeekworx::log << LOGSTAMP << xeekworx::logtype::ERR << "Test error logtype" << std::endl;
	xeekworx::log << LOGSTAMP << xeekworx::logtype::EMPTY << "Test empty logtype" << std::endl;

	// Log from main thread:
	xeekworx::log << LOGSTAMP << xeekworx::logtype::NOTICE << "Called from main thread " << std::this_thread::get_id() << std::endl;

	// Let a second go by so the time stamp changes, just for testing:
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	// Test the thread safety of the logger:
	std::thread t1(call_from_thread);
	std::this_thread::sleep_for(std::chrono::milliseconds(20));
	std::thread t2(call_from_thread);
	std::this_thread::sleep_for(std::chrono::milliseconds(40));
	std::thread t3(call_from_thread);
	std::this_thread::sleep_for(std::chrono::milliseconds(60));
	std::thread t4(call_from_thread);

	t4.join();
	t3.join();
	t2.join();
	t1.join();

	// Use custom log:
	xeekworx::logger::config config = xeekworx::logger::default_config();
	config.enable = true;
	config.file = "mylog.log";
	xeekworx::logger mylog(config);
	mylog << LOGSTAMP << xeekworx::logtype::INFO << "Logger Version: " << xeekworx::logger::get_version() << std::endl;
	mylog << LOGSTAMP << xeekworx::logtype::NOTICE << "Test notice logtype" << std::endl;
	mylog << LOGSTAMP << xeekworx::logtype::DEBUG << "Test debug logtype" << std::endl;
	mylog << LOGSTAMP << xeekworx::logtype::DEBUG2 << "Test debug2 logtype" << std::endl;
	mylog << LOGSTAMP << xeekworx::logtype::DEBUG3 << "Test debug3 logtype" << std::endl;
	mylog << LOGSTAMP << xeekworx::logtype::WARNING << "Test warning logtype" << std::endl;
	mylog << LOGSTAMP << xeekworx::logtype::FATAL << "Test fatal logtype" << std::endl;
	mylog << LOGSTAMP << xeekworx::logtype::ERR << "Test error logtype" << std::endl;
	mylog << LOGSTAMP << xeekworx::logtype::EMPTY << "Test empty logtype" << std::endl;

	return 0;
}

void call_from_thread() {
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	xeekworx::log << LOGSTAMP << xeekworx::logtype::NOTICE << "Called from other thread " << std::this_thread::get_id() << std::endl;
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	xeekworx::log << LOGSTAMP << xeekworx::logtype::DEBUG << "Called from other thread " << std::this_thread::get_id() << std::endl;
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	xeekworx::log << LOGSTAMP << xeekworx::logtype::DEBUG2 << "Called from other thread " << std::this_thread::get_id() << std::endl;
}