cmake_minimum_required(VERSION 3.14)
project(wordAnalyzer_C C)

set(CMAKE_C_STANDARD 90)

add_executable(wordAnalyzer_C src/main.c src/word.h src/reserved.c src/char.c src/symble.c src/syntax.c src/syntax.h src/symbleTable.c src/const.h src/symbleTable.h src/error.h src/error.c src/mid.c src/mid.h src/generate.c src/generate.h)