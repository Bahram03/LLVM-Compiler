#include "CodeGen.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

// Define a visitor class for generating LLVM IR from the AST.
namespace
{
  class ToIRVisitor : public ASTVisitor
  {
    Module *M;
    IRBuilder<> Builder;
    Type *VoidTy;
    Type *Int32Ty;
    Type *Int8PtrTy;
    Type *Int8PtrPtrTy;
    Constant *Int32Zero;
    Constant *Int32One;
    Function *CalcWriteFn;
    FunctionType *CalcWriteFnTy;    

    Value *V;
    StringMap<AllocaInst *> nameMap;

  public:
    // Constructor for the visitor class.
    ToIRVisitor(Module *M) : M(M), Builder(M->getContext())
    {
      // Initialize LLVM types and constants.
      VoidTy = Type::getVoidTy(M->getContext());
      Int32Ty = Type::getInt32Ty(M->getContext());
      Int8PtrTy = Type::getInt8PtrTy(M->getContext());
      Int8PtrPtrTy = Int8PtrTy->getPointerTo();
      Int32Zero = ConstantInt::get(Int32Ty, 0, true);
      CalcWriteFnTy = FunctionType::get(VoidTy, {Int32Ty}, false);
      CalcWriteFn = Function::Create(CalcWriteFnTy, GlobalValue::ExternalLinkage, "main_write", M);
    }

    // Entry point for generating LLVM IR from the AST.
    void run(AST *Tree)
    {
      // Create the main function with the appropriate function type.
      FunctionType MainFty = FunctionType::get(Int32Ty, {Int32Ty, Int8PtrPtrTy}, false);
      Function MainFn = Function::Create(MainFty, GlobalValue::ExternalLinkage, "main", M);

      // Create a basic block for the entry point of the main function.
      BasicBlock *BB = BasicBlock::Create(M->getContext(), "entry", MainFn);
      Builder.SetInsertPoint(BB);

      // Visit the root node of the AST to generate IR.
      Tree->accept(*this);

      // Create a return instruction at the end of the main function.
      Builder.CreateRet(Int32Zero);
    }

    // Visit function for the GSM node in the AST.
    virtual void visit(Goal &Node) override
    {
      // Iterate over the children of the GSM node and visit each child.
      for (auto I = Node.begin(), E = Node.end(); I != E; ++I)
      {
        (*I)->accept(*this);
      }
    };

virtual void visit(Equation &Node) override {
    
    Node.getE()->accept(*this);
    Value *rhsVal = V; /
    auto varName = Node.getId()->getVal();

    Value *varPtr = nameMap[varName];

    Value *varValue = Builder.CreateLoad(varPtr);

    switch (Node.getOp()) {
        case Equation::equal:
            // Simple assignment (=).
            // will be handled outside the switch-case
            break;
        case Equation::plusequal:
            // Addition-assignment (+=).
            varValue = Builder.CreateAdd(varValue, rhsVal);
            break;
        case Equation::minusequal:
            // Subtraction-assignment (-=).
            varValue = Builder.CreateSub(varValue, rhsVal);
            break;
        case Equation::starequal:
            // Multiplication-assignment (*=).
            varValue = Builder.CreateMul(varValue, rhsVal);
            break;
        case Equation::slashequal:
            // Division-assignment (/=).
            varValue = Builder.CreateSDiv(varValue, rhsVal);
            break;
        case Equation::percentequal:
            // Modulus-assignment (%=).
            varValue = Builder.CreateSRem(varValue, rhsVal);
            break;
    }

    Builder.CreateStore(varValue, varPtr);

  
    FunctionType *CalcWriteFnTy = FunctionType::get(VoidTy, {Int32Ty}, false);

    // Create a function declaration for the "gsm_write" function.
    Function *CalcWriteFn = Function::Create(CalcWriteFnTy, GlobalValue::ExternalLinkage, "gsm_write", M);

    // Create a call instruction to invoke the "gsm_write" function with the value.
    CallInst *Call = Builder.CreateCall(CalcWriteFnTy, CalcWriteFn, {val});
}


