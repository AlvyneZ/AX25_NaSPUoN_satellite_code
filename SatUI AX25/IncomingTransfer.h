#pragma once
#include <vector>

namespace SatUI {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Summary for IncomingTransfer
	/// </summary>
	public ref class IncomingTransfer : public System::Windows::Forms::Form
	{
	public:
		IncomingTransfer(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~IncomingTransfer()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Label^  label_transferID;
	public: System::Windows::Forms::TextBox^  textBox_transferID;
	private: System::Windows::Forms::Label^  label_filename;
	public: System::Windows::Forms::TextBox^  textBox_filename;
	public: System::Windows::Forms::ProgressBar^  progressBar_downlink;
	public: uint16_t transferID;
	public: MyForm^ form;

	public: System::Windows::Forms::Button^  button_downlinkCancel;


	protected:


	protected:

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->label_transferID = (gcnew System::Windows::Forms::Label());
			this->textBox_transferID = (gcnew System::Windows::Forms::TextBox());
			this->label_filename = (gcnew System::Windows::Forms::Label());
			this->textBox_filename = (gcnew System::Windows::Forms::TextBox());
			this->progressBar_downlink = (gcnew System::Windows::Forms::ProgressBar());
			this->button_downlinkCancel = (gcnew System::Windows::Forms::Button());
			this->SuspendLayout();
			// 
			// label_transferID
			// 
			this->label_transferID->AutoSize = true;
			this->label_transferID->Location = System::Drawing::Point(12, 9);
			this->label_transferID->Name = L"label_transferID";
			this->label_transferID->Size = System::Drawing::Size(60, 13);
			this->label_transferID->TabIndex = 0;
			this->label_transferID->Text = L"TransferID:";
			// 
			// textBox_transferID
			// 
			this->textBox_transferID->Location = System::Drawing::Point(78, 6);
			this->textBox_transferID->Name = L"textBox_transferID";
			this->textBox_transferID->ReadOnly = true;
			this->textBox_transferID->Size = System::Drawing::Size(486, 20);
			this->textBox_transferID->TabIndex = 1;
			this->textBox_transferID->Text = L"\?\?";
			// 
			// label_filename
			// 
			this->label_filename->AutoSize = true;
			this->label_filename->Location = System::Drawing::Point(12, 39);
			this->label_filename->Name = L"label_filename";
			this->label_filename->Size = System::Drawing::Size(52, 13);
			this->label_filename->TabIndex = 2;
			this->label_filename->Text = L"Filename:";
			// 
			// textBox_filename
			// 
			this->textBox_filename->Location = System::Drawing::Point(78, 36);
			this->textBox_filename->Name = L"textBox_filename";
			this->textBox_filename->ReadOnly = true;
			this->textBox_filename->Size = System::Drawing::Size(486, 20);
			this->textBox_filename->TabIndex = 3;
			// 
			// progressBar_downlink
			// 
			this->progressBar_downlink->Location = System::Drawing::Point(15, 71);
			this->progressBar_downlink->Name = L"progressBar_downlink";
			this->progressBar_downlink->Size = System::Drawing::Size(549, 23);
			this->progressBar_downlink->TabIndex = 4;
			// 
			// button_downlinkCancel
			// 
			this->button_downlinkCancel->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(255)),
				static_cast<System::Int32>(static_cast<System::Byte>(128)), static_cast<System::Int32>(static_cast<System::Byte>(128)));
			this->button_downlinkCancel->Location = System::Drawing::Point(238, 104);
			this->button_downlinkCancel->Name = L"button_downlinkCancel";
			this->button_downlinkCancel->Size = System::Drawing::Size(75, 23);
			this->button_downlinkCancel->TabIndex = 24;
			this->button_downlinkCancel->Text = L"Cancel";
			this->button_downlinkCancel->UseVisualStyleBackColor = false;
			this->button_downlinkCancel->Click += gcnew System::EventHandler(this, &IncomingTransfer::button_downlinkCancel_Click);
			// 
			// IncomingTransfer
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(576, 139);
			this->Controls->Add(this->button_downlinkCancel);
			this->Controls->Add(this->progressBar_downlink);
			this->Controls->Add(this->textBox_filename);
			this->Controls->Add(this->label_filename);
			this->Controls->Add(this->textBox_transferID);
			this->Controls->Add(this->label_transferID);
			this->Name = L"IncomingTransfer";
			this->Text = L"IncomingTransfer";
			this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &IncomingTransfer::IncomingTransfer_FormClosing);
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
		private: System::Void button_downlinkCancel_Click(System::Object^  sender, System::EventArgs^  e) {
			this->Close();
		}

		private: System::Void IncomingTransfer_FormClosing(System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e);
};
}
