/*

MultiX Network Applications Development Toolkit.
Copyright (C) 2007, Moshe Shitrit, Mitug Distributed Systems Ltd., All Rights Reserved.

This file is part of MultiX.

MultiX is free software; you can redistribute it and/or modify it under the terms of the 
GNU General Public License as published by the Free Software Foundation; 
either version 2 of the License, or (at your option) any later version. 

MultiX is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with this program;
if not, write to the 
Free Software Foundation, Inc., 
59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
--------------------------------------------------------------------------------
Author contact information: 
msr@mitug.co.il
--------------------------------------------------------------------------------

*/
#include "ISO8583Shared.h"
#include "ISO8583Utilities.h"

CISO8583Utilities::CISO8583Utilities(void)
{
}

CISO8583Utilities::~CISO8583Utilities(void)
{
}

std::string	CISO8583Utilities::BCDToString(const	byte_t	*Data,int	Size,bool	bWithHexDigits,bool	bLeftJust)
{
	char	*S	=	new	char[Size	+	1];
	if(bLeftJust)
	{
		const	byte_t	*Ptr	=	Data;
		for(int	I=0;I<Size;I++)
		{
			byte_t	B;
			if(I	&	1)
			{
				B	=	*Ptr	&	0xf;
				Ptr++;
			}
			else
			{
				B	=	*Ptr	>>	4;
			}
			if(B	>	9)
			{
				if(!bWithHexDigits	||	!(B	==	0xd	||	B	==	0xf))
				{
					delete	[]	S;
					Throw();
				}
			}
			S[I]	=	B	+	0x30;
		}
	}	else
	{
		const	byte_t	*Ptr	=	&Data[(Size+1)/2	-1];
		for(int	I=Size;I>0;I--)
		{
			byte_t	B;
			if((Size	-	I)	&	1)
			{
				B	=	*Ptr	>>	4;
				Ptr--;
			}
			else
			{
				B	=	*Ptr	&	0xf;
			}
			if(B	>	9)
			{
				if(!bWithHexDigits	||	!(B	==	0xd	||	B	==	0xf))
				{
					delete	[]	S;
					Throw();
				}
			}
			S[I-1]	=	B	+	0x30;
		}
	}
	S[Size]	=	0;
	std::string	RetVal	=	S;
	delete	[]	S;
	return	RetVal;
}

int	CISO8583Utilities::HexToBin(byte_t	*Dest,std::string	HexSrc)
{
	const	char	*S	=	HexSrc.c_str();
	if(HexSrc.length()	&	1)
		return	-1;
	int	Size	=	0;
	while(*S)
	{
		if(!isxdigit(*S))
			return	-1;
		int	N;
		if(*S	>=	'0'	&&	*S	<=	'9')
			N	=	*S	-	0x30;
		else if(*S	>=	'A'	&&	*S	<=	'F')
			N	=	*S	-	'A'	+	10;
		else if(*S	>=	'a'	&&	*S	<=	'f')
			N	=	*S	-	'a'	+	10;
		else
			return	-1;
		if(Size	&	1)
			Dest[Size	>>	1]	+=	N	;
		else
			Dest[Size	>>	1]	=	N	<<	4;
		S++;
		Size++;
	}
	return	Size	>>	1;
}

int	CISO8583Utilities::ToBCD(byte_t	*Data,int	Digits,unsigned	long	Value)
{
	int	Size	=	(Digits	+	1)	>>	1;
	int	RetVal	=	Size;
	while(Size	>	0)
	{
		if(Digits	>	1)
		{
			Data[Size-1]	=	(byte_t)((Value	%	10)	+	(((Value/10)	%	10)	<<	4));
			Digits	-=	2;
		}	else
		{
			Data[Size-1]	=	(byte_t)(Value	%	10);
			Digits--;
		}
		Value	/=	100;
		Size--;
	}
	return	RetVal;
}

std::string CISO8583Utilities::BinToHex(byte_t * Src, int Count)
{
	static	char	HexChars[]	=	{'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
	char	*S	=	new	char[Count	*	2	+1];
	for(int	I=0;I	<	Count;I++)
	{
		S[I	*	2]	=	HexChars[Src[I]	>>	4];
		S[I	*	2	+	1]	=	HexChars[Src[I]	&	0xf];
	}
	S[Count	*	2]	=	0;
	std::string	RetVal	=	S;
	return	RetVal;
}

#define	VALIDATE_AND_SET_BCD_VALUE(CHAR_VALUE,BCD_VALUE,WITH_HEX)	\
{\
	if((CHAR_VALUE)	>=	0x30	&&	(CHAR_VALUE)	<=	0x39)\
	{\
		(BCD_VALUE)	=	(CHAR_VALUE)	-	0x30;\
	}	else	if(WITH_HEX)\
	{\
		if((CHAR_VALUE)	>=	0x40	&&	(CHAR_VALUE)	<=	0x4f)\
		{\
			(BCD_VALUE)	=	(CHAR_VALUE)	-	0x30;\
		}	else	if((CHAR_VALUE)	>=	'A'	&&	(CHAR_VALUE)	<=	'F')\
		{\
			(BCD_VALUE)	=	(CHAR_VALUE)	-	'A'	+	10;\
		}	else	if((CHAR_VALUE)	>=	'a'	&&	(CHAR_VALUE)	<=	'f')\
		{\
			(BCD_VALUE)	=	(CHAR_VALUE)	-	'a'	+	10;\
		}	else\
		{\
			return	-1;\
		}\
	}	else\
	{\
		return	-1;\
	}\
}

int	CISO8583Utilities::ToBCD(byte_t	*Data,int	Digits,std::string	NumericString,bool	bWithHexDigits,bool	bLeftJust)
{
	int	DataLength	=	(int)NumericString.length();
	if(Digits	>	DataLength)
	{
		if(bLeftJust)
		{
			memset(Data	+	(DataLength	>>	1),0,((Digits	+	1)	>>	1)	-	(DataLength	>>	1));
		}	else
		{
			memset(Data,0,((Digits	+	1)	>>	1)	-	(DataLength	>>	1));
		}
	}

	const	char	*Ptr	=	NumericString.c_str();
	byte_t	Temp;
	if(bLeftJust)
	{
		for(int	I=0;I<Digits	&&	I<DataLength;I++)
		{
			VALIDATE_AND_SET_BCD_VALUE(Ptr[I],Temp,bWithHexDigits);
			if(I	&	1)
			{
				*Data	+=	Temp;
				Data++;
			}	else
			{
				*Data	=	Temp	<<	4;
			}
		}
	}	else
	{
		Data	+=	(((Digits	+	1)	>>	1)-1);
		bool	bRightNibble	=	true;
		for(int	I=DataLength;I>0	&&	I>DataLength-Digits;I--)
		{
			VALIDATE_AND_SET_BCD_VALUE(Ptr[I-1],Temp,bWithHexDigits);
			if(bRightNibble)
			{
				*Data	=	Temp;
				bRightNibble	=	false;
			}	else
			{
				*Data	+=	(Temp	<<	4);
				Data--;
				bRightNibble	=	true;
			}
		}
	}
	return	(Digits	+	1)	>>	1;
}
int64_t	CISO8583Utilities::ToInt64(std::string	Value)
{
	int64_t	Number	=	0;
	const	char	*Buf	=	Value.c_str();
	for(int	I=0;I<(int)Value.length();I++)
	{
		if(!isdigit(Buf[I]))
			return	0;
		Number	*=	10;
		Number	+=	Buf[I]	-	'0';
	}
	return	Number;
}
float	CISO8583Utilities::ToFloat(std::string	Value)
{
	float	F=0;
	sscanf(Value.c_str(),"%f",&F);
	return	F;
}

std::string CISO8583Utilities::ToString(int64_t Number, int Length)
{
	char	RetBuf[200];
	bool	bMinus	=	Number	<	0;
	if(Length	>=	sizeof(RetBuf))
		Throw();
	if(Length	<	0)
		return	"";
	int	LastPos	=	(int)sizeof(RetBuf);
	RetBuf[--LastPos]	=	0;
	if(bMinus)
		Number	*=	(-1);

	while(LastPos	>	0	&&	Number	>	0)
	{
		RetBuf[--LastPos]	=	(char)((Number%10)+48);
		Number	/=	10;
	}

	int	Size	=	(int)sizeof(RetBuf) - LastPos - 1;
	if(Length	>	0)
	{
		int	Diff	=	Length	-	Size;
		LastPos	-=	Diff;
		if(Diff	>	0)
		{
			memset(&RetBuf[LastPos],'0',Diff);
		}
	}

	return	RetBuf+LastPos;
}


std::string CISO8583Utilities::ToStringAmount(double Number)
{
	char	RetBuf[100];
	sprintf(RetBuf,"%.02f",Number);

	return	RetBuf;
}


int64_t	CISO8583Utilities::PowerOf10(unsigned	int	E)
{
	int64_t	RetVal	=	1;
	while(E	>	0)
	{
		RetVal	*=	10;
		E--;
	}
	return	RetVal;
}

bool	CISO8583Utilities::TLVDataHasTag(const	byte_t	*Data,int	DataLength,unsigned int	TagToFind)
{
	int	NextPos	=	0;
	unsigned	int	Tag;
	bool	bTagFound	=	false;

	while(NextPos	<	DataLength)
	{
		if(!bTagFound)
		{
			if(Data[NextPos]	==	0xff	||	Data[NextPos]	==	0)
			{
				NextPos++;
				continue;
			}
			Tag	=	Data[NextPos++];
			if((Tag	&	0x1f)	==	0x1f)
			{
				do
				{
					if(NextPos	>=	DataLength)
						return	false;
					Tag	<<=	8;
					Tag	|=	Data[NextPos];
				}	while (Data[NextPos++]	&	0x80);
				if(Tag	==	TagToFind)
					return	true;
			}
			bTagFound	=	true;
		}	else
		{
			int	Length	=	0;
			if(Data[NextPos]	&&	0x80)
			{
				int	Count	=	Data[NextPos]	&	0x7f;
				NextPos++;
				for(int	I=0;I	<	Count;I++)
				{
					Length	<<=	8;
					Length	+=	Data[NextPos++];
				}
			}	else
			{
				Length	=	Data[NextPos++];
			}
			NextPos	+=	Length;
			bTagFound	=	false;
		}
	}
	return	false;
}

std::string	CISO8583Utilities::TraceBuffer(const	char *SBuf,int	Size)
{
	std::string	RetVal	=	"\n";
	char	Line[200];
	int	LineChars	=	0;
	unsigned char	*Buf	=	(unsigned char	*)SBuf;
	memset(Line,32,sizeof(Line));
	for(int	I=0;I	<	Size;I++)
	{
		sprintf(&Line[LineChars*3],"%02X ",Buf[I]);
		if(Buf[I]	>	31)
			Line[50+LineChars]	=	Buf[I];
		else
			Line[50+LineChars]	=	'.';
		LineChars++;
		if(LineChars	==	16)
		{
			Line[LineChars*3]	=	' ';
			Line[50+LineChars]	=	0;
			strcat(Line,"\n");
			RetVal	+=	Line;
			memset(Line,32,sizeof(Line));
			LineChars	=	0;
		}
	}
	if(LineChars	>	0)
	{
		Line[LineChars*3]	=	' ';
		Line[50+LineChars]	=	0;
		strcat(Line,"\n");
		RetVal	+=	Line;
		memset(Line,32,sizeof(Line));
		LineChars	=	0;
	}
	return	RetVal;
}
