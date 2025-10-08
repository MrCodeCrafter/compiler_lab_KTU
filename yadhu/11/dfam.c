//enetr number of ststes : 5
A B C D E
ENTER NUM OF INPUT SYMBOLS  : 2
ENTER TWO INPUT SYMBOLS 
a b 
enter transition foreach state 
for each satte the destination states for symbols in order 
a b 
example format of < stae name > < dst1 > < dest2 >
A B C
B B D 
C B C 
D B E 
E B C
ENTER NUMBER OF FINAL STATES : 1
ENTER NAMES OF FINAL SATTAES : E

MINIMISED DFA GROUPS  { groups }
A = { A, C }
B = { B }
C = { D }
D = { E }

minimised DFA  trabsition table :
STATE a   b
A	  B	  A
B     B   C
C     B   D
D     B   A

FINAL STATES IN MINIMISD DFA :
D
