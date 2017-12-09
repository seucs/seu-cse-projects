%{

%}

%token	c d

%%

SS 		:	S
		;

S 		:	C C
		;
C		:	c C
		|	d
		;

%%


