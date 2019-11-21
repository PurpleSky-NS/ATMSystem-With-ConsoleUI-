#pragma once

#include <exception>

class FileOperationException :public std::runtime_error
{
public:
	FileOperationException() = default;
	FileOperationException(const std::string& msg) :
		runtime_error(msg) {}
};
