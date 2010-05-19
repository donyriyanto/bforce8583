/*!	\file	
 MultiXEchoClient.cpp : Defines the entry point for the console application.
*/
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

#include "MultiXEchoClient.h"
#include	"MultiXEchoClientApp.h"

/*!
	the main function is used just to create the main CMultiXEchoClientApp object,
	start it and wait for it finish.
*/
int main(int Argc, char* Argv[])
{
	
	CMultiXEchoClientApp	App;
	if(!App.Connect(/* Set here the unique MultiX Process ID */ 100100,
									/* Host Address of the MultiXTpm Application Server */	"localhost",
									/* TCP/IP port of the MultiXTpm Application Server*/	"12345" ,
									/* specify here the timeout in millisecond to wait for the connection establishment */ 5000))
	{
		printf("Unable to connect to MultiXTpm Application Server !!!\n");
		return	-1;
	}
	/*
		Once we have the server connected, we can allocate buffers from a pool owned by the "App" object. We allocate a dynamic buffer
		to fill in our request to send to the server.
	*/
	CMultiXBuffer	*pBuf	=	App.AllocateBuffer();
	
	for(int	I=0;I<Argc;I++)
	{
		pBuf->Empty();
		pBuf->AppendString(Argv[I],true);
		/*
		Send your request here. The request is sent in nowait manner, this means that a you have to call WaitReply() to verify the outcome
		or to receive the response. till you call WaitReply() you can preform other tasks.
		*/
		
		printf("Sending request to  MultiXTpm Application Server:\n%s\n",pBuf->DataPtr());
		if(!App.Send(1000 /* Use a well known Message Code that is supported by the server */,*pBuf))
		{
			pBuf->ReturnBuffer();	//	you must return buffers you allocated
			printf("Failed to send message to MultiXTpm Application Server !!!\n");
			return	-1;
		}

		int	MsgCode;
		int	Error;
		
		/*
			Here you wait for the response from the server. MultiX API sets default timeout on each send, so you do not have to set timeout. if
			there is an error or some Server configured timeout expires, the function will return with an error.
			On a successful send, the buffer will contain the response from the server. if the server responded with no data, the buffer length
			will be set to zero.
		*/
		if(!App.WaitReply(MsgCode,*pBuf,Error))
		{
			pBuf->ReturnBuffer();
			printf("Failed waiting for a reply from MultiXTpm Application Server !!!\n");
			return	-1;
		}
		printf("Reply Received from MultiXTpm Application Server:\n%s\n",pBuf->DataPtr());
	}
		return	0;
}
