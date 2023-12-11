#ifndef AST_H
#define AST_H

#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"

// Forward declarations of classes used in the AST
class AST;
class Expr;
class Goal;
class BinaryOp;
class Declaration;
class Final;
class Condition;
class Equation;
class If;
class Elif;
class Else;
class Loop;
class C;


// ASTVisitor class defines a visitor pattern to traverse the AST
class ASTVisitor
{
public:
  // Virtual visit functions for each AST node type
  virtual void visit(AST &) {}             
  virtual void visit(Expr &) {}             
  virtual void visit(Goal &) = 0;            
  virtual void visit(Factor &) = 0;         
  virtual void visit(BinaryOp &) = 0;  
  virtual void visit(Assignment &) = 0;      
  virtual void visit(Declaration &) = 0;    
  virtual void visit(Equation &) = 0;
  virtual void visit(If &) = 0;
  virtual void visit(Elif &) = 0;
  virtual void visit(Else &) = 0;
  virtual void visit(Loop &) = 0;
  virtual void visit(C &) = 0;
};

// AST class serves as the base class for all AST nodes
class AST
{
public:
  virtual ~AST() {}
  virtual void accept(ASTVisitor &V) = 0;    // Accept a visitor for traversal
};

// Expr class represents an expression in the AST
class Expr : public AST
{
public:
  Expr() {}
};

// Goal class represents a group of expressions in the AST
class Goal : public Expr
{
  using ExprVector = llvm::SmallVector<Expr *>;

private:
  ExprVector exprs;                          // Stores the list of expressions

public:
  Goal(llvm::SmallVector<Expr *> exprs) : exprs(exprs) {}

  llvm::SmallVector<Expr *> getExprs() { return exprs; }

  ExprVector::const_iterator begin() { return exprs.begin(); }

  ExprVector::const_iterator end() { return exprs.end(); }

  virtual void accept(ASTVisitor &V) override
  {
    V.visit(*this);
  }
};

// Final class represents a final in the AST (either an identifier or a number)
class Final : public Expr
{
public:
  enum ValueKind
  {
    Id,
    Num
  };

private:
  ValueKind Kind;                            // Stores the kind of factor (identifier or number)
  llvm::StringRef Val;                       // Stores the value of the factor

public:
  Final(ValueKind Kind, llvm::StringRef Val) : Kind(Kind), Val(Val), Expr() {}

  ValueKind getKind() { return Kind; }

  llvm::StringRef getVal() { return Val; }

  virtual void accept(ASTVisitor &V) override
  {
    V.visit(*this);
  }
};

// BinaryOp class represents a binary operation in the AST (plus, minus, multiplication, division, power, modxq)
class BinaryOp : public Expr
{
public:
  enum Operator
  {
    Plus,
    Minus,
    Mul,
    Div,
    Mod,
    Pow
  };

private:
  Expr *Left;                               // Left-hand side expression
  Expr *Right;                              // Right-hand side expression
  Operator Op;                              // Operator of the binary operation

public:
  BinaryOp(Operator Op, Expr *L, Expr *R) : Op(Op), Left(L), Right(R), Expr() {}

  Expr *getLeft() { return Left; }

  Expr *getRight() { return Right; }

  Operator getOperator() { return Op; }

  virtual void accept(ASTVisitor &V) override
  {
    V.visit(*this);
  }
};


// Declaration class represents a variable declaration with an initializer in the AST
class Declaration : public Expr
{
  private:
    llvm::SmallVector<llvm::StringRef, 8> Vars;
    llvm::SmallVector<Expr*> Exprs;

public:
  Declaration(VarVector Vars, ExprVector Exprs) : Vars(Vars), Exprs(Exprs), Expr() {}

  llvm::SmallVector<llvm::StringRef, 8> getVars() { return Vars; }
  llvm::SmallVector<Expr*> getExprs() { return Exprs; }

  virtual void accept(ASTVisitor &V) override
  {
    V.visit(*this);
  }
};

class Equation : public AST
{
  public:
    enum Operator
    {
      equal,
      plusequal,
      minusequal,
      starequal,
      slashequal,
      percentequal
    };

  private:
    Final* Id;
    Expr* E;
    Operator Op;
  
  public:
    Equation(Final *Id, Expr *E, Operator Op) : Id(Id), E(E), Op(Op) {}
    
    Final *getId() { return Id; }

    Expr *getE() { return E; }

    Operator getOp(){return Op;}

}

class C : public AST
{
  public:
    enum LogicOp
    {
      And,
      Or
    };
  private:
    C *Left;
    C *Right;
    LogicOp LOp;

  public:
    C(Condition *L, Condition *R, LogicOp LO) : Left(L), Right(R), LOp(LO){} C() {}
    C *getLeft() {return Left;}
    C *getRight() { return Right;}
    C getLOp() {return Lop;} 

    virtual void accept(ASTVisitor &V) override
    {
        V.visit(*this);
    } 
}

class Condition : public C
{
  public:
    enum OperatorCondition
    {
      greater,
      less,
      greaterequal,
      lessequal,
      equalequal,
      notequal
    };
  private:
    Expr *Left;
    Expr *Right;
    OperatorCondition OpC;
  public:
    Condition(Expr *L, Expr *R, OperatorCondition C) : Left(L), Right(R), OpC(C), C() {}

    Expr* getLeft(){return Left;}
    Expr* getRight(){return Right;}
    OperatorCondition getOpC(){return OpC;}

    virtual void accept(ASTVisitor &V) override
    {
        V.visit(*this);
    }
}

class If : public AST
{
  private:
    C* conditions;
    llvm::SmallVector<Equation *> equations;
    llvm::SmallVector<Elif *> elifs;
    Else* elsestate;

  public:
    If(C* cs, llvm::SmallVector<Equation *> eqs, llvm::SmallVector<Elif *> elfs,  Else* els) : 
    conditions(cs), equation(eqs), elifs(elfs), elsestate(els) {}
    Else *getElsestate(){return elsestate;}
    C *getConditions(){return conditions;}
    llvm::SmallVector<Equation *> getEquations(){return equations;}
    llvm::SmallVector<Elif *> getElifs() {return elifs;}

    virtual void accept(ASTVisitor &V) override
    {
        V.visit(*this);
    }
}

class Else : public If
{
  private: 
    llvm::SmallVector<Equation *> equations;
   
  public:
    Else(lvm::SmallVector<Equation *> eqs) : equations(eqs), if() {}
    llvm::SmallVector<Equation *> getEquations(){return equations;}

    virtual void accept(ASTVisitor &V) override
    {
        V.visit(*this);
    }
}

class Elif : public If
{
  private: 
    C* conditions;
    llvm::SmallVector<Equation *> equations;
   
  public:
    Else(C* cs, lvm::SmallVector<Equation *> eqs) : conditions(cs) , equations(eqs), if() {}
    llvm::SmallVector<Equation *> getEquations(){return equations;}
    C* getConditions(){return conditions;}
    virtual void accept(ASTVisitor &V) override
    {
        V.visit(*this);
    }
}

class Loop : public AST
{
  private: 
    C* conditions;
    llvm::SmallVector<Equation *> equations;
   
  public:
    Loop(C* cs, lvm::SmallVector<Equation *> eqs) : conditions(cs) , equations(eqs), if() {}
    C* getConditions(){return conditions;}
    llvm::SmallVector<Equation *> getEquations(){return equations;}
    
    virtual void accept(ASTVisitor &V) override
    {
        V.visit(*this);
    }

}

#endif
