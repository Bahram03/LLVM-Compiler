#include "AST.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/InitLLVM.h"
#include "llvm/Support/raw_ostream.h"


class OptVisitor : public ASTVisitor {

    std::vector<llvm::StringRef> alive;
    std::vector<llvm::StringRef> aliveDec;
    bool b;

    public:
    OptVisitor(){
        alive.push_back(llvm::StringRef("result"));
        aliveDec.push_back(llvm::StringRef("result"));
    }

    
    virtual void visit(Statement &Node) {}
    virtual void visit(IfStatement &Node) {}
    virtual void visit(ElifStatement &Node) {}
    virtual void visit(ElseStatement &Node) {}
    virtual void visit(Condition &Node) {}
    virtual void visit(LoopStatement &Node) {}
    virtual void visit(Conditions &Node) {}
    
    virtual void visit(MSM &msm){
        auto v = msm.getStatements();
        auto end = v.end();
        auto begin = v.begin();
        do {
            end--;
            (*end)->accept(*this);
            if(b){
                v.erase(end);
            }
        } while(end != begin);
        
        msm.setStatements(v);
    }

    virtual void visit(AssignStatement &statement){
        Final* lValue = statement.getLValue();
        auto findItr = find(alive.begin(), alive.end(), lValue->getVal());
        if(findItr != alive.end()){
            if(statement.getAssignmentOP() == AssignStatement::AssOp::Assign){
                alive.erase(findItr);
            }
            statement.getRValue()->accept(*this);
            b = false;
            return;
        }
        b =  true;
    }

    virtual void visit(DecStatement &statement){
        llvm::StringRef lValue = *(statement.getVars().begin());
        auto findItr = find(alive.begin(), alive.end(), lValue);
        if(findItr != alive.end()){
            llvm::errs() << lValue;
            alive.erase(findItr);
            auto rightV = *(statement.getExprs().begin());
            rightV->accept(*this);
            b = false;
            return;
        }
        
        if(find(aliveDec.begin(), aliveDec.end(), lValue) == aliveDec.end())
            b = true;
        else 
            b = false;
    }
    
    virtual void visit(Expression &statement){
        auto LValue = statement.getLeft();
        auto RValue = statement.getRight();

        LValue->accept(*this);

        RValue->accept(*this);
        
    }
    
    virtual void visit(Final &statement){
        if(statement.getKind() == Final::ValueKind::Ident){
            alive.push_back(statement.getVal());
            aliveDec.push_back(statement.getVal());
        }
    }
};

class Optimization{
    public:
    void Optimize(AST *Tree) {
        OptVisitor Op;
        Tree->accept(Op);
    }
};