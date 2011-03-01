%{
#include <stdio.h>
#include <string.h>
#include <memory.h>

#include "data.h"
#include "inf.l.h"

DATA**	dataptr;

int	yyerror(const char* msg)
{
	fprintf(stderr,"%s\n",msg);
	return 0;
}



%}
%union {
	float	float_val;
	int	int_val;
	char*	string_ptr;
}

%token ASSIGN COLON MULTIPLY
%token	RAW_FILE IKDTREE_FILE NORMAL_FILE RATIO RESOLUTION SAMPLE_TYPE BYTEORDER VALUEMAX VALUEMIN
%token	INT FLOAT STRING
%token	ST_USHORT ST_UCHAR
%token	BIGENDIAN LITTLEENDIAN

%type <int_val> INT
%type <float_val> FLOAT
%type <string_ptr> STRING
%type <float_val> value
%type <int_val> sampletype
%type <int_val> byteorder

%%

inf_body:	commandlist
		{
			int i;
			float min;
			
			if(!((*dataptr)->flag&DATA_FIELD_RATIO))
			{
				(*dataptr)->ratio[0]=1;
				(*dataptr)->ratio[1]=1;
				(*dataptr)->ratio[2]=1;
			}
			
			min=9999999.0f;
			for(i=0;i<3;++i)
			{
				if((*dataptr)->ratio[i]<min) min=(*dataptr)->ratio[i];
			}
			for(i=0;i<3;++i)
			{
				(*dataptr)->m_ratio[i]=(*dataptr)->ratio[i]/min;
			}
			for(i=0;i<3;++i)
			{
				(*dataptr)->m_resol[i]=(*dataptr)->resol[i]*(*dataptr)->m_ratio[i];
			}
			
			min=9999999.0f;
			for(i=0;i<3;++i)
			{
				if((*dataptr)->m_resol[i]<min) min=(*dataptr)->m_resol[i];
			}
			for(i=0;i<3;++i)
			{
				(*dataptr)->unit_dim[i]=(float)(*dataptr)->m_resol[i]/min;
			}
		}

commandlist:	commandlist commandlist
		|command

command:	RAW_FILE ASSIGN STRING
		{
			(*dataptr)->rawfile_ptr=fopen($3,"rb");
			if(!(*dataptr)->rawfile_ptr) fputs("Failed opening file",stderr);
			(*dataptr)->flag|=DATA_FIELD_RAW_FILE;
			(*dataptr)->rawfilename=malloc(strlen($3)+1);
			strcpy((*dataptr)->rawfilename,$3);

		}
		|IKDTREE_FILE ASSIGN STRING
		{
			(*dataptr)->ikdtreefile_ptr=fopen($3,"rb");
			if(!(*dataptr)->ikdtreefile_ptr) fputs("Failed opening file",stderr);
			
			(*dataptr)->ikdtreefilename=malloc(strlen($3)+1);
			strcpy((*dataptr)->ikdtreefilename,$3);
		}
		|NORMAL_FILE ASSIGN STRING
		{
			(*dataptr)->normalfile_ptr=fopen($3,"rb");
			if(!(*dataptr)->normalfile_ptr) fputs("Failed opening file",stderr);
			
			(*dataptr)->normalfilename=malloc(strlen($3)+1);
			strcpy((*dataptr)->normalfilename,$3);
		}
		|RESOLUTION ASSIGN INT MULTIPLY INT MULTIPLY INT
		{
			(*dataptr)->resol[0]=$3;
			(*dataptr)->resol[1]=$5;
			(*dataptr)->resol[2]=$7;
			(*dataptr)->flag|=DATA_FIELD_RESOLUTION;
			(*dataptr)->totalsize=$3*$5*$7;
		}
		|RATIO ASSIGN value COLON value COLON value
		{
			(*dataptr)->ratio[0]=$3;
			(*dataptr)->ratio[1]=$5;
			(*dataptr)->ratio[2]=$7;
			(*dataptr)->flag|=DATA_FIELD_RATIO;
		}
		|SAMPLE_TYPE ASSIGN sampletype
		{
			(*dataptr)->sample_type=$3;
			(*dataptr)->flag|=DATA_FIELD_SAMPLE_TYPE;
		}
		|BYTEORDER ASSIGN byteorder
		{
			(*dataptr)->byte_order=$3;
			(*dataptr)->flag|=DATA_FIELD_BYTE_ORDER;
		}
		|VALUEMAX ASSIGN value
		{
			(*dataptr)->vmax=$3;
			(*dataptr)->flag|=DATA_FIELD_VALUE_MAX;
		}
		|VALUEMIN ASSIGN value
		{
			(*dataptr)->vmin=$3;
			(*dataptr)->flag|=DATA_FIELD_VALUE_MIN;
		};

byteorder:	BIGENDIAN
		{
			$$=BYTE_ORDER_BIG_ENDIAN;
		}
		|LITTLEENDIAN
		{
			$$=BYTE_ORDER_LITTLE_ENDIAN;
		};

sampletype:	ST_UCHAR
		{
			$$=SAMPLE_TYPE_UCHAR;
			(*dataptr)->sample_size=1;
		}
		|ST_USHORT
		{
			$$=SAMPLE_TYPE_USHORT;
			(*dataptr)->sample_size=2;
		};

value:		INT
		{
			$$=$1;
		}
		|FLOAT
		{
			$$=$1;
		};

%%


