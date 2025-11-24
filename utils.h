#pragma once
#include <iostream>
#include <mutex>
#include <sstream>

// Check for osyncstream support
#if __has_include(<syncstream>)
    #include <syncstream>
    #ifdef __cpp_lib_syncstream
        #define HAS_OSYNCSTREAM
    #endif
#endif

#ifndef HAS_OSYNCSTREAM
    // Inline mutex to prevent multiple definition errors when included in multiple files
    inline std::mutex cout_mutex;
    
    struct osyncstream {
        std::stringstream ss;
        std::ostream& target;
        osyncstream(std::ostream& os) : target(os) {}
        ~osyncstream() {
            std::lock_guard<std::mutex> lock(cout_mutex);
            target << ss.rdbuf();
            target.flush();
        }
        template<typename T>
        osyncstream& operator<<(const T& x) {
            ss << x;
            return *this;
        }
        // Support manipulators like std::endl
        osyncstream& operator<<(std::ostream& (*manip)(std::ostream&)) {
            manip(ss);
            return *this;
        }
    };
#else
    using std::osyncstream;
#endif
