#pragma once

#include <cstdlib>
#include <memory>
#include <vector>
#include "columnType.hpp"

extern __int128_t parseSigned128(const char *src);

namespace zsql {
    struct SelectStatement;

// Helper function used by the lexer.
// TODO: move to more appropriate place.
    char* substr(const char* source, int from, int to);

    enum ExprType {
        kExprLiteralFloat=0,
        kExprLiteralString,
        kExprLiteralInt,
        kExprLiteralNull,
        kExprStar,
        kExprParameter,
        kExprColumnRef,
        kExprFunctionRef,
        kExprOperator,
        kExprSelect,
        kExprHint,
        kExprArray,
        kExprArrayIndex,
        kExprExtract,
        kExprCast,
        kExprSrid,
        kExprDistanceSphere
    };
    extern const char *ExprTypeName[];

// Operator types. These are important for expressions of type kExprOperator.
enum OperatorType {
    kOpNone,

    // Ternary operator
    kOpBetween,

    // n-nary special case
    kOpCase,
    kOpCaseListElement,  // `WHEN expr THEN expr`

    // Binary operators.
    kOpPlus,
    kOpMinus,
    kOpAsterisk,
    kOpSlash,
    kOpPercentage,
    kOpCaret,

    kOpEquals,
    kOpNotEquals,
    kOpLess,
    kOpLessEq,
    kOpGreater,
    kOpGreaterEq,
    kOpLike,
    kOpNotLike,
    kOpILike,
    kOpAnd,
    kOpOr,
    kOpXor,
    kOpIn,
    kOpConcat,

    // Unary operators.
    kOpNot,
    kOpUnaryMinus,
    kOpIsNull,
    kOpExists,

    // functions
    kOpPoint,
    kOpSrid,
    kOpDistanceSphere,
    kOpCount,
    kOpMin,
    kOpMax,
    kOpSum,
    kOpAvg
};

enum DatetimeField {
    kDatetimeNone,
    kDatetimeSecond,
    kDatetimeMinute,
    kDatetimeHour,
    kDatetimeDay,
    kDatetimeMonth,
    kDatetimeYear,
};

typedef struct Expr Expr;

// Represents SQL expressions (i.e. literals, operators, column_refs).
// TODO: When destructing a placeholder expression, we might need to alter the
// placeholder_list.
struct Expr {
    explicit Expr(ExprType type);
    virtual ~Expr();

    ExprType type;

    // TODO: Replace expressions by list.
    Expr* expr;
    Expr* expr2;
    std::vector<Expr*>* exprList;
    SelectStatement* select;
    char* name;
    char* table;
    char* alias;
    double fval;
    __int128_t ival;
    __int128_t ival2;
    DatetimeField datetimeField;
    ColumnType columnType;
    bool isBoolLiteral;

    OperatorType opType;
    bool distinct;

    // Convenience accessor methods.

    [[nodiscard]] bool isType(ExprType exprType) const;

    [[nodiscard]] bool isLiteral() const;

    [[nodiscard]] bool hasAlias() const;

    [[nodiscard]] bool hasTable() const;

    [[nodiscard]] const char* getName() const;

    // Static constructors.

    static Expr* make(ExprType type);

    static Expr* makeOpUnary(OperatorType op, Expr* expr);

    static Expr* makeOpBinary(Expr* expr1, OperatorType op, Expr* expr2);

    static Expr* makeBetween(Expr* expr, Expr* left, Expr* right);

    static Expr* makeCaseList(Expr* caseListElement);

    static Expr* makeCaseListElement(Expr* when, Expr* then);

    static Expr* caseListAppend(Expr* caseList, Expr* caseListElement);

    static Expr* makeCase(Expr* expr, Expr* when, Expr* elseExpr);

    static Expr* makeLiteral(__int128_t val);

    static Expr* makeLiteral(double val);

    static Expr* makeLiteral(char* val);

    static Expr* makeLiteral(bool val);

    static Expr* makeNullLiteral();

    static Expr* makeColumnRef(char* name);

    static Expr* makeColumnRef(char* table, char* name);

    static Expr* makeStar();

    static Expr* makeStar(char* table);

    static Expr* makeFunctionRef(char* func_name, std::vector<Expr*>* exprList, bool distinct);

    static Expr* makeArray(std::vector<Expr*>* exprList);

    static Expr* makeArrayIndex(Expr* expr, int64_t index);

    static Expr* makeParameter(int id);

    static Expr* makeSelect(SelectStatement* select);

    static Expr* makeExists(SelectStatement* select);

    static Expr* makeInOperator(Expr* expr, std::vector<Expr*>* exprList);

    static Expr* makeInOperator(Expr* expr, SelectStatement* select);

    static Expr* makeExtract(DatetimeField datetimeField1, Expr* expr);

    static Expr* makeCast(Expr* expr, ColumnType columnType);

    static Expr* makePoint(std::vector<Expr*>* exprList);

    static Expr* makeSrid(Expr* expr,__int128 spatialReferenceSystemID);

    static Expr* makeDistanceSphere(Expr* expr,Expr* expr2);

    static Expr* makeCount(Expr* expr);

    static Expr* makeMin(Expr* expr);

    static Expr* makeMax(Expr* expr);

    static Expr* makeSum(Expr* expr);

    static Expr* makeAvg(Expr* expr);

};

// Zero initializes an Expr object and assigns it to a space in the heap
// For Hyrise we still had to put in the explicit NULL constructor
// http://www.ex-parrot.com/~chris/random/initialise.html
// Unused
#define ALLOC_EXPR(var, type)             \
    Expr* var;                            \
    do {                                  \
        Expr zero = {type};               \
        var = (Expr*)malloc(sizeof *var); \
        *var = zero;                      \
    } while (0);
#undef ALLOC_EXPR

}