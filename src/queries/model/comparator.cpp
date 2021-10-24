#include "comparator.hpp"
#include "../../queries/builders/ast.hpp"
#include "../../repository/indexRepository.hpp"

using namespace std;
using namespace indexes;

namespace queries::model {

void ComparatorModel::parseForType(const std::vector<std::shared_ptr<BaseModel>> &el,queries::builders::AST *ast) {
    for (const auto &e : el) {
        associatedBank[e.get()] = bank;
        if (doScale.find(e.get()) == doScale.end()) {
            doScale[e.get()] = true;
        }
        switch (e->getClassInstanceType()) {
            case ClassInstanceInteger: {
                __uint128_t val = e->vals.at(0);
                if (!isaFloat) {
                    len = max(len, getKernelLength(val));
                } else {
                    len = max(len, min(getKernelLength(val), 64));
                }
                isSigned |= (__int128_t)val < 0;
                bank++;
                toFilter = (__int128_t)e->vals.at(0);
            }
                break;
            case ClassInstanceFloat: {
                isaFloat = true;
                bank++;
                toFilter = (__int128_t)e->vals.at(0);
            }
                break;
            case ClassInstanceString:
                if (isDate) {
                    bank++;
                }
                break;
            case ClassInstanceSrid:
                parseForType(e->getExprList(), ast);
                break;
            case ClassInstanceDistanceSphere:
                isaFloat = true;
                if (len > 64) len = 64;
                parseForType(e->getExprList(), ast);
                break;
            case ClassInstanceColumnRef : {
                shared_ptr<IndexManager> im = IndexRepository::getInstance()->getIndex(ast->getColumnMapping()->getIndexName(), e->values.at(0).at(0));
                isaFloat |= isFloat(im->getStorageType()) || isDouble(im->getStorageType()) || im->getStorageType() == mapping::storage_type_geopoint;
                int le = im->getStorageType() == mapping::storage_type_geopoint ? 32 : getKernelLengthForColumn(im->getLength());
                if (!isaFloat) {
                    len = max(len, le);
                } else {
                    len = max(len, min(le, 64));
                }
                if (im->getIndexType() == mapping::index_type_date) {
                    len = max(32,len);
                }
                if (im->getIndexType() == mapping::index_type_date_nano) {
                    len = max(64,len);
                }
                isSigned |= isSignedInt(im->getStorageType());
                isDate = im->getIndexType() == mapping::index_type_date;
                if (isDate) {
                    meDate = im->getMapping();
                }
                bank++;
                indexManagers.push_back(im);
                maxScaling = max(maxScaling,im->getScale());
            }
                break;
            case ClassInstanceSlash: {
                doScale[e->getExprList().at(0).get()] = true;
                doScale[e->getExprList().at(1).get()] = false;
                if (e->getExprList().at(0)->getClassInstanceType() == ClassInstanceColumnRef && e->getExprList().at(1)->getClassInstanceType() == ClassInstanceColumnRef) {
                    shared_ptr<IndexManager> im = IndexRepository::getInstance()->getIndex(ast->getColumnMapping()->getIndexName(), e->getExprList().at(1)->values.at(0).at(0));
                    extraScale[e->getExprList().at(0).get()] = im->getScale();
                }
                parseForType(e->getExprList(), ast);
            }
                break;
            case ClassInstanceAsterisk: {
                bool scaledOne = false;
                for (auto &x : e->getExprList()) {
                    if (x->getClassInstanceType() == ClassInstanceColumnRef) {
                        scaledOne = true;
                    } else {
                        // it's an operator who takes care of scaling
                        if (x->getClassInstanceType() != ClassInstanceInteger && x->getClassInstanceType() != ClassInstanceFloat) {
                            scaledOne = true;
                        }
                    }
                }
                if (!scaledOne) {
                    bool m = true;
                    for (auto &x : e->getExprList()) {
                        if (x->getClassInstanceType() == ClassInstanceInteger || x->getClassInstanceType() == ClassInstanceFloat) {
                            doScale[x.get()] = m;
                            m = false;
                        }
                    }
                } else {
                    for (auto &x : e->getExprList()) {
                        doScale[x.get()] = false;
                    }
                }
                parseForType(e->getExprList(), ast);
            }
                break;
            case ClassInstanceMinus:
            case ClassInstanceUnaryMinus:
            case ClassInstancePlus:
            case ClassInstanceMin:
            case ClassInstanceMax:
            case ClassInstanceCaret: {
                parseForType(e->getExprList(), ast);
                break;
            }
            default:
            break;
        }
    }
}


void ComparatorModel::gen(const std::vector<std::shared_ptr<BaseModel>> &el,queries::builders::AST *ast) {
    for (const auto &e : el) {
        double currentScaling = doScale.at(e.get()) ? maxScaling : 1.0;
        bank = associatedBank.at(e.get());
        switch (e->getClassInstanceType()) {
            case ClassInstanceInteger: {
                __uint128_t val = e->vals.at(0);
                __uint128_t a;
                if (isaFloat) {
                    if (len == 32) {
                        a = floatToUint32((float)currentScaling*(float)(int32_t)val);
                        codegen.loadConstant(a,len,true,bank,isInClause);
                    } else {
                        a = doubleToUint64(currentScaling*(double)(int64_t)val);
                        codegen.loadConstant(a,len,true,bank,isInClause);
                    }
                } else {
                    a = val * (__uint128_t)currentScaling;
                    codegen.loadConstant(a,len,false,bank,isInClause);
                }
            }
                break;
            case ClassInstanceFloat: {
                __uint128_t val = e->vals.at(0);
                if (len == 64) {
                    auto w = *(double*)&e->vals.at(0);
                    w *= currentScaling;
                    *(double*)&val = w;
                    codegen.loadConstant(val,64,true,bank,isInClause);
                } else {
                    auto w = *(double*)&e->vals.at(0);
                    w *= currentScaling;
                   *(float*)&val = (float)w;
                    codegen.loadConstant(val,32,true,bank,isInClause);
                }
            }
                break;
            case ClassInstanceString:
                if (isDate) {
                    bool fail;
                    __uint128_t val = stringToDate(meDate.isFTDate,meDate.t == mapping::index_type_date_nano,meDate.format,e->values.at(0).at(0),fail);
                    if (fail) {
                        Log::error(queryStatus,"Invalid data format " + e->values.at(0).at(0));
                    } else {
                        codegen.loadConstant(val,meDate.t == mapping::index_type_date ? 32 : 64,false,bank,isInClause);
                        toFilter = (__int128_t)val;
                    }
                }
                break;
            case ClassInstanceSrid: {
                bool is;
                __uint128_t ace = e->getResult(is);
                codegen.loadConstant(ace, 64, false, bank, true);   // push value on stack
            }
                break;
            case ClassInstanceDistanceSphere: {
                hasFunctionInside = true;
                int g0,w=0;
                for (const auto &n : e->getExprList()) {
                    if (n->getClassInstanceType() == ClassInstanceColumnRef) {
                        g0 = w;
                    }
                    w++;
                }
                gen(e->getExprList(),ast);
                codegen.distanceSphere(associatedBank.at(e->getExprList().at(g0).get()),len);
            }
                break;
            case ClassInstanceColumnRef : {
                shared_ptr<IndexManager> im =  IndexRepository::getInstance()->getIndex(ast->getColumnMapping()->getIndexName(), e->values.at(0).at(0));
                bool isf = isFloat(im->getStorageType()) || isDouble(im->getStorageType());
                if (extraScale.find(e.get()) != extraScale.end()) {
                    currentScaling *= extraScale.at(e.get());       // case of column divided by column
                }
                if (im->getStorageType() == mapping::storage_type_enum) {
                    // Code generated for each chunk
                } else if (im->getStorageType() == mapping::storage_type_geopoint) {
                    // Geopoint stored as a packed 64 bit int
                    codegen.fetchVariable(64,false,false,64,false,sourceIndex,bank,1);
                } else {
                    codegen.fetchVariable((int)im->getLength(),isf,isSignedInt(im->getStorageType()),len,isaFloat,sourceIndex,bank,currentScaling/im->getScale());
                }
                sourceIndex++;
            }
                break;
            case ClassInstanceSlash:
                gen(e->getExprList(),ast);
                codegen.div(len,isaFloat,associatedBank.at(e->getExprList().at(0).get()),associatedBank.at(e->getExprList().at(1).get()));
                break;
            case ClassInstanceMinus:
                gen(e->getExprList(),ast);
                codegen.sub(len,isaFloat,associatedBank.at(e->getExprList().at(0).get()),associatedBank.at(e->getExprList().at(1).get()));
                break;
            case ClassInstanceUnaryMinus:
                // should not be there
                break;
            case ClassInstanceAsterisk:
                gen(e->getExprList(),ast);
                codegen.mul(len,isaFloat,associatedBank.at(e->getExprList().at(0).get()),associatedBank.at(e->getExprList().at(1).get()));
                break;
            case ClassInstancePlus:
                gen(e->getExprList(),ast);
                codegen.add(len,isaFloat,associatedBank.at(e->getExprList().at(0).get()),associatedBank.at(e->getExprList().at(1).get()));
                break;
            case ClassInstanceCaret: {
                gen(e->getExprList(),ast);
                codegen.xor_mm(len,isaFloat,associatedBank.at(e->getExprList().at(0).get()),associatedBank.at(e->getExprList().at(1).get()));
                break;
            }
            case ClassInstanceAnd:
                break;
            case ClassInstanceBetween:
                gen(e->getExprList(),ast);
                codegen.between(len,isaFloat,isSigned,associatedBank.at(e->getExprList().at(0).get()),associatedBank.at(e->getExprList().at(1).get()),associatedBank.at(e->getExprList().at(2).get()));
                break;
            case ClassInstanceExists:
            case ClassInstanceHaving:
            case ClassInstanceInsert:
            case ClassInstanceNone:
            case ClassInstanceOffset:
            case ClassInstanceValues:
            case ClassInstanceArray:
            case ClassInstanceConcat:
            case ClassInstanceExtract:
            case ClassInstanceILike:
            case ClassInstanceIntersect:
            case ClassInstanceLike:
            case ClassInstanceNot:
            case ClassInstanceOr:
            case ClassInstanceXor:
            case ClassInstancePoint:
            case ClassInstanceStar:
            case ClassInstanceWhere:
            case ClassInstanceCase:
            case ClassInstanceImportFile:
            case ClassInstanceIsNull:
            case ClassInstanceLimit:
            case ClassInstanceOrder:
            case ClassInstanceRoot:
            case ClassInstanceBase:
            case ClassInstanceCaseListElement:
            case ClassInstanceExcept:
            case ClassInstanceIn:
            case ClassInstanceNotLike:
            case ClassInstancePercentage:
            case ClassInstanceSelect:
            case ClassInstanceUnion:
            case ClassInstanceCount:
                break;
            case ClassInstanceMin:
                gen(e->getExprList(),ast);
                codegen.min(len,isaFloat,associatedBank.at(e->getExprList().at(0).get()),associatedBank.at(e->getExprList().at(1).get()));
                break;
            case ClassInstanceMax:
                gen(e->getExprList(),ast);
                codegen.max(len,isaFloat,associatedBank.at(e->getExprList().at(0).get()),associatedBank.at(e->getExprList().at(1).get()));
                break;
            case ClassInstanceSum:
            case ClassInstanceAvg:
            case ClassInstanceLessEq:
            case ClassInstanceEquals:
            case ClassInstanceGreater:
            case ClassInstanceNotEquals:
            case ClassInstanceGreaterEq:
            case ClassInstanceLess:
            case ClassInstanceDrop:
            case ClassInstanceDelete:
                break;
        }
    }
}

void ComparatorModel::genComparator() {
    switch (getClassInstanceType()) {
        case ClassInstanceLessEq:
            codegen.cmple(len, isaFloat, isSigned, associatedBank.at(exprList.at(0).get()), associatedBank.at(exprList.at(1).get()));
            break;
        case ClassInstanceEquals:
            codegen.cmpeq(len, isaFloat, isSigned, associatedBank.at(exprList.at(0).get()), associatedBank.at(exprList.at(1).get()));
            break;
        case ClassInstanceGreater:
            codegen.cmpgt(len, isaFloat, isSigned, associatedBank.at(exprList.at(0).get()), associatedBank.at(exprList.at(1).get()));
            break;
        case ClassInstanceNotEquals:
            codegen.cmpne(len, isaFloat, isSigned, associatedBank.at(exprList.at(0).get()), associatedBank.at(exprList.at(1).get()));
            break;
        case ClassInstanceGreaterEq:
            codegen.cmpge(len, isaFloat, isSigned, associatedBank.at(exprList.at(0).get()), associatedBank.at(exprList.at(1).get()));
            break;
        case ClassInstanceLess:
            codegen.cmplt(len, isaFloat, isSigned, associatedBank.at(exprList.at(0).get()), associatedBank.at(exprList.at(1).get()));
            break;
        case ClassInstanceBetween:
            codegen.between(len, isaFloat, isSigned, associatedBank.at(exprList.at(0).get()), associatedBank.at(exprList.at(1).get()), associatedBank.at(exprList.at(2).get()));
            break;
        case ClassInstanceIn: {
            if (len == 32) {
               codegen.in32(associatedBank.at(exprList.at(0).get()));
            } else if (len == 64) {
                codegen.in64(associatedBank.at(exprList.at(0).get()));
            }
        }
            break;
        case ClassInstanceIsNull:
            hasFunctionInside = true;
            codegen.isNull();
            break;
        case ClassInstanceLike: {
            // handled in like class
            break;
        }
        default:
            break;
    }
}

void ComparatorModel::parseSubExpression(queries::builders::AST *ast) {
    queryStatus = ast->getQueryStatus();
    // Checks
    for (const auto &e : exprList) {
        switch (e->getClassInstanceType()) {
            case ClassInstanceColumnRef : {
                if (e->values.empty()) {
                    Log::error(queryStatus, "ColumnRef is empty. " + ast->getSaveqQuery());
                    return;
                }
                if (!ast->getColumnMapping()->hasEntry(e->values.at(0).at(0))) {
                    Log::error(ast->getQueryStatus(),
                               "ColumnRef " + e->values.at(0).at(0) + " cannot be resolved. " +
                               ast->getSaveqQuery());
                    return;
                }
                shared_ptr<IndexManager> im =  IndexRepository::getInstance()->getIndex(ast->getColumnMapping()->getIndexName(), e->values.at(0).at(0));
                if (im == nullptr) {
                    Log::error(ast->getQueryStatus(),
                               "ColumnRef " + e->values.at(0).at(0) + " cannot be resolved. " +
                               ast->getSaveqQuery());
                    return;
                }
            }
            default:
                break;
        }
    }
    isaFloat = false;
    isSigned = false;
    isDate = false;
    isInClause = getClassInstanceType() == ClassInstanceIn;
    len = 32;
    bank = 0;
    sourceIndex = 0;
    maxScaling = 1;
    associatedBank[this] = bank;
    parseForType(exprList,ast);
    bank = 0;
    gen(exprList,ast);
    genComparator();
    //codegen.illegalInstruction();
    codegen.flush();
    Log::debug(string("Query code created as ")+to_string(len)+ "bit " + (isaFloat ? "" :(isSigned ? "signed " : "unsigned "))  + (isaFloat? "float" : "integers") + " numbers.  Number of X86 instructions:"+ to_string(codegen.getSize()));
}

}