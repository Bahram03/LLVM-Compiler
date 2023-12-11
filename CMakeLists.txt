add_executable (main
  main.cpp
  CodeGen.cpp
  Lexer.cpp
  Parser.cpp
  Sema.cpp
  )
target_link_libraries(main PRIVATE ${llvm_libs})
