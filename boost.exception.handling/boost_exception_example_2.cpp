// More data with BOOST_THROW_EXCEPTION
//
#include <boost/exception/all.hpp>
#include <exception>
#include <new>
#include <string>
#include <limits>
#include <algorithm>
#include <iostream>

typedef
boost::error_info<struct tag_errmsg, std::string> errmsg_info;

struct allocation_failed : public std::exception
{
	const char* what() const noexcept
	{
		return "allocation failed";
	}
};

char* allocate_memory(std::size_t size)
{
	char *c = new(std::nothrow) char[size];
	if (!c)
		BOOST_THROW_EXCEPTION(allocation_failed{});
	
	return c;
}

char* write_lots_of_zeros()
{
	try
	{
		char *c = allocate_memory(std::numeric_limits<std::size_t>::max());
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
		char *c = write_lots_of_zeros();
		delete[] c;
	}
	catch(boost::exception& e)
	{
		std::cerr << boost::diagnostic_information(e);
	}
}

/*
 * Using the macro 	BOOST_THROW_EXCEPTION instead of throw, data such as function 
 * name, file name, and line number are automatically added to the exception. But 
 * this only works if the compiler supports macros for the additional data. While 
 * macros such as __FILE__ and __LINE__ have been standardized since C++98, the 
 * macro __func__, which gets the name of the current function, only became standard
 * with C++11. Because many compilers provided such a macro before C++11. Because
 * many compilers provided such a macro before C++11, BOOST_THROW_EXCEPTION tries 
 * to identify the underlying compiler and use the corresponding macro if it exists.
 *
 * Compiled with Visual C++ 2013, Example 56.2 displays the following message:
 * main.cpp(20): Throw in function char *__cdecl allocate_memory(unsigned int)
 * Dynamic exception type: class boost::exception_detail::clone_impl<struct
 *   boost::exception_detail::error_info_injector<struct allocation_failed> >
 *   std::exception::what: allocation failed
 *   [struct tag_errmsg *] = writing lots of zeros failed
 *
 *
 * In the above example, allocation_failed is no longer derived from boost::exception.
 * BOOST_THROW_EXCEPTION accesses the function boost::enable_error_info(), which 
 * identifies whether or not an exception is derived from boost::exception. If not,
 * it creates a new exception type derived from the specified and boost::exception. 
 * This is why the message shown contains a different exception type than 
 * allocation_failed.
 *
 */
