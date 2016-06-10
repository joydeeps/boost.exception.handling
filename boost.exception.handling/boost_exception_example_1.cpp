/*
 * The library Boost.Exception provides a new exception type boost::exception,
 * that lets you add data to an exception after it has been thrown. This type
 * is defined in /boost/exception.hpp. Because Boost.Exception spreads its 
 * classes and functions over multiple header files, the following examples 
 * access the master header file boost/exception/all.hpp to avoid including 
 * header files one by one.
 * 
 * Boost.Exception supports the mechanism from the C++11 standard that
 * transports an exception from one thread to another.
 * boost::exception_ptr is similar to std::exception_ptr.
 * 
 * However, Boost.Exception isn't full replacement for the header file
 * "exception" from the standard library. 
 * 
 * For example, Boost.Exception is missing support for nested exceptions of 
 * type std::nested_exception.
 * 
 */

// Using boost::exception

#include <boost/exception/all.hpp>
#include <exception>
#include <new>
#include <string>
#include <algorithm>
#include <limits>
#include <iostream>

typedef 
boost::error_info<struct tag_errmsg, std::string> errmsg_info;

struct allocation_failed : public boost::exception, public std::exception
{
  const char* what() const noexcept
  {
    return "allocation failed";
  }
};

char* allocate_memory(std::size_t size)
{
  char* c = new(std::nothrow) char[size];
  if (!c)
    throw allocation_failed();
  
  return c;
}

char* write_lot_of_zeros()
{
  try
  {
    char* c = allocate_memory(std::numeric_limits<std::size_t>::max());
    std::fill_n(c, std::numeric_limits<std::size_t>::max(), 0);
    return c;
  }
  catch(boost::exception &e)
  {
    e << errmsg_info("writing lots of zeros failed");
    throw;
  }
}

int main()
{
  try
  {
    char* c = write_lot_of_zeros();
    delete[] c;
  }
  catch(boost::exception& e)
  {
    std::cerr << boost::diagnostic_information(e);
  }
}