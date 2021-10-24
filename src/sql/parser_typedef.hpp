#pragma once
#include <vector>

#ifndef YYtypeDEF_YY_SCANNER_T
#define YYtypeDEF_YY_SCANNER_T
typedef void* yyscan_t;
#endif


#define YYSTYPE ZSQL_STYPE
#define YYLTYPE ZSQL_LTYPE

struct ZSQL_CUST_LTYPE {
    int first_line;
    int first_column;
    int last_line;
    int last_column;

    int total_column;

    // Length of the string in the SQL query string
    int string_length;

    // Parameters.
    // int param_id;
    std::vector<void*> param_list;
};

#define ZSQL_LTYPE ZSQL_CUST_LTYPE
#define ZSQL_LTYPE_IS_DECLARED 1
