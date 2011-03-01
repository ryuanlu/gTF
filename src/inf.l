%{
#include	<stdio.h>
#include	"inf.y.h"


char*	strbuf;

int	yywrap();
void	yyreset(void);


%}

%x STR


%%


\/\/.*\n	|
\;.*\n		|
\/\*.*\*\/	|
\#.*\n		{}


\=	{
		return ASSIGN;
	}

x	{
		return	MULTIPLY;
	}

\:	{
		return	COLON;
	}
\n	{}

[Rr][Aa][Ww][-_\ ][Ff][Ii][Ll][Ee]	{
						return RAW_FILE;
					}

[Rr][Ee][Ss][Oo][Ll][Uu][Tt][Ii][Oo][Nn]	{
							return RESOLUTION;
						}

[Rr][Aa][Tt][Ii][Oo]	{
				return RATIO;
			}

[Ss][Aa][Mm][Pp][Ll][Ee][-_\ ][Tt][Yy][Pp][Ee]	{
								return SAMPLE_TYPE;
							}
[Bb][Yy][Tt][Ee][-_\ ][Oo][Rr][Dd][Ee][Rr]	{
								return BYTEORDER;
							}
[Vv][Aa][Ll][Uu][Ee][-_\ ][Mm][Aa][Xx]	{
							return VALUEMAX;
						}

[Vv][Aa][Ll][Uu][Ee][-_][Mm][Ii][Nn]	{
							return VALUEMIN;
						}
[Ii][Kk][Dd][Tt][Rr][Ee][Ee][-_][Ff][Ii][Ll][Ee]	{
									return IKDTREE_FILE;
								}
[Nn][Oo][Rr][Mm][Aa][Ll][-_][Ff][Ii][Ll][Ee]	{
								return NORMAL_FILE;
							}

[0-9]+	{
		yylval.int_val=atoi(yytext);
		return INT;
	}

[0-9]+\.[0-9]+([eE][\+\-][0-9]+)?	{
						yylval.float_val=atof(yytext);
						return FLOAT;
					}

\"	{
		BEGIN STR;
		strbuf=malloc(256);
		strbuf[0]=0;
	}

<STR>[^\n\"]*	{
			yylval.string_ptr=malloc(strlen(yytext)+1);
			strcpy(yylval.string_ptr,yytext);
			return STRING;
		}
<STR>\"	{
			BEGIN 0;
		}


[Uu][Nn][Ss][Ii][Gg][Nn][Ee][Dd][\ _-][Ss][Hh][Oo][Rr][Tt]	{
										return ST_USHORT;
									}

[Uu][Nn][Ss][Ii][Gg][Nn][Ee][Dd][\ _-][Cc][Hh][Aa][Rr]	{
									return ST_UCHAR;
								}

[Bb][Ii][Gg][-_][Ee][Nn][Dd][Ii][Aa][Nn]	{
							return BIGENDIAN;
						}

[Ll][Ii][Tt][Tt][Ll][Ee][-_][Ee][Nn][Dd][Ii][Aa][Nn]	{
									return LITTLEENDIAN;
								}

%%

int	yywrap()
{
	return 1;
}

void	yyreset(void)
{
	YY_FLUSH_BUFFER;
	BEGIN 0;
}
