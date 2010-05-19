namespace gSoapCalculatorClient
{
	partial class Calculator
	{
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.IContainer components = null;

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		/// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
		protected override void Dispose(bool disposing)
		{
			if (disposing && (components != null))
			{
				components.Dispose();
			}
			base.Dispose(disposing);
		}

		#region Windows Form Designer generated code

		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.label1 = new System.Windows.Forms.Label();
			this.label2 = new System.Windows.Forms.Label();
			this.FirstNumber = new System.Windows.Forms.MaskedTextBox();
			this.SecondNUmber = new System.Windows.Forms.MaskedTextBox();
			this.label3 = new System.Windows.Forms.Label();
			this.CalcResult = new System.Windows.Forms.TextBox();
			this.AddBtn = new System.Windows.Forms.Button();
			this.SubBtn = new System.Windows.Forms.Button();
			this.MulBtn = new System.Windows.Forms.Button();
			this.DivBtn = new System.Windows.Forms.Button();
			this.ExitBtn = new System.Windows.Forms.Button();
			this.label4 = new System.Windows.Forms.Label();
			this.WSUrl = new System.Windows.Forms.TextBox();
			this.SuspendLayout();
			// 
			// label1
			// 
			this.label1.AutoSize = true;
			this.label1.Location = new System.Drawing.Point(44, 69);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(66, 13);
			this.label1.TabIndex = 0;
			this.label1.Text = "First Number";
			// 
			// label2
			// 
			this.label2.AutoSize = true;
			this.label2.Location = new System.Drawing.Point(47, 111);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(84, 13);
			this.label2.TabIndex = 1;
			this.label2.Text = "Second Number";
			// 
			// FirstNumber
			// 
			this.FirstNumber.AllowPromptAsInput = false;
			this.FirstNumber.BeepOnError = true;
			this.FirstNumber.Culture = new System.Globalization.CultureInfo("en-US");
			this.FirstNumber.Location = new System.Drawing.Point(167, 69);
			this.FirstNumber.Mask = "00000";
			this.FirstNumber.Name = "FirstNumber";
			this.FirstNumber.Size = new System.Drawing.Size(100, 20);
			this.FirstNumber.TabIndex = 1;
			this.FirstNumber.ValidatingType = typeof(int);
			this.FirstNumber.TypeValidationCompleted += new System.Windows.Forms.TypeValidationEventHandler(this.FirstNumber_TypeValidationCompleted);
			// 
			// SecondNUmber
			// 
			this.SecondNUmber.AllowPromptAsInput = false;
			this.SecondNUmber.BeepOnError = true;
			this.SecondNUmber.Culture = new System.Globalization.CultureInfo("en-US");
			this.SecondNUmber.Location = new System.Drawing.Point(167, 111);
			this.SecondNUmber.Mask = "00000";
			this.SecondNUmber.Name = "SecondNUmber";
			this.SecondNUmber.Size = new System.Drawing.Size(100, 20);
			this.SecondNUmber.TabIndex = 2;
			this.SecondNUmber.ValidatingType = typeof(int);
			this.SecondNUmber.TypeValidationCompleted += new System.Windows.Forms.TypeValidationEventHandler(this.SecondNUmber_TypeValidationCompleted);
			// 
			// label3
			// 
			this.label3.AutoSize = true;
			this.label3.Location = new System.Drawing.Point(50, 155);
			this.label3.Name = "label3";
			this.label3.Size = new System.Drawing.Size(37, 13);
			this.label3.TabIndex = 4;
			this.label3.Text = "Result";
			// 
			// CalcResult
			// 
			this.CalcResult.Location = new System.Drawing.Point(167, 147);
			this.CalcResult.Name = "CalcResult";
			this.CalcResult.ReadOnly = true;
			this.CalcResult.Size = new System.Drawing.Size(100, 20);
			this.CalcResult.TabIndex = 5;
			this.CalcResult.TabStop = false;
			// 
			// AddBtn
			// 
			this.AddBtn.Location = new System.Drawing.Point(43, 205);
			this.AddBtn.Name = "AddBtn";
			this.AddBtn.Size = new System.Drawing.Size(75, 23);
			this.AddBtn.TabIndex = 3;
			this.AddBtn.Text = "Add";
			this.AddBtn.UseVisualStyleBackColor = true;
			this.AddBtn.Click += new System.EventHandler(this.AddBtn_Click);
			// 
			// SubBtn
			// 
			this.SubBtn.Location = new System.Drawing.Point(165, 204);
			this.SubBtn.Name = "SubBtn";
			this.SubBtn.Size = new System.Drawing.Size(75, 23);
			this.SubBtn.TabIndex = 4;
			this.SubBtn.Text = "Subtract";
			this.SubBtn.UseVisualStyleBackColor = true;
			this.SubBtn.Click += new System.EventHandler(this.SubBtn_Click);
			// 
			// MulBtn
			// 
			this.MulBtn.Location = new System.Drawing.Point(287, 205);
			this.MulBtn.Name = "MulBtn";
			this.MulBtn.Size = new System.Drawing.Size(75, 23);
			this.MulBtn.TabIndex = 5;
			this.MulBtn.Text = "Multiply";
			this.MulBtn.UseVisualStyleBackColor = true;
			this.MulBtn.Click += new System.EventHandler(this.MulBtn_Click);
			// 
			// DivBtn
			// 
			this.DivBtn.Location = new System.Drawing.Point(409, 205);
			this.DivBtn.Name = "DivBtn";
			this.DivBtn.Size = new System.Drawing.Size(75, 23);
			this.DivBtn.TabIndex = 6;
			this.DivBtn.Text = "Divide";
			this.DivBtn.UseVisualStyleBackColor = true;
			this.DivBtn.Click += new System.EventHandler(this.DivBtn_Click);
			// 
			// ExitBtn
			// 
			this.ExitBtn.Location = new System.Drawing.Point(221, 258);
			this.ExitBtn.Name = "ExitBtn";
			this.ExitBtn.Size = new System.Drawing.Size(75, 23);
			this.ExitBtn.TabIndex = 7;
			this.ExitBtn.Text = "Exit";
			this.ExitBtn.UseVisualStyleBackColor = true;
			this.ExitBtn.Click += new System.EventHandler(this.ExitBtn_Click);
			// 
			// label4
			// 
			this.label4.AutoSize = true;
			this.label4.Location = new System.Drawing.Point(44, 26);
			this.label4.Name = "label4";
			this.label4.Size = new System.Drawing.Size(94, 13);
			this.label4.TabIndex = 8;
			this.label4.Text = "Web Service URL";
			// 
			// WSUrl
			// 
			this.WSUrl.Location = new System.Drawing.Point(167, 26);
			this.WSUrl.Name = "WSUrl";
			this.WSUrl.Size = new System.Drawing.Size(317, 20);
			this.WSUrl.TabIndex = 9;
			this.WSUrl.Text = "http://localhost:18999/Calculator";
			// 
			// Calculator
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(516, 311);
			this.Controls.Add(this.WSUrl);
			this.Controls.Add(this.label4);
			this.Controls.Add(this.ExitBtn);
			this.Controls.Add(this.DivBtn);
			this.Controls.Add(this.MulBtn);
			this.Controls.Add(this.SubBtn);
			this.Controls.Add(this.AddBtn);
			this.Controls.Add(this.CalcResult);
			this.Controls.Add(this.label3);
			this.Controls.Add(this.SecondNUmber);
			this.Controls.Add(this.FirstNumber);
			this.Controls.Add(this.label2);
			this.Controls.Add(this.label1);
			this.Name = "Calculator";
			this.Text = "Calculator";
			this.Load += new System.EventHandler(this.Calculator_Load);
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion

		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.Label label2;
		private System.Windows.Forms.MaskedTextBox FirstNumber;
		private System.Windows.Forms.MaskedTextBox SecondNUmber;
		private System.Windows.Forms.Label label3;
		private System.Windows.Forms.TextBox CalcResult;
		private System.Windows.Forms.Button AddBtn;
		private System.Windows.Forms.Button SubBtn;
		private System.Windows.Forms.Button MulBtn;
		private System.Windows.Forms.Button DivBtn;
		private System.Windows.Forms.Button ExitBtn;
		private System.Windows.Forms.Label label4;
		private System.Windows.Forms.TextBox WSUrl;
	}
}

