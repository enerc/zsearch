#pragma once
#include <string>

namespace queries {

typedef enum {
	HTTP_100=100,//	Continue
	HTTP_101=101,//	Switching Protocols
	HTTP_102=102,//	Processing	WebDAV RFC 25183,4
	HTTP_103=103,//	Early Hints	RFC 82975
	HTTP_200=200,//	OK
	HTTP_201=201,//	Created
	HTTP_202=202,//	Accepted
	HTTP_203=203,//	Non-Authoritative Information
	HTTP_204=204,//	No Content
	HTTP_205=205,//	Reset Content
	HTTP_206=206,//	Partial Content
	HTTP_207=207,//	Multi-Status
	HTTP_208=208,//	Already Reported
	HTTP_210=210,//	Content Different
	HTTP_226=226,//	IM Used	RFC 32296
	HTTP_300=300,//	Multiple Choices
	HTTP_301=301,//	Moved Permanently
	HTTP_302=302,//	Found
	HTTP_303=303,//	See Other
	HTTP_304=304,//	Not Modified
	HTTP_305=305,//	Use Proxy
	HTTP_306=306,//	Switch Proxy
	HTTP_307=307,//	Temporary Redirect
	HTTP_308=308,//	Permanent Redirect
	HTTP_310=310,//	Too many Redirects
	HTTP_400=400,//	Bad Request
	HTTP_401=401,//	Unauthorized
	HTTP_402=402,//	Payment Required
	HTTP_403=403,//	Forbidden
	HTTP_404=404,//	Not Found
	HTTP_405=405,//	Method Not Allowed
	HTTP_406=406,//	Not Acceptable
	HTTP_407=407,//	Proxy Authentication Required
	HTTP_408=408,//	Request Time-out
	HTTP_409=409,//	Conflict
	HTTP_410=410,//	Gone
	HTTP_411=411,//	Length Required
	HTTP_412=412,//	Precondition Failed
	HTTP_413=413,//	Request Entity Too Large
	HTTP_414=414,//	Request-URI Too Long
	HTTP_415=415,//	Unsupported Media Type
	HTTP_416=416,//	Requested range unsatisfiable
	HTTP_417=417,//	Expectation failed
	HTTP_418=418,//	I’m a teapot
	HTTP_421=421,//	Bad mapping / Misdirected Request
	HTTP_422=422,//	Unprocessable entity
	HTTP_423=423,//	Locked	WebDAV
	HTTP_424=424,//	Method failure
	HTTP_425=425,//	Unordered Collection
	HTTP_426=426,//	Upgrade Required
	HTTP_428=428,//	Precondition Required
	HTTP_429=429,//	Too Many Requests
	HTTP_431=431,//	Request Header Fields Too Large
	HTTP_449=449,//	Retry With	Code
	HTTP_450=450,//	Blocked by Windows Parental Controls
	HTTP_451=451,//	Unavailable For Legal Reasons
	HTTP_456=453,//	Unrecoverable Error
	HTTP_444=444,//	No Response
	HTTP_495=495,//	SSL Certificate Error
	HTTP_496=496,//	SSL Certificate Required
	HTTP_497=497,//	HTTP Request Sent to HTTPS Port
	HTTP_498=498,//	Token expired/invalid
	HTTP_499=499,//	Client Closed Request
	HTTP_500=500,//	Internal Server Error
	HTTP_501=501,//	Not Implemented
	HTTP_502=502,//	Bad Gateway
	HTTP_503=503,//	Service Unavailable
	HTTP_504=504,//	Gateway Time-out
	HTTP_505=505,//	HTTP Version not supported
	HTTP_506=506,//	Variant Also Negotiates	RFC 229518
	HTTP_507=507,//	Insufficient storage
	HTTP_508=508,//	Loop detected
	HTTP_509=509,//	Bandwidth Limit Exceeded
	HTTP_510=510,//	Not extended
	HTTP_511=511,//	Network authentication required	RFC 658514
	HTTP_520=520,//	Unknown Error
	HTTP_521=521,//	Web Server Is Down
	HTTP_522=522,//	Connection Timed Out
	HTTP_523=523,//	Origin Is Unreachable
	HTTP_524=524,//	A Timeout Occurred
	HTTP_525=525,//	SSL Handshake Failed
	HTTP_526=526,//	Invalid SSL Certificate
	HTTP_527=527,//	Railgun Error
} HTTP_CODES;

class QueryStatus {
public:
	QueryStatus() {
		status = HTTP_200;
		processedDocuments = 0;
		failedDocuments = 0;
		warnings = 0L;
		errors = 0;
	}
	void addFailedDocuments(int count) {
		__atomic_add_fetch(&failedDocuments,count,__ATOMIC_RELAXED);
	}
	void addProcessedDocuments(int count) {
		__atomic_add_fetch(&processedDocuments,count,__ATOMIC_RELAXED);
	}
	void addWarnings(int count) {
		__atomic_add_fetch(&warnings,count,__ATOMIC_RELAXED);
	}
	void addErrors(int count) {
		__atomic_add_fetch(&errors,count,__ATOMIC_RELAXED);
	}

	[[nodiscard]] std::string toString() const {
	    if (status == HTTP_200) {
	        return "";
	    } else {
	        return std::string("Error Code: HTTP_")+std::to_string(status)+" "+customMessage+" "+sql;
	    }
	}
	HTTP_CODES status;
	std::string customMessage;
	std::string sql;
	uint64_t	processedDocuments;
	uint64_t	failedDocuments;
	uint64_t	warnings;
	uint64_t	errors;
};

}
