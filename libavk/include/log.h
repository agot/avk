#ifndef AVK_LOG_H_
#define AVK_LOG_H_

#include <string>
#include <cstdio>

namespace avk {

class Log {
public:
    template<typename... Args>
    static inline void debug(const std::string& fmt, Args... args) {
#ifndef NDEBUG
        std::printf((fmt + '\n').c_str(), args...);
#endif // NDEBUG
    }

private:
};

} // namespace avk

#endif // #ifndef AVK_LOG_H_
