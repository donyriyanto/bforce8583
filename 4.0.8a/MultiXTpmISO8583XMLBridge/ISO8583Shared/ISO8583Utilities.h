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
#ifndef	ISO8583Utilities_H_Included
#define	ISO8583Utilities_H_Included
#ifdef	ShowInclude
#pragma message( "Include " __FILE__ )
#endif

class ISO8583SHARED_API	CISO8583Utilities
{
public:
	CISO8583Utilities(void);
	~CISO8583Utilities(void);
	static	int	HexToBin(byte_t	*Dest,std::string	HexSrc);
	static	std::string	BCDToString(const	byte_t	*Data,int	Size,bool	bWithHexDigits,bool	bLeftJust);
	static	int	ToBCD(byte_t	*Data,int	Digits,unsigned	long	Value);
	static	int	ToBCD(byte_t	*Data,int	Digits,std::string	NumericString,bool	bWithHexDigits,bool	bLeftJust);
	static std::string BinToHex(byte_t * Src, int Count);
	static	int64_t	ToInt64(std::string	Value);
	static	float	ToFloat(std::string	Value);
	static	std::string ToString(int64_t Number, int Length=0);
	static	std::string ToStringAmount(double Number);
	static	int64_t	PowerOf10(unsigned	int	E);
	static	bool	TLVDataHasTag(const	byte_t	*Data,int	DataLength,unsigned int	Tag);
	static	std::string	TraceBuffer(const	char *SBuf,int	Size);
};

#endif	//	ISO8583Utilities_H_Included
