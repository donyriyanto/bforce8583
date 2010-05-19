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

// gSoapEchoClient.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include	"../gSoapEchoServer/EchoServerH.h"
#include	"../gSoapEchoServer/EchoServer.nsmap"

int main(int argc, char* argv[])
{
	if(argc	<	4)
	{
		printf("Usage: %s <url> <toupper|tolower|echo|echoSession|echoApplication> <string> [Repeat Count]\n",argv[0]);
		return	1;
	}
	int	RetVal;
	int	Count	=	1;
	if(argc > 4)
		if(atoi(argv[4]) > 0)
			Count	=	atoi(argv[4]);

	soap	soap;
	soap_init2(&soap, SOAP_IO_KEEPALIVE, SOAP_IO_KEEPALIVE); 
	soap_set_namespaces(&soap,EchoServer_namespaces);

	time_t	Start	=	time(NULL);
	for(int	I=	0;I	<	Count;	I++)
	{
		std::string	Result;
		if(strcmp(argv[2],"toupper")	==	0)
			RetVal	=	soap_call_EchoServer__ToUpper(&soap,argv[1],NULL,argv[3],Result);
		else
		if(strcmp(argv[2],"tolower")	==	0)
			RetVal	=	soap_call_EchoServer__ToLower(&soap,argv[1],NULL,argv[3],Result);
		else
		if(strcmp(argv[2],"echo")	==	0)
			RetVal	=	soap_call_EchoServer__Echo(&soap,argv[1],NULL,argv[3],Result);
		else
		if(strcmp(argv[2],"echoSession")	==	0)
			RetVal	=	soap_call_EchoServer__EchoWithSessionData(&soap,argv[1],NULL,argv[3],Result);
		else
		if(strcmp(argv[2],"echoApplication")	==	0)
			RetVal	=	soap_call_EchoServer__EchoWithApplicationData(&soap,argv[1],NULL,argv[3],Result);
		else
		{
		printf("Usage: %s <url> <toupper|tolower|echo|echoSession|echoApplication> <string> [Repeat Count]\n",argv[0]);
			return	1;
		}
		if(RetVal	!=	SOAP_OK)
			printf("Error %d while calling Web Service\n",RetVal);
		else
			printf("Echo Server returned : \"%s\"\n",Result.c_str());
	}
	printf("Total of %d queries - took %d seconds\n",Count,time(NULL)-Start);
	return 0;
}

#include	<gsoap/stdsoap2.cpp>

