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

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Security.Cryptography;
using System.Security.Cryptography.X509Certificates;
using System.Net.Security;
using System.Net;

namespace gSoapCalculatorClient
{
	public partial class Calculator : Form
	{
		public Calculator()
		{
			InitializeComponent();
		}

		private void AddBtn_Click(object sender, EventArgs e)
		{
			CalculatorWS.Service WS = new CalculatorWS.Service();
			WS.Url = WSUrl.Text;
			CalcResult.Text	=	WS.Add(Convert.ToDouble(this.FirstNumber.Text),Convert.ToDouble(SecondNUmber.Text)).ToString();
		}

		private void SubBtn_Click(object sender, EventArgs e)
		{
			CalculatorWS.Service WS = new CalculatorWS.Service();
			WS.Url = WSUrl.Text;
			CalcResult.Text = WS.Sub(Convert.ToDouble(this.FirstNumber.Text), Convert.ToDouble(SecondNUmber.Text)).ToString();
		}

		private void MulBtn_Click(object sender, EventArgs e)
		{
			CalculatorWS.Service WS = new CalculatorWS.Service();
			WS.Url = WSUrl.Text;
			CalcResult.Text = WS.Mul(Convert.ToDouble(this.FirstNumber.Text), Convert.ToDouble(SecondNUmber.Text)).ToString();
		}

		private void DivBtn_Click(object sender, EventArgs e)
		{
			CalculatorWS.Service WS = new CalculatorWS.Service();
			WS.Url = WSUrl.Text;
			CalcResult.Text = WS.Div(Convert.ToDouble(this.FirstNumber.Text), Convert.ToDouble(SecondNUmber.Text)).ToString();
		}

		private void ExitBtn_Click(object sender, EventArgs e)
		{
			Application.Exit();
		}

		private void Calculator_Load(object sender, EventArgs e)
		{
			FirstNumber.Mask = "#####";
			SecondNUmber.Mask = "#####";
			FirstNumber.ValidatingType = typeof(int);
			SecondNUmber.ValidatingType = typeof(int);
			FirstNumber.Text = "0";
			SecondNUmber.Text = "0";

		}

		private void FirstNumber_TypeValidationCompleted(object sender, TypeValidationEventArgs e)
		{
			if (!e.IsValidInput)
				e.Cancel = true;
		}

		private void SecondNUmber_TypeValidationCompleted(object sender, TypeValidationEventArgs e)
		{
			if (!e.IsValidInput)
				e.Cancel = true;
		}

	}
}