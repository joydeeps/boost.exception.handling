// Selectively accessing data with boost::get_error_info()
//
#include <boost/exception/all.hpp>
#include <exception>
#include <string>
#include <limits>
#include <algorithm>
#include <new>
#include <iostream>

typedef
boost::error_info<struct errmsg_tag, std::string> errmsg_info;

struct allocation_failed : public std::exception
{
	const char* what() const noexcept
	{
		return "allocation_failed";
	}
};

char* allocate_memory(std::size_t size)
{
	char* c = new(std::nothrow) char[size];
	if (!c)
		BOOST_THROW_EXCEPTION(allocation_failed());

	return c;
}

char* write_lots_of_zeros()
{
	try
	{	
		char* c = allocate_memory(std::numeric_limits<std::size_t>::max());
		std::fill_n(c, std::numeric_limits<std::size_t>::max(), 0);

		return c;
	}
	catch(boost::exception& e)
	{
		e << errmsg_info("writing lots of zeros failed");
		throw;
	}
}

int main()
{
	try
	{
		char* c = write_lots_of_zeros();
		delete[] c;
	}
	catch(boost::exception& e)
	{
		std::cerr << *boost::get_error_info<errmsg_info>(e);
	}
}

/*
 * The above example does not use boost::diagnostic_information(), it uses 
 * boost::get_error_info() to directly access the error message of type
 * errmsg_info.
 *
 * Because boost::get_error_info() returns a smart pointer of type boost::shared_ptr,
 * operator* is used to fetch the error message. If the parameter passed to
 * boost::get_error_info() is not of type boost::exception, a null pointer is
 * returned. 
 *
 * If the macro BOOST_THROW_EXCEPTION is always used to throw an exception,
 * the exception will always be derived from boost::exception - there is no 
 * need to check the returned smart pointer for null in that case.
 *
 */
