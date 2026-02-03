CXX = g++
CC = gcc
CFLAGS = -g -Wall -Iast
CXXFLAGS = -g -Wall -std=c++11 -Iast

TARGET = miniC_compiler

LEX_SRC = miniC.l
YACC_SRC = miniC.y
AST_SRC = ast/ast.c
SEMANTIC_SRC = semantic.cpp
DRIVER_SRC = driver.cpp

LEX_GEN = lex.yy.c
YACC_GEN = y.tab.c y.tab.h

OBJS = lex.yy.o y.tab.o ast.o semantic.o driver.o

all: $(TARGET)

y.tab.c y.tab.h: $(YACC_SRC)
	yacc -d $(YACC_SRC)

lex.yy.c: $(LEX_SRC) y.tab.h
	lex $(LEX_SRC)

y.tab.o: y.tab.c
	$(CXX) $(CXXFLAGS) -c y.tab.c

lex.yy.o: lex.yy.c
	$(CXX) $(CXXFLAGS) -c lex.yy.c

ast.o: $(AST_SRC) ast/ast.h
	$(CXX) $(CXXFLAGS) -c $(AST_SRC)

driver.o: $(DRIVER_SRC) ast/ast.h
	$(CXX) $(CXXFLAGS) -c $(DRIVER_SRC)

semantic.o: $(SEMANTIC_SRC) ast/ast.h
	$(CXX) $(CXXFLAGS) -c $(SEMANTIC_SRC)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

test_parse: $(TARGET)
	@echo "=== testing parser ==="
	./$(TARGET) parser_tests/p1.c
	./$(TARGET) parser_tests/p2.c
	./$(TARGET) parser_tests/p3.c
	./$(TARGET) parser_tests/p4.c
	./$(TARGET) parser_tests/p5.c

test_good: $(TARGET)
	@echo "=== testing good programs (should pass) ==="
	./$(TARGET) semantic_analysis_tests/p1_good.c
	./$(TARGET) semantic_analysis_tests/p2_good.c
	./$(TARGET) semantic_analysis_tests/p3_good.c

test_bad: $(TARGET)
	@echo "=== testing bad programs (should fail) ==="
	./$(TARGET) semantic_analysis_tests/p1_bad.c
	./$(TARGET) semantic_analysis_tests/p2_bad.c
	./$(TARGET) semantic_analysis_tests/p3_bad.c
	./$(TARGET) semantic_analysis_tests/p4_bad.c

test: test_parse test_good test_bad

clean:
	rm -f $(TARGET) $(OBJS) $(YACC_GEN) $(LEX_GEN)

.PHONY: all test test_parse test_good test_bad clean