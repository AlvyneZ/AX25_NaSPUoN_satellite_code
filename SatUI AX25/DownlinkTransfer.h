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
	/// Summary for DownlinkTransfer
	/// </summary>
	public ref class DownlinkTransfer : public System::Windows::Forms::Form
	{
	public:
		DownlinkTransfer(void)
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
		~DownlinkTransfer()
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
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(DownlinkTransfer::typeid));
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
			this->label_transferID->Location = System::Drawing::Point(16, 11);
			this->label_transferID->Margin = System::Windows::Forms::Padding(4, 0, 4, 0);
			this->label_transferID->Name = L"label_transferID";
			this->label_transferID->Size = System::Drawing::Size(79, 17);
			this->label_transferID->TabIndex = 0;
			this->label_transferID->Text = L"TransferID:";
			// 
			// textBox_transferID
			// 
			this->textBox_transferID->Location = System::Drawing::Point(104, 7);
			this->textBox_transferID->Margin = System::Windows::Forms::Padding(4, 4, 4, 4);
			this->textBox_transferID->Name = L"textBox_transferID";
			this->textBox_transferID->ReadOnly = true;
			this->textBox_transferID->Size = System::Drawing::Size(647, 22);
			this->textBox_transferID->TabIndex = 1;
			this->textBox_transferID->Text = L"\?\?";
			// 
			// label_filename
			// 
			this->label_filename->AutoSize = true;
			this->label_filename->Location = System::Drawing::Point(16, 48);
			this->label_filename->Margin = System::Windows::Forms::Padding(4, 0, 4, 0);
			this->label_filename->Name = L"label_filename";
			this->label_filename->Size = System::Drawing::Size(69, 17);
			this->label_filename->TabIndex = 2;
			this->label_filename->Text = L"Filename:";
			// 
			// textBox_filename
			// 
			this->textBox_filename->Location = System::Drawing::Point(104, 44);
			this->textBox_filename->Margin = System::Windows::Forms::Padding(4, 4, 4, 4);
			this->textBox_filename->Name = L"textBox_filename";
			this->textBox_filename->ReadOnly = true;
			this->textBox_filename->Size = System::Drawing::Size(647, 22);
			this->textBox_filename->TabIndex = 3;
			// 
			// progressBar_downlink
			// 
			this->progressBar_downlink->Location = System::Drawing::Point(20, 87);
			this->progressBar_downlink->Margin = System::Windows::Forms::Padding(4, 4, 4, 4);
			this->progressBar_downlink->Name = L"progressBar_downlink";
			this->progressBar_downlink->Size = System::Drawing::Size(732, 28);
			this->progressBar_downlink->TabIndex = 4;
			// 
			// button_downlinkCancel
			// 
			this->button_downlinkCancel->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(255)),
				static_cast<System::Int32>(static_cast<System::Byte>(128)), static_cast<System::Int32>(static_cast<System::Byte>(128)));
			this->button_downlinkCancel->Enabled = false;
			this->button_downlinkCancel->Location = System::Drawing::Point(317, 128);
			this->button_downlinkCancel->Margin = System::Windows::Forms::Padding(4, 4, 4, 4);
			this->button_downlinkCancel->Name = L"button_downlinkCancel";
			this->button_downlinkCancel->Size = System::Drawing::Size(100, 28);
			this->button_downlinkCancel->TabIndex = 24;
			this->button_downlinkCancel->Text = L"Cancel";
			this->button_downlinkCancel->UseVisualStyleBackColor = false;
			this->button_downlinkCancel->Visible = false;
			this->button_downlinkCancel->Click += gcnew System::EventHandler(this, &DownlinkTransfer::button_downlinkCancel_Click);
			// 
			// DownlinkTransfer
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(8, 16);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(768, 171);
			this->ControlBox = false;
			this->Controls->Add(this->button_downlinkCancel);
			this->Controls->Add(this->progressBar_downlink);
			this->Controls->Add(this->textBox_filename);
			this->Controls->Add(this->label_filename);
			this->Controls->Add(this->textBox_transferID);
			this->Controls->Add(this->label_transferID);
			this->Icon = (cli::safe_cast<System::Drawing::Icon^>(resources->GetObject(L"$this.Icon")));
			this->Margin = System::Windows::Forms::Padding(4, 4, 4, 4);
			this->Name = L"DownlinkTransfer";
			this->Text = L"DownlinkTransfer";
			this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &DownlinkTransfer::DownlinkTransfer_FormClosing);
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
		private: System::Void button_downlinkCancel_Click(System::Object^  sender, System::EventArgs^  e) {
			this->Close();
		}

		private: System::Void DownlinkTransfer_FormClosing(System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e);
};
}
