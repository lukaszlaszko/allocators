#pragma once

#include <iostream>
#include <string>
#include <vector>


namespace boost { namespace utils {    
 
/**
 * @brief Produces a backtrace of RAII
 * 
 * @details 
 * An instance of this object preserves backtrace of invocation on which the 
 * object has been created. An std::ostream insertion operator is overloaded to 
 * write back trace with iostream.
 */    
class backtrace
{
public:
    backtrace();
    ~backtrace();
    
    static void *operator new     (size_t) = delete;
    static void *operator new[]   (size_t) = delete;
    static void  operator delete  (void*)  = delete;
    static void  operator delete[](void*)  = delete;
    
    void dump(std::ostream& os) const;
    
private:
    static const std::size_t BT_BUF_SIZE = 100ul;
    
    void* buffer_[BT_BUF_SIZE];
    std::uint64_t nptrs_;
    char** locations_;
};
    
} }

std::ostream& operator<<(std::ostream& os, const boost::utils::backtrace& bt);

#include "backtrace.ipp"

