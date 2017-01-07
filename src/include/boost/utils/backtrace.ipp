#pragma once

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <stdexcept>

#include <execinfo.h>


namespace boost { namespace utils {
    
backtrace::backtrace()
        :
    locations_()
{
    nptrs_ = ::backtrace(buffer_, BT_BUF_SIZE);
    locations_ = ::backtrace_symbols(buffer_, nptrs_);
    if (locations_ == NULL)
    {
        auto error = strerror(errno);
        throw std::runtime_error(error);
    }
}

backtrace::~backtrace()
{
    free(locations_);
}

void backtrace::dump(std::ostream& os) const
{
    for (auto j = 0; j < nptrs_; j++)
        os << locations_[j] << std::endl;
}
    
} }

std::ostream& operator<<(std::ostream& os, const boost::utils::backtrace& bt)
{   
    bt.dump(os);
    return os;
}

