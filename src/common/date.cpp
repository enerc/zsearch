#include "date.hpp"
#include <iomanip>
#include <ctime>
#include <chrono>
#include <sstream>
#include "log.hpp"

using namespace std;
using namespace std::chrono;

[[maybe_unused]] inline std::string to_iso_8601(std::chrono::time_point<std::chrono::system_clock> t, bool isNano) {

    // convert to time_t which will represent the number of
    // seconds since the UNIX epoch, UTC 00:00:00 Thursday, 1st. January 1970
    auto epoch_seconds = std::chrono::system_clock::to_time_t(t);

    // Format this as date time to seconds resolution
    // e.g. 2016-08-30T08:18:51
    std::stringstream stream;
    stream << std::put_time(gmtime(&epoch_seconds), "%FT%T");

    // If we now convert back to a time_point we will get the time truncated
    // to whole seconds
    auto truncated = std::chrono::system_clock::from_time_t(epoch_seconds);

    if (isNano) {
        // Now we subtract this seconds count from the original time to
        // get the number of extra microseconds..
        auto delta_us = std::chrono::duration_cast<std::chrono::nanoseconds>(t - truncated).count();

        // And append this to the output stream as fractional seconds
        // e.g. 2016-08-30T08:18:51.867479
        stream << "." << std::fixed << std::setw(9) << std::setfill('0') << delta_us;
    }

    return stream.str();
}

[[maybe_unused]] inline uint64_t parse8601(const std::string &s) {
    std::string save = s;
    std::istringstream in(s);
    date::sys_time<std::chrono::milliseconds> tp;
    in >> date::parse("%FT%TZ", tp);
    if (in.fail()) {
        in.clear();
        in.str(save);
        in >> date::parse("%FT%T%Ez", tp);
        if (in.fail()) {
            in.clear();
            for (auto &g : save) {
                if (g < '0' || g > '9') {
                    Log::error("Invalid Date : "+s);
                    return 0;
                }
            }
            return atol(s.c_str());
        }
    }
    return std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch()).count();
}

inline uint32_t dateFTToSeconds(const std::string_view &s) {
    if (s.size() < 19) {
        return 0; // error
    }
    // Date format %F %T : YYYY-MM-DD %H:%M:%S
    const char *p = s.data();
    int year = (p[0]-'0')*1000+(p[1]-'0')*100+(p[2]-'0')*10+(p[3]-'0');
    unsigned int month = (p[5]-'0')*10 + (p[6]-'0');
    unsigned int day = (p[8]-'0')*10 + (p[9]-'0');
    unsigned int hour = (p[11]-'0')*10 + (p[12]-'0');
    unsigned int minute = (p[14]-'0')*10 + (p[15]-'0');
    unsigned int second = (p[17]-'0')*10 + (p[18]-'0');
    auto ymd = date::year_month_day{date::year{year},date::month{month},date::day{day}};
    auto tp = date::sys_days{ymd};
    auto dp = tp.time_since_epoch();
    return dp.count()*24*3600 + hour * 3600 + minute *60 + second;
}

uint64_t stringToDate(bool isFt,bool isNano,const std::string &format, const std::string_view &v,bool &fail) {
    fail = false;
    if (isFt) {
        return dateFTToSeconds(v);
    } else if (!isNano){
        time_point<system_clock, seconds> tp;
        istringstream ss(string(v.data(), v.size()));
        ss >> date::parse(format, tp);
        fail = ss.fail();
        return tp.time_since_epoch().count();
    } else {
        time_point<system_clock,nanoseconds> tp;
        istringstream ss(string(v.data(),v.size()));
        ss >> date::parse(format, tp);
        if (ss.fail()) {
        }
        return tp.time_since_epoch().count();
    }
}
