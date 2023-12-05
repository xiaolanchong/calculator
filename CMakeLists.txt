cmake_minimum_required(VERSION 3.0)

project(calculator)

set(SRC Lexer.h
	Lexer.cpp
	LLParser.h
	LLParser.cpp
	LRParser.h
	LRParser.cpp
	Calculator.cpp)

add_executable(calculator ${SRC})
target_compile_features(calculator PRIVATE cxx_std_17)