    virtual void visit(Final &Node) override
    {
      if (Node.getKind() == Factor::Id)
      {
        // If the factor is an identifier, load its value from memory.
        V = Builder.CreateLoad(Int32Ty, nameMap[Node.getVal()]);
      }
      else
      {
        // If the factor is a literal, convert it to an integer and create a constant.
        int intval;
        Node.getVal().getAsInteger(10, intval);
        V = ConstantInt::get(Int32Ty, intval, true);
      }
    };

virtual void visit(BinaryOp &Node) override {
    // Visit the left-hand side of the binary operation and get its value.
    Node.getLeft()->accept(*this);
    Value *Left = V;

    // Visit the right-hand side of the binary operation and get its value.
    Node.getRight()->accept(*this);
    Value *Right = V;

    // Perform the binary operation based on the operator type and create the corresponding instruction.
    switch (Node.getOperator()) {
        case BinaryOp::Plus:{
            V = Builder.CreateNSWAdd(Left, Right);
            break;
        }
        case BinaryOp::Minus:{
            V = Builder.CreateNSWSub(Left, Right);
            break;
        }
        case BinaryOp::Mul:{
            V = Builder.CreateNSWMul(Left, Right);
            break;
        }
        case BinaryOp::Div:{
            V = Builder.CreateSDiv(Left, Right);
            break;
        }
        case BinaryOp::Mod:{
            V = Builder.CreateSRem(Left, Right);
            break;
        }
        case BinaryOp::Pow:{
          int rightValue = 0;
          Final * right = (Final*) Node.getRight();
          Value * left = Node.getLeft();
          right.getVal().getAsInteger(10, rightValue);
          llvm::errs()<<rightValue<<"\n";
          Value *LeftVal = Left;
          if(rightValue == 0){
            V = ConstantInt::get(Int32Ty, 1, true);
          }else{
            for(int i = 0; i < rightValue; i++){
              V = Builder.CreateNSWMul(Left, LeftVal);
            }
            V = Left;
          }
        }
    }
  }

virtual void visit(Loop& Node) override
  {
    llvm::BasicBlock* WhileCondBB = llvm::BasicBlock::Create(M->getContext(), "loopc.cond", MainFn);
    llvm::BasicBlock* WhileBodyBB = llvm::BasicBlock::Create(M->getContext(), "loopc.body", MainFn);
    llvm::BasicBlock* AfterWhileBB = llvm::BasicBlock::Create(M->getContext(), "after.loopc", MainFn);

    Builder.CreateBr(WhileCondBB);
    Builder.SetInsertPoint(WhileCondBB);
    Node.getConditions()->accept(*this);
    Value* val = V;
    Builder.CreateCondBr(val, WhileBodyBB, AfterWhileBB);
    Builder.SetInsertPoint(WhileBodyBB);
    llvm::SmallVector<Equation* > equations = Node.getEquations();
    for (auto i : equations)
      {
          (*i)->accept(*this);
      }
    Builder.CreateBr(WhileCondBB);
    Builder.SetInsertPoint(AfterWhileBB);
  }

virtual void visit(If &Node) override {
    Node.getConditions()->accept(*this);
    Value *ifCondValue = V;

    llvm::Function *TheFunction = Builder.GetInsertBlock()->getParent();

    llvm::BasicBlock *ThenBB = llvm::BasicBlock::Create(Context, "then", TheFunction);
    llvm::BasicBlock *ElseBB = llvm::BasicBlock::Create(Context, "else");
    llvm::BasicBlock *MergeBB = llvm::BasicBlock::Create(Context, "merge");

    Builder.CreateCondBr(ifCondValue, ThenBB, ElseBB);

    Builder.SetInsertPoint(ThenBB);

    llvm::SmallVector<Equation *> equations = Node.getEquations();
    for (auto *equation : equations) {
        equation->accept(*this);
    }

    Builder.CreateBr(MergeBB);
    ThenBB = Builder.GetInsertBlock(); 
    TheFunction->getBasicBlockList().push_back(ElseBB);
    Builder.SetInsertPoint(ElseBB);

    llvm::SmallVector<Elif *> elifs = Node.getElifs();
    for (auto *elif : elifs) {
        elif->accept(*this);
    }

    Builder.CreateBr(MergeBB);
    ElseBB = Builder.GetInsertBlock(); 
    TheFunction->getBasicBlockList().push_back(MergeBB);
    Builder.SetInsertPoint(MergeBB);

    Else *elseState = Node.getElsestate();
    if (elseState) {
        elseState->accept(*this);
    }
  }


  virtual void visit(Declaration &Node) override
    {
      Value *val = nullptr;

      if (Node.getExpr())
      {
        // If there is an expression provided, visit it and get its value.
        Node.getExpr()->accept(*this);
        val = V;
      }

      // Iterate over the variables declared in the declaration statement.
      for (auto I = Node.begin(), E = Node.end(); I != E; ++I)
      {
        StringRef Var = *I;

        // Create an alloca instruction to allocate memory for the variable.
        nameMap[Var] = Builder.CreateAlloca(Int32Ty);

        // Store the initial value (if any) in the variable's memory location.
        if (val != nullptr)
        {
          Builder.CreateStore(val, nameMap[Var]);
        }
      }
    };

virtual void visit(C &Node) override {
    if (Node.getLeft() && Node.getRight()) {
        Node.getLeft()->accept(*this);
        Value *LeftValue = V; 
        Node.getRight()->accept(*this);
        Value *RightValue = V; 
        switch (Node.getLOp()) {
            case C::And:
                // Perform logical AND operation.
                V = Builder.CreateAnd(LeftValue, RightValue);
                break;
            case C::Or:
                // Perform logical OR operation.
                V = Builder.CreateOr(LeftValue, RightValue);
                break;
        }
      }
    }
  };
}; // namespace

void CodeGen::compile(AST *Tree)
{
  // Create an LLVM context and a module.
  LLVMContext Ctx;
  Module *M = new Module("main.expr", Ctx);

  // Create an instance of the ToIRVisitor and run it on the AST to generate LLVM IR.
  ToIRVisitor ToIR(M);
  ToIR.run(Tree);

  // Print the generated module to the standard output.
  M->print(outs(), nullptr);
}
