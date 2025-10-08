📚 Repository Contents

This repository is organized according to the major topics in Compiler Design:

1. Introduction to Compiler

Overview of compilers and interpreters

Phases of compilation

Compiler construction tools

Differences between compiler and interpreter

Sample programs: “Hello, World!” in Lex & Yacc

2. Lexical Analysis

Lexical Analyzer: Role, structure, and functions

Tokens, Lexemes, Patterns

Regular expressions & finite automata (DFA/NFA)

Lex programs for:

Counting lines, words, and characters

Identifiers, keywords, and operators

Removing comments from code

Algorithm for lexical analysis

3. Syntax Analysis

Context-Free Grammars (CFGs)

Parse trees and derivations

Top-Down Parsing:

Recursive Descent Parsing

Predictive Parsing (LL(1))

Bottom-Up Parsing:

Shift-Reduce Parsing

LR Parsing (LR(0), SLR(1), LALR(1), Canonical LR(1))

Syntax-directed translation

4. Semantic Analysis

Syntax vs Semantic analysis

Type checking

Attribute grammars and semantic rules

Symbol tables and their structure

5. Intermediate Code Generation

Need for intermediate representation

Three-address code

Syntax trees for expressions

Translating statements into intermediate code

6. Code Optimization

Purpose and types of optimization:

Local, Global, Loop optimization

Examples of common optimizations

7. Code Generation

Generating target code from intermediate representation

Register allocation techniques

Simple code generation examples

8. Error Handling

Types of errors in compilation

Lexical, syntax, semantic errors

Panic-mode, phrase-level, and error recovery strategies

9. Practical Programs

Lex/Yacc programs for:

Token counting

Keyword/Identifier/Operator detection

Expression evaluation

Simple calculators

C programs for parsing expressions

10. Lab Viva Preparation

Common questions with answers

Comparison tables (Top-down vs Bottom-up, Compiler vs Interpreter)

Sample tokens, parse trees, and symbol table examples

Step-by-step explanation of algorithms

🛠 Tools & Technologies

Lex / Flex – For lexical analysis

Yacc / Bison – For syntax analysis

C Language – To integrate Lex/Yacc programs

GNU Compiler (gcc) – To compile C programs
