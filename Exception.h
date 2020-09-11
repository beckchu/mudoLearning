#pragma once
#include<exception>
#include<string>

namespace mudo{
class Exception:public std::exception
{
public:
	explicit Exception(const char* what);
	virtual ~Exception() throw();//no throw any exception
	virtual const char* what() const throw();
	const char* stackTrace() const throw();

private:
	std::string message_;
	std::string stack_;
};
}


