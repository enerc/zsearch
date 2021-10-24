#include "xor.hpp"
#include "../../queries/builders/ast.hpp"

using namespace std;

namespace queries::model {

void XorModel::execute(queries::builders::AST *ast) {
    if (exprList.size() != 2) {
        Log::error(ast->getQueryStatus(),"SQL query XOR should have 2 childrens. "+ast->getSaveqQuery());
        return;
    }
    auto a = exprList.at(0)->getWorkingSet();
    auto b = exprList.at(1)->getWorkingSet();
    if (a == nullptr || b == nullptr || exprList.at(0)->getWorkingSetSize() != exprList.at(1)->getWorkingSetSize()) {
        Log::error(ast->getQueryStatus(),"SQL query XOR failed "+ast->getSaveqQuery());
        return;
    }
    allocWorkingSet(exprList.at(0)->getWorkingSetSize());
    fastXor(workingSet,a,b,workingSetSize);
    exprList.at(0)->freeWorkingSet();
    exprList.at(1)->freeWorkingSet();
    nbResults = fastCount(workingSet,workingSetSize);
}

}
