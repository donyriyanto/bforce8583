/*!	\file	
 MultiXEchoServer.cpp : Defines the entry point for the console application.
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

#include "MultiXEchoServer.h"
#include	"MultiXEchoServerApp.h"

/*!
	the main function is used just to create the main CMultiXEchoServerApp object,
	start it and wait for it finish.
*/
int main(int Argc, char* Argv[])
{
			
	CMultiXEchoServerApp	App;
	/*
		A Server Application is usually started by MultiXTpm Application Server and required parameters are passed on the command line.
		First thing to do is to communicate with MultiXTpm and tell it that this process is willing to accept requests.
	*/
	if(!App.Accept(Argc,Argv,5000))
	{
		printf("Unable to connect to MultiXTpm Application Server !!!\n");
		return	-1;
	}
	/*
		in order to accept request you allocate a dynamic buffer to hold the new request.
	*/
	CMultiXBuffer	*pBuf	=	App.AllocateBuffer();
	int	MsgCode;
	/*
		Here you enter an endless loop of waiting for new requests, processing them and responding. this will happen untill the connection
		with MultiXTpm is dropped or, the timeout (of 600 seconds in this case) occurs.
		MultiXTpm might elect to shutdown this process, in this case the Receive() operation will return with an error.
		In any case, a false return from Receive() implies that the process should terminate.
	*/
	while(App.Receive(MsgCode,*pBuf,600000))
	{
		printf("Message Received from MultiXTpm Application Server:\nMsg Code=%d\nMsg Data=%s\n",MsgCode,pBuf->DataPtr());
		char	*PrintBuf	=	new	char[1000 + pBuf->Length()];
		sprintf(PrintBuf,"Your Request (MsgCode=%d) has been processed by MultiXEchoServer (MultiX Process ID=%d)\nRequest Data=%s",MsgCode,App.MyProcessID(),pBuf->DataPtr());
		App.Reply(PrintBuf,0);
		delete	[]	PrintBuf;
	}
	pBuf->ReturnBuffer();
				return	0;
}
