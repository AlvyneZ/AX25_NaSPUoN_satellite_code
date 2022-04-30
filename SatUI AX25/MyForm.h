#pragma once

#include <string>
#include <vector>
#include <msclr\marshal_cppstd.h>
#include <msclr/lock.h>
#include <cliext/map>

#include "fileTransferClass.h"
#include "MainFormForwardDeclaration.h"
#include "AboutDialog.h"
#include "DownlinkTransfer.h"


namespace SatUI {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::IO::Ports;

	/// <summary>
	/// Summary for MyForm
	/// </summary>
	public ref class MyForm : public System::Windows::Forms::Form
	{
	public:
		MyForm(void)
		{
			InitializeComponent();

			findPorts();
			//
			//TODO: Add the constructor code here
			this->TNCPort = 0x0;
			this->satelliteSSID = 0x2;
			this->satelliteCallsign = L"N0CAL";
			this->groundSSID = 0x1;
			this->groundCallsign = L"N0CAL";
			this->downlinkSavelocation = L"%USERPROFILE%\\Desktop\\downlinks";
			this->frameIDCountXB = 0x01;
			this->transferIDCount = 0x0000;
			this->frameIDMutex = gcnew System::Object();
			this->transferIDMutex = gcnew System::Object();
			//this->uplinkSendNextMutex = gcnew System::Object();
			this->kissOutMutex = gcnew System::Object();
			this->HPAX25Mutex = gcnew System::Object();
			this->UIThreadID = System::Threading::Thread::CurrentThread->ManagedThreadId;
			this->transferForms = gcnew cliext::map<uint16_t, DownlinkTransfer^>();
			//
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~MyForm()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::IO::Ports::SerialPort^  serialPort1;
	protected:
	private: System::Windows::Forms::ComboBox^  comboBox_COMPort;
	private: System::Windows::Forms::Label^  label_COMPort;
	private: System::Windows::Forms::ComboBox^  comboBox_Baud;
	private: System::Windows::Forms::Label^  label_baudRate;

	private: System::Windows::Forms::Button^  button_initPort;

	private: System::Windows::Forms::Button^  button_closePort;
	public: System::Windows::Forms::RichTextBox^  richTextBox_output;
	private: System::Windows::Forms::Button^  button_imagesTaken;
	private: System::Windows::Forms::Button^  button_LoRaPackets;
	private: System::Windows::Forms::Button^  button_saveLogFile;
	private: System::Windows::Forms::Button^  button_oldLogFiles;
	private: System::Windows::Forms::Button^  button_houseKeepingData;
	private: System::Windows::Forms::Button^  button_downlinkRequest;
	private: System::Windows::Forms::ComboBox^  comboBox_downlinkFileName;
	private: System::Windows::Forms::Label^  label_downlinkFileName;
	private: System::Windows::Forms::Label^  label_uplinkFileName;


	private: System::Windows::Forms::Button^  button_uplinkRequest;
	private: System::Windows::Forms::TextBox^  textBox_uplinkFileName;
	private: System::Windows::Forms::GroupBox^  groupBox_Initialization;
	private: System::Windows::Forms::ProgressBar^  progressBar_PortOpenSatus;
	private: System::ComponentModel::BackgroundWorker^  backgroundWorker_Receiver;
	private: System::Windows::Forms::ProgressBar^  progressBar_uplink;
	private: System::ComponentModel::BackgroundWorker^  backgroundWorker_Uplink;

	private: System::ComponentModel::BackgroundWorker^  backgroundWorker_DownlinkPartRequest;
	private: System::Windows::Forms::TextBox^  textBox_AX25SatCallsign;

	private: System::Windows::Forms::Label^  label_AX25Callsign;


	private: System::Windows::Forms::Label^  label_AX25SSID;


	private: System::Windows::Forms::TextBox^  textBox_AX25SatSSID;



	private: System::Windows::Forms::TextBox^  textBox_DownlinkSaveLocation;
	private: System::Windows::Forms::Label^  labelDownlinkSaveLocation;
	private: System::Windows::Forms::Button^  button_saveNewConfigFile;
	private: System::Windows::Forms::Button^  button_uplinkCancel;




	private: System::ComponentModel::IContainer^  components;

	public:
			uint8_t TNCPort;
			uint8_t satelliteSSID;
			System::String^ satelliteCallsign;
			uint8_t groundSSID;
			System::String^ groundCallsign;
			cliext::map<uint16_t, DownlinkTransfer^>^ transferForms;
	private:
		/// <summary>
		/// Required designer variable.
		uint8_t frameIDCountXB;
		uint16_t transferIDCount;
		System::Object^ frameIDMutex;
		System::Object^ transferIDMutex;
		//System::Object^ uplinkSendNextMutex;
		System::Object^ kissOutMutex;
		System::Object^ HPAX25Mutex;
		uint16_t currentDownlinkTransferID, currentUplinkTransferID;
		System::String^ downlinkSavelocation;
		int UIThreadID, receiverThreadID, UplinkThreadID, DownlinkPtRqThreadID;
		AboutDialog^ about;
	private: System::Windows::Forms::Button^  button_about;
private: System::Windows::Forms::ComboBox^  comboBox_TNCPort;
private: System::Windows::Forms::Label^  label_TNCPort;
private: System::Windows::Forms::TextBox^  textBox_AX25GSSSID;
private: System::Windows::Forms::TextBox^  textBox_AX25GSCallsign;



private: System::Windows::Forms::Label^  label_AX25Sat;
private: System::Windows::Forms::Label^  label_AX25GS;



	private: System::ComponentModel::BackgroundWorker^  backgroundWorker_Resender;

			 /// </summary>


#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(MyForm::typeid));
			this->serialPort1 = (gcnew System::IO::Ports::SerialPort(this->components));
			this->comboBox_COMPort = (gcnew System::Windows::Forms::ComboBox());
			this->label_COMPort = (gcnew System::Windows::Forms::Label());
			this->comboBox_Baud = (gcnew System::Windows::Forms::ComboBox());
			this->label_baudRate = (gcnew System::Windows::Forms::Label());
			this->button_initPort = (gcnew System::Windows::Forms::Button());
			this->button_closePort = (gcnew System::Windows::Forms::Button());
			this->richTextBox_output = (gcnew System::Windows::Forms::RichTextBox());
			this->button_imagesTaken = (gcnew System::Windows::Forms::Button());
			this->button_LoRaPackets = (gcnew System::Windows::Forms::Button());
			this->button_saveLogFile = (gcnew System::Windows::Forms::Button());
			this->button_oldLogFiles = (gcnew System::Windows::Forms::Button());
			this->button_houseKeepingData = (gcnew System::Windows::Forms::Button());
			this->button_downlinkRequest = (gcnew System::Windows::Forms::Button());
			this->comboBox_downlinkFileName = (gcnew System::Windows::Forms::ComboBox());
			this->label_downlinkFileName = (gcnew System::Windows::Forms::Label());
			this->label_uplinkFileName = (gcnew System::Windows::Forms::Label());
			this->button_uplinkRequest = (gcnew System::Windows::Forms::Button());
			this->textBox_uplinkFileName = (gcnew System::Windows::Forms::TextBox());
			this->groupBox_Initialization = (gcnew System::Windows::Forms::GroupBox());
			this->comboBox_TNCPort = (gcnew System::Windows::Forms::ComboBox());
			this->label_TNCPort = (gcnew System::Windows::Forms::Label());
			this->progressBar_PortOpenSatus = (gcnew System::Windows::Forms::ProgressBar());
			this->backgroundWorker_Receiver = (gcnew System::ComponentModel::BackgroundWorker());
			this->progressBar_uplink = (gcnew System::Windows::Forms::ProgressBar());
			this->backgroundWorker_Uplink = (gcnew System::ComponentModel::BackgroundWorker());
			this->backgroundWorker_DownlinkPartRequest = (gcnew System::ComponentModel::BackgroundWorker());
			this->textBox_AX25SatCallsign = (gcnew System::Windows::Forms::TextBox());
			this->label_AX25Callsign = (gcnew System::Windows::Forms::Label());
			this->button_uplinkCancel = (gcnew System::Windows::Forms::Button());
			this->label_AX25SSID = (gcnew System::Windows::Forms::Label());
			this->textBox_AX25SatSSID = (gcnew System::Windows::Forms::TextBox());
			this->textBox_DownlinkSaveLocation = (gcnew System::Windows::Forms::TextBox());
			this->labelDownlinkSaveLocation = (gcnew System::Windows::Forms::Label());
			this->button_saveNewConfigFile = (gcnew System::Windows::Forms::Button());
			this->backgroundWorker_Resender = (gcnew System::ComponentModel::BackgroundWorker());
			this->button_about = (gcnew System::Windows::Forms::Button());
			this->textBox_AX25GSSSID = (gcnew System::Windows::Forms::TextBox());
			this->textBox_AX25GSCallsign = (gcnew System::Windows::Forms::TextBox());
			this->label_AX25Sat = (gcnew System::Windows::Forms::Label());
			this->label_AX25GS = (gcnew System::Windows::Forms::Label());
			this->groupBox_Initialization->SuspendLayout();
			this->SuspendLayout();
			// 
			// comboBox_COMPort
			// 
			this->comboBox_COMPort->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->comboBox_COMPort->FormattingEnabled = true;
			this->comboBox_COMPort->Location = System::Drawing::Point(76, 22);
			this->comboBox_COMPort->Name = L"comboBox_COMPort";
			this->comboBox_COMPort->Size = System::Drawing::Size(121, 21);
			this->comboBox_COMPort->TabIndex = 0;
			// 
			// label_COMPort
			// 
			this->label_COMPort->AutoSize = true;
			this->label_COMPort->Location = System::Drawing::Point(14, 25);
			this->label_COMPort->Name = L"label_COMPort";
			this->label_COMPort->Size = System::Drawing::Size(56, 13);
			this->label_COMPort->TabIndex = 1;
			this->label_COMPort->Text = L"COM Port:";
			this->label_COMPort->DoubleClick += gcnew System::EventHandler(this, &MyForm::label_COMPort_DoubleClick);
			// 
			// comboBox_Baud
			// 
			this->comboBox_Baud->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->comboBox_Baud->FormattingEnabled = true;
			this->comboBox_Baud->Items->AddRange(gcnew cli::array< System::Object^  >(9) {
				L"1200", L"2400", L"4800", L"9600", L"19200",
					L"38400", L"57600", L"115200", L"230400"
			});
			this->comboBox_Baud->Location = System::Drawing::Point(76, 49);
			this->comboBox_Baud->Name = L"comboBox_Baud";
			this->comboBox_Baud->Size = System::Drawing::Size(121, 21);
			this->comboBox_Baud->TabIndex = 2;
			// 
			// label_baudRate
			// 
			this->label_baudRate->AutoSize = true;
			this->label_baudRate->Location = System::Drawing::Point(9, 52);
			this->label_baudRate->Name = L"label_baudRate";
			this->label_baudRate->Size = System::Drawing::Size(61, 13);
			this->label_baudRate->TabIndex = 3;
			this->label_baudRate->Text = L"Baud Rate:";
			// 
			// button_initPort
			// 
			this->button_initPort->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->button_initPort->Location = System::Drawing::Point(204, 32);
			this->button_initPort->Name = L"button_initPort";
			this->button_initPort->Size = System::Drawing::Size(116, 52);
			this->button_initPort->TabIndex = 4;
			this->button_initPort->Text = L"Init Port";
			this->button_initPort->UseVisualStyleBackColor = true;
			this->button_initPort->Click += gcnew System::EventHandler(this, &MyForm::button_initPort_Click);
			// 
			// button_closePort
			// 
			this->button_closePort->Enabled = false;
			this->button_closePort->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular,
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->button_closePort->Location = System::Drawing::Point(363, 34);
			this->button_closePort->Name = L"button_closePort";
			this->button_closePort->Size = System::Drawing::Size(113, 49);
			this->button_closePort->TabIndex = 5;
			this->button_closePort->Text = L"Close Port";
			this->button_closePort->UseVisualStyleBackColor = true;
			this->button_closePort->Click += gcnew System::EventHandler(this, &MyForm::button_closePort_Click);
			// 
			// richTextBox_output
			// 
			this->richTextBox_output->Location = System::Drawing::Point(363, 133);
			this->richTextBox_output->Name = L"richTextBox_output";
			this->richTextBox_output->ReadOnly = true;
			this->richTextBox_output->Size = System::Drawing::Size(490, 371);
			this->richTextBox_output->TabIndex = 6;
			this->richTextBox_output->Text = L"Output:\n";
			// 
			// button_imagesTaken
			// 
			this->button_imagesTaken->Enabled = false;
			this->button_imagesTaken->Location = System::Drawing::Point(59, 133);
			this->button_imagesTaken->Name = L"button_imagesTaken";
			this->button_imagesTaken->Size = System::Drawing::Size(239, 23);
			this->button_imagesTaken->TabIndex = 7;
			this->button_imagesTaken->Text = L"Check Images Taken";
			this->button_imagesTaken->UseVisualStyleBackColor = true;
			this->button_imagesTaken->Click += gcnew System::EventHandler(this, &MyForm::button_imagesTaken_Click);
			// 
			// button_LoRaPackets
			// 
			this->button_LoRaPackets->Enabled = false;
			this->button_LoRaPackets->Location = System::Drawing::Point(59, 162);
			this->button_LoRaPackets->Name = L"button_LoRaPackets";
			this->button_LoRaPackets->Size = System::Drawing::Size(239, 23);
			this->button_LoRaPackets->TabIndex = 8;
			this->button_LoRaPackets->Text = L"Check LoRa Packets Received";
			this->button_LoRaPackets->UseVisualStyleBackColor = true;
			this->button_LoRaPackets->Click += gcnew System::EventHandler(this, &MyForm::button_LoRaPackets_Click);
			// 
			// button_saveLogFile
			// 
			this->button_saveLogFile->Enabled = false;
			this->button_saveLogFile->Location = System::Drawing::Point(59, 191);
			this->button_saveLogFile->Name = L"button_saveLogFile";
			this->button_saveLogFile->Size = System::Drawing::Size(239, 23);
			this->button_saveLogFile->TabIndex = 9;
			this->button_saveLogFile->Text = L"Save Current log File To Old log Files";
			this->button_saveLogFile->UseVisualStyleBackColor = true;
			this->button_saveLogFile->Click += gcnew System::EventHandler(this, &MyForm::button_saveLogFile_Click);
			// 
			// button_oldLogFiles
			// 
			this->button_oldLogFiles->Enabled = false;
			this->button_oldLogFiles->Location = System::Drawing::Point(59, 220);
			this->button_oldLogFiles->Name = L"button_oldLogFiles";
			this->button_oldLogFiles->Size = System::Drawing::Size(239, 23);
			this->button_oldLogFiles->TabIndex = 10;
			this->button_oldLogFiles->Text = L"Check Old log Files";
			this->button_oldLogFiles->UseVisualStyleBackColor = true;
			this->button_oldLogFiles->Click += gcnew System::EventHandler(this, &MyForm::button_oldLogFiles_Click);
			// 
			// button_houseKeepingData
			// 
			this->button_houseKeepingData->Enabled = false;
			this->button_houseKeepingData->Location = System::Drawing::Point(59, 299);
			this->button_houseKeepingData->Name = L"button_houseKeepingData";
			this->button_houseKeepingData->Size = System::Drawing::Size(239, 42);
			this->button_houseKeepingData->TabIndex = 11;
			this->button_houseKeepingData->Text = L"Request HouseKeeping Data";
			this->button_houseKeepingData->UseVisualStyleBackColor = true;
			this->button_houseKeepingData->Click += gcnew System::EventHandler(this, &MyForm::button_houseKeepingData_Click);
			// 
			// button_downlinkRequest
			// 
			this->button_downlinkRequest->Enabled = false;
			this->button_downlinkRequest->Location = System::Drawing::Point(239, 367);
			this->button_downlinkRequest->Name = L"button_downlinkRequest";
			this->button_downlinkRequest->Size = System::Drawing::Size(107, 47);
			this->button_downlinkRequest->TabIndex = 12;
			this->button_downlinkRequest->Text = L"Request Downlink File Transfer";
			this->button_downlinkRequest->UseVisualStyleBackColor = true;
			this->button_downlinkRequest->Click += gcnew System::EventHandler(this, &MyForm::button_downlinkRequest_Click);
			// 
			// comboBox_downlinkFileName
			// 
			this->comboBox_downlinkFileName->Enabled = false;
			this->comboBox_downlinkFileName->FormattingEnabled = true;
			this->comboBox_downlinkFileName->Location = System::Drawing::Point(33, 384);
			this->comboBox_downlinkFileName->Name = L"comboBox_downlinkFileName";
			this->comboBox_downlinkFileName->Size = System::Drawing::Size(200, 21);
			this->comboBox_downlinkFileName->TabIndex = 13;
			// 
			// label_downlinkFileName
			// 
			this->label_downlinkFileName->AutoSize = true;
			this->label_downlinkFileName->Location = System::Drawing::Point(30, 367);
			this->label_downlinkFileName->Name = L"label_downlinkFileName";
			this->label_downlinkFileName->Size = System::Drawing::Size(150, 13);
			this->label_downlinkFileName->TabIndex = 14;
			this->label_downlinkFileName->Text = L"File To Request For Downlink:";
			// 
			// label_uplinkFileName
			// 
			this->label_uplinkFileName->AutoSize = true;
			this->label_uplinkFileName->Location = System::Drawing::Point(30, 430);
			this->label_uplinkFileName->Name = L"label_uplinkFileName";
			this->label_uplinkFileName->Size = System::Drawing::Size(114, 13);
			this->label_uplinkFileName->TabIndex = 17;
			this->label_uplinkFileName->Text = L"File To Attempt Uplink:";
			// 
			// button_uplinkRequest
			// 
			this->button_uplinkRequest->Enabled = false;
			this->button_uplinkRequest->Location = System::Drawing::Point(239, 430);
			this->button_uplinkRequest->Name = L"button_uplinkRequest";
			this->button_uplinkRequest->Size = System::Drawing::Size(107, 67);
			this->button_uplinkRequest->TabIndex = 15;
			this->button_uplinkRequest->Text = L"Attempt Uplink File Transfer";
			this->button_uplinkRequest->UseVisualStyleBackColor = true;
			this->button_uplinkRequest->Click += gcnew System::EventHandler(this, &MyForm::button_uplinkRequest_Click);
			// 
			// textBox_uplinkFileName
			// 
			this->textBox_uplinkFileName->Enabled = false;
			this->textBox_uplinkFileName->Location = System::Drawing::Point(33, 446);
			this->textBox_uplinkFileName->Multiline = true;
			this->textBox_uplinkFileName->Name = L"textBox_uplinkFileName";
			this->textBox_uplinkFileName->ScrollBars = System::Windows::Forms::ScrollBars::Vertical;
			this->textBox_uplinkFileName->Size = System::Drawing::Size(200, 41);
			this->textBox_uplinkFileName->TabIndex = 18;
			this->textBox_uplinkFileName->Text = L"%USERPROFILE%\\Desktop\\downlinks\\im\\testImage2.png";
			// 
			// groupBox_Initialization
			// 
			this->groupBox_Initialization->Controls->Add(this->comboBox_TNCPort);
			this->groupBox_Initialization->Controls->Add(this->label_TNCPort);
			this->groupBox_Initialization->Controls->Add(this->progressBar_PortOpenSatus);
			this->groupBox_Initialization->Controls->Add(this->button_closePort);
			this->groupBox_Initialization->Controls->Add(this->button_initPort);
			this->groupBox_Initialization->Controls->Add(this->label_baudRate);
			this->groupBox_Initialization->Controls->Add(this->comboBox_Baud);
			this->groupBox_Initialization->Controls->Add(this->label_COMPort);
			this->groupBox_Initialization->Controls->Add(this->comboBox_COMPort);
			this->groupBox_Initialization->Location = System::Drawing::Point(88, 5);
			this->groupBox_Initialization->Name = L"groupBox_Initialization";
			this->groupBox_Initialization->Size = System::Drawing::Size(482, 119);
			this->groupBox_Initialization->TabIndex = 19;
			this->groupBox_Initialization->TabStop = false;
			this->groupBox_Initialization->Text = L"Initialization";
			// 
			// comboBox_TNCPort
			// 
			this->comboBox_TNCPort->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->comboBox_TNCPort->FormattingEnabled = true;
			this->comboBox_TNCPort->Items->AddRange(gcnew cli::array< System::Object^  >(16) {
				L"0", L"1", L"2", L"3", L"4", L"5", L"6",
					L"7", L"8", L"9", L"A", L"B", L"C", L"D", L"E", L"F"
			});
			this->comboBox_TNCPort->Location = System::Drawing::Point(76, 76);
			this->comboBox_TNCPort->Name = L"comboBox_TNCPort";
			this->comboBox_TNCPort->Size = System::Drawing::Size(121, 21);
			this->comboBox_TNCPort->TabIndex = 33;
			// 
			// label_TNCPort
			// 
			this->label_TNCPort->AutoSize = true;
			this->label_TNCPort->Location = System::Drawing::Point(9, 79);
			this->label_TNCPort->Name = L"label_TNCPort";
			this->label_TNCPort->Size = System::Drawing::Size(54, 13);
			this->label_TNCPort->TabIndex = 32;
			this->label_TNCPort->Text = L"TNC Port:";
			// 
			// progressBar_PortOpenSatus
			// 
			this->progressBar_PortOpenSatus->Location = System::Drawing::Point(326, 49);
			this->progressBar_PortOpenSatus->Name = L"progressBar_PortOpenSatus";
			this->progressBar_PortOpenSatus->Size = System::Drawing::Size(31, 18);
			this->progressBar_PortOpenSatus->TabIndex = 6;
			// 
			// backgroundWorker_Receiver
			// 
			this->backgroundWorker_Receiver->WorkerReportsProgress = true;
			this->backgroundWorker_Receiver->WorkerSupportsCancellation = true;
			this->backgroundWorker_Receiver->DoWork += gcnew System::ComponentModel::DoWorkEventHandler(this, &MyForm::backgroundWorker_Receiver_DoWork);
			this->backgroundWorker_Receiver->ProgressChanged += gcnew System::ComponentModel::ProgressChangedEventHandler(this, &MyForm::backgroundWorker_Receiver_ProgressChanged);
			this->backgroundWorker_Receiver->RunWorkerCompleted += gcnew System::ComponentModel::RunWorkerCompletedEventHandler(this, &MyForm::backgroundWorker_Receiver_RunWorkerCompleted);
			// 
			// progressBar_uplink
			// 
			this->progressBar_uplink->Location = System::Drawing::Point(33, 489);
			this->progressBar_uplink->Name = L"progressBar_uplink";
			this->progressBar_uplink->Size = System::Drawing::Size(200, 8);
			this->progressBar_uplink->TabIndex = 20;
			// 
			// backgroundWorker_Uplink
			// 
			this->backgroundWorker_Uplink->WorkerReportsProgress = true;
			this->backgroundWorker_Uplink->WorkerSupportsCancellation = true;
			this->backgroundWorker_Uplink->DoWork += gcnew System::ComponentModel::DoWorkEventHandler(this, &MyForm::backgroundWorker_Uplink_DoWork);
			this->backgroundWorker_Uplink->ProgressChanged += gcnew System::ComponentModel::ProgressChangedEventHandler(this, &MyForm::backgroundWorker_Uplink_ProgressChanged);
			this->backgroundWorker_Uplink->RunWorkerCompleted += gcnew System::ComponentModel::RunWorkerCompletedEventHandler(this, &MyForm::backgroundWorker_Uplink_RunWorkerCompleted);
			// 
			// backgroundWorker_DownlinkPartRequest
			// 
			this->backgroundWorker_DownlinkPartRequest->WorkerReportsProgress = true;
			this->backgroundWorker_DownlinkPartRequest->WorkerSupportsCancellation = true;
			this->backgroundWorker_DownlinkPartRequest->DoWork += gcnew System::ComponentModel::DoWorkEventHandler(this, &MyForm::backgroundWorker_DownlinkPartRequest_DoWork);
			this->backgroundWorker_DownlinkPartRequest->ProgressChanged += gcnew System::ComponentModel::ProgressChangedEventHandler(this, &MyForm::backgroundWorker_DownlinkPartRequest_ProgressChanged);
			// 
			// textBox_AX25SatCallsign
			// 
			this->textBox_AX25SatCallsign->CharacterCasing = System::Windows::Forms::CharacterCasing::Upper;
			this->textBox_AX25SatCallsign->Location = System::Drawing::Point(731, 30);
			this->textBox_AX25SatCallsign->MaxLength = 16;
			this->textBox_AX25SatCallsign->Name = L"textBox_AX25SatCallsign";
			this->textBox_AX25SatCallsign->Size = System::Drawing::Size(58, 20);
			this->textBox_AX25SatCallsign->TabIndex = 22;
			this->textBox_AX25SatCallsign->Text = L"N0CAL";
			this->textBox_AX25SatCallsign->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			this->textBox_AX25SatCallsign->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &MyForm::textBox_AX25Callsign_KeyPress);
			// 
			// label_AX25Callsign
			// 
			this->label_AX25Callsign->AutoSize = true;
			this->label_AX25Callsign->Location = System::Drawing::Point(644, 33);
			this->label_AX25Callsign->Name = L"label_AX25Callsign";
			this->label_AX25Callsign->Size = System::Drawing::Size(78, 13);
			this->label_AX25Callsign->TabIndex = 23;
			this->label_AX25Callsign->Text = L"AX.25 Callsign:";
			// 
			// button_uplinkCancel
			// 
			this->button_uplinkCancel->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(255)),
				static_cast<System::Int32>(static_cast<System::Byte>(128)), static_cast<System::Int32>(static_cast<System::Byte>(128)));
			this->button_uplinkCancel->Enabled = false;
			this->button_uplinkCancel->ForeColor = System::Drawing::SystemColors::ControlText;
			this->button_uplinkCancel->Location = System::Drawing::Point(179, 420);
			this->button_uplinkCancel->Name = L"button_uplinkCancel";
			this->button_uplinkCancel->Size = System::Drawing::Size(54, 23);
			this->button_uplinkCancel->TabIndex = 25;
			this->button_uplinkCancel->Text = L"Cancel";
			this->button_uplinkCancel->UseVisualStyleBackColor = false;
			this->button_uplinkCancel->Visible = false;
			this->button_uplinkCancel->Click += gcnew System::EventHandler(this, &MyForm::button_uplinkCancel_Click);
			// 
			// label_AX25SSID
			// 
			this->label_AX25SSID->AutoSize = true;
			this->label_AX25SSID->Location = System::Drawing::Point(644, 12);
			this->label_AX25SSID->Name = L"label_AX25SSID";
			this->label_AX25SSID->Size = System::Drawing::Size(67, 13);
			this->label_AX25SSID->TabIndex = 27;
			this->label_AX25SSID->Text = L"AX.25 SSID:";
			// 
			// textBox_AX25SatSSID
			// 
			this->textBox_AX25SatSSID->CharacterCasing = System::Windows::Forms::CharacterCasing::Upper;
			this->textBox_AX25SatSSID->Location = System::Drawing::Point(731, 9);
			this->textBox_AX25SatSSID->MaxLength = 4;
			this->textBox_AX25SatSSID->Name = L"textBox_AX25SatSSID";
			this->textBox_AX25SatSSID->Size = System::Drawing::Size(58, 20);
			this->textBox_AX25SatSSID->TabIndex = 26;
			this->textBox_AX25SatSSID->Text = L"2";
			this->textBox_AX25SatSSID->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			this->textBox_AX25SatSSID->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &MyForm::textBox_AX25SSID_KeyPress);
			// 
			// textBox_DownlinkSaveLocation
			// 
			this->textBox_DownlinkSaveLocation->Location = System::Drawing::Point(600, 79);
			this->textBox_DownlinkSaveLocation->Multiline = true;
			this->textBox_DownlinkSaveLocation->Name = L"textBox_DownlinkSaveLocation";
			this->textBox_DownlinkSaveLocation->ScrollBars = System::Windows::Forms::ScrollBars::Vertical;
			this->textBox_DownlinkSaveLocation->Size = System::Drawing::Size(200, 41);
			this->textBox_DownlinkSaveLocation->TabIndex = 29;
			this->textBox_DownlinkSaveLocation->Text = L"%USERPROFILE%\\Desktop\\downlinks";
			// 
			// labelDownlinkSaveLocation
			// 
			this->labelDownlinkSaveLocation->AutoSize = true;
			this->labelDownlinkSaveLocation->Location = System::Drawing::Point(597, 63);
			this->labelDownlinkSaveLocation->Name = L"labelDownlinkSaveLocation";
			this->labelDownlinkSaveLocation->Size = System::Drawing::Size(126, 13);
			this->labelDownlinkSaveLocation->TabIndex = 28;
			this->labelDownlinkSaveLocation->Text = L"Downlink Save Location:";
			// 
			// button_saveNewConfigFile
			// 
			this->button_saveNewConfigFile->Enabled = false;
			this->button_saveNewConfigFile->Location = System::Drawing::Point(60, 249);
			this->button_saveNewConfigFile->Name = L"button_saveNewConfigFile";
			this->button_saveNewConfigFile->Size = System::Drawing::Size(238, 23);
			this->button_saveNewConfigFile->TabIndex = 30;
			this->button_saveNewConfigFile->Text = L"Save new Config File and Apply";
			this->button_saveNewConfigFile->UseVisualStyleBackColor = true;
			this->button_saveNewConfigFile->Click += gcnew System::EventHandler(this, &MyForm::button_saveNewConfigFile_Click);
			// 
			// backgroundWorker_Resender
			// 
			this->backgroundWorker_Resender->WorkerSupportsCancellation = true;
			this->backgroundWorker_Resender->DoWork += gcnew System::ComponentModel::DoWorkEventHandler(this, &MyForm::backgroundWorker_Resender_DoWork);
			this->backgroundWorker_Resender->RunWorkerCompleted += gcnew System::ComponentModel::RunWorkerCompletedEventHandler(this, &MyForm::backgroundWorker_Resender_RunWorkerCompleted);
			// 
			// button_about
			// 
			this->button_about->Location = System::Drawing::Point(4, 6);
			this->button_about->Name = L"button_about";
			this->button_about->Size = System::Drawing::Size(51, 23);
			this->button_about->TabIndex = 31;
			this->button_about->Text = L"About";
			this->button_about->UseVisualStyleBackColor = true;
			this->button_about->Click += gcnew System::EventHandler(this, &MyForm::button_about_Click);
			// 
			// textBox_AX25GSSSID
			// 
			this->textBox_AX25GSSSID->CharacterCasing = System::Windows::Forms::CharacterCasing::Upper;
			this->textBox_AX25GSSSID->Location = System::Drawing::Point(795, 9);
			this->textBox_AX25GSSSID->MaxLength = 4;
			this->textBox_AX25GSSSID->Name = L"textBox_AX25GSSSID";
			this->textBox_AX25GSSSID->Size = System::Drawing::Size(58, 20);
			this->textBox_AX25GSSSID->TabIndex = 33;
			this->textBox_AX25GSSSID->Text = L"1";
			this->textBox_AX25GSSSID->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			this->textBox_AX25GSSSID->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &MyForm::textBox_AX25SSID_KeyPress);
			// 
			// textBox_AX25GSCallsign
			// 
			this->textBox_AX25GSCallsign->CharacterCasing = System::Windows::Forms::CharacterCasing::Upper;
			this->textBox_AX25GSCallsign->Location = System::Drawing::Point(795, 30);
			this->textBox_AX25GSCallsign->MaxLength = 16;
			this->textBox_AX25GSCallsign->Name = L"textBox_AX25GSCallsign";
			this->textBox_AX25GSCallsign->Size = System::Drawing::Size(58, 20);
			this->textBox_AX25GSCallsign->TabIndex = 32;
			this->textBox_AX25GSCallsign->Text = L"N0CAL";
			this->textBox_AX25GSCallsign->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			this->textBox_AX25GSCallsign->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &MyForm::textBox_AX25Callsign_KeyPress);
			// 
			// label_AX25Sat
			// 
			this->label_AX25Sat->AutoSize = true;
			this->label_AX25Sat->Location = System::Drawing::Point(745, 53);
			this->label_AX25Sat->Name = L"label_AX25Sat";
			this->label_AX25Sat->Size = System::Drawing::Size(44, 13);
			this->label_AX25Sat->TabIndex = 34;
			this->label_AX25Sat->Text = L"Satellite";
			// 
			// label_AX25GS
			// 
			this->label_AX25GS->AutoSize = true;
			this->label_AX25GS->Location = System::Drawing::Point(831, 53);
			this->label_AX25GS->Name = L"label_AX25GS";
			this->label_AX25GS->Size = System::Drawing::Size(22, 13);
			this->label_AX25GS->TabIndex = 35;
			this->label_AX25GS->Text = L"GS";
			// 
			// MyForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(865, 516);
			this->Controls->Add(this->label_AX25GS);
			this->Controls->Add(this->label_AX25Sat);
			this->Controls->Add(this->textBox_AX25GSSSID);
			this->Controls->Add(this->textBox_AX25GSCallsign);
			this->Controls->Add(this->button_about);
			this->Controls->Add(this->button_saveNewConfigFile);
			this->Controls->Add(this->textBox_DownlinkSaveLocation);
			this->Controls->Add(this->labelDownlinkSaveLocation);
			this->Controls->Add(this->label_AX25SSID);
			this->Controls->Add(this->textBox_AX25SatSSID);
			this->Controls->Add(this->button_uplinkCancel);
			this->Controls->Add(this->label_AX25Callsign);
			this->Controls->Add(this->textBox_AX25SatCallsign);
			this->Controls->Add(this->progressBar_uplink);
			this->Controls->Add(this->groupBox_Initialization);
			this->Controls->Add(this->textBox_uplinkFileName);
			this->Controls->Add(this->label_uplinkFileName);
			this->Controls->Add(this->button_uplinkRequest);
			this->Controls->Add(this->label_downlinkFileName);
			this->Controls->Add(this->comboBox_downlinkFileName);
			this->Controls->Add(this->button_downlinkRequest);
			this->Controls->Add(this->button_houseKeepingData);
			this->Controls->Add(this->button_oldLogFiles);
			this->Controls->Add(this->button_saveLogFile);
			this->Controls->Add(this->button_LoRaPackets);
			this->Controls->Add(this->button_imagesTaken);
			this->Controls->Add(this->richTextBox_output);
			this->Icon = (cli::safe_cast<System::Drawing::Icon^>(resources->GetObject(L"$this.Icon")));
			this->Name = L"MyForm";
			this->Text = L"NaSPUoN Ground Station Software";
			this->groupBox_Initialization->ResumeLayout(false);
			this->groupBox_Initialization->PerformLayout();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion

			//Find available COM Ports
			private: void findPorts(void) {
				//Get Port names
				array<Object^>^ objectArray = SerialPort::GetPortNames();
				//Add String array to combobox
				this->comboBox_COMPort->Items->AddRange( objectArray );
			}

			//To find ports in case the device was plugged in after starting the program
			private: System::Void label_COMPort_DoubleClick(System::Object^  sender, System::EventArgs^  e) {
				this->comboBox_COMPort->Items->Clear();
				findPorts();
			}

			//Log Output to Form's richTextBox
			public: void log(std::string message);
			public: void logErr(std::string message);
			public: void clearOutput();

			//Enable Buttons and input controls
			private: void setEnableInputs(bool enabled) {
				//Setting setup Controls to opposite of input controls
				this->button_initPort->Enabled = !enabled;
				this->comboBox_COMPort->Enabled = !enabled;
				this->comboBox_Baud->Enabled = !enabled;
				this->comboBox_TNCPort->Enabled = !enabled;
				this->textBox_AX25SatSSID->Enabled = !enabled;
				this->textBox_AX25SatCallsign->Enabled = !enabled;
				this->textBox_AX25GSSSID->Enabled = !enabled;
				this->textBox_AX25GSCallsign->Enabled = !enabled;
				this->textBox_DownlinkSaveLocation->Enabled = !enabled;
				//Enabling or disabling input controls
				this->button_closePort->Enabled = enabled;
				this->button_downlinkRequest->Enabled = enabled;
				this->button_houseKeepingData->Enabled = enabled;
				this->button_imagesTaken->Enabled = enabled;
				this->button_LoRaPackets->Enabled = enabled;
				this->button_oldLogFiles->Enabled = enabled;
				this->button_saveNewConfigFile->Enabled = enabled;
				this->button_saveLogFile->Enabled = enabled;
				this->button_uplinkRequest->Enabled = enabled;
				this->comboBox_downlinkFileName->Enabled = enabled;
				this->textBox_uplinkFileName->Enabled = enabled;
				if (enabled) {
					clearOutput();
					this->progressBar_PortOpenSatus->Value = 100;
				}
				else {
					this->progressBar_PortOpenSatus->Value = 0;
				}
			}

			//Init Port Button
			private: System::Void button_initPort_Click(System::Object^  sender, System::EventArgs^  e) {
				if ((this->comboBox_COMPort->Text == String::Empty) || (this->comboBox_Baud->Text == String::Empty) || (this->comboBox_TNCPort->Text == String::Empty)) {
					logErr("Please select a COM Port, TNC Port and Baud Rate.");
					return;
				}
				if ((this->textBox_AX25SatSSID->Text == String::Empty) || (this->textBox_AX25SatCallsign->Text == String::Empty)) {
					logErr("Please enter the satellite's AX.25 Callsign and SSID.");
					return;
				}
				if ((this->textBox_AX25GSSSID->Text == String::Empty) || (this->textBox_AX25GSCallsign->Text == String::Empty)) {
					logErr("Please enter the Ground Station's AX.25 Callsign and SSID.");
					return;
				}
				if (this->textBox_DownlinkSaveLocation->Text == String::Empty) {
					logErr("Please enter a save location for downlinks.");
					return;
				}
				this->downlinkSavelocation = this->textBox_DownlinkSaveLocation->Text;
				if (!System::Text::RegularExpressions::Regex::IsMatch(this->textBox_AX25SatSSID->Text, "^([A-F0-9]{1})$")) {
					logErr("Please enter a valid Hex digit for the satellite's AX.25 SSID.");
					return;
				}
				if (!System::Text::RegularExpressions::Regex::IsMatch(this->textBox_AX25SatCallsign->Text, "^([A-Z]{1,2}[0-9][A-Z]{1,3})$")) {
					logErr("Please enter a valid AX.25 Callsign for the satellite ({1or2 letters}{1 number}{1to3 letters}).");
					return;
				}
				if (!System::Text::RegularExpressions::Regex::IsMatch(this->textBox_AX25GSSSID->Text, "^([A-F0-9]{1})$")) {
					logErr("Please enter a valid Hex digit for the Ground Station's AX.25 SSID.");
					return;
				}
				if (!System::Text::RegularExpressions::Regex::IsMatch(this->textBox_AX25GSCallsign->Text, "^([A-Z]{1,2}[0-9][A-Z]{1,3})$")) {
					logErr("Please enter a valid AX.25 Callsign for the Ground Station ({1or2 letters}{1 number}{1to3 letters}).");
					return;
				}
				try {
					//Make sure Port isn't open
					if (this->serialPort1->IsOpen) {
						logErr("The port is already open.");
						return;
					}
					//Setting serial port parameters
					this->serialPort1->PortName = this->comboBox_COMPort->Text;
					this->serialPort1->BaudRate = Int32::Parse(this->comboBox_Baud->Text);

					//Open Port
					this->serialPort1->Open();

					//Start Receiver Thread
					if (!(this->backgroundWorker_Receiver->IsBusy)) {
						this->backgroundWorker_Receiver->RunWorkerAsync();
					}

					//Start Resender Thread
					if (!(this->backgroundWorker_Resender->IsBusy)) {
						this->backgroundWorker_Resender->RunWorkerAsync();
					}

					//Getting AX.25 KISS TNC port
					this->TNCPort = this->comboBox_TNCPort->Text[0] - '0';
					if (this->TNCPort > 10)
						this->TNCPort += (('0' + 10) - 'A');

					//Getting satellite's AX.25 SSID and Callsign
					this->satelliteSSID = this->textBox_AX25SatSSID->Text[0] - '0';
					if (this->satelliteSSID > 10)
						this->satelliteSSID += (('0' + 10) - 'A');
					this->satelliteCallsign = this->textBox_AX25SatCallsign->Text;

					//Getting Ground Station's AX.25 SSID and Callsign
					this->groundSSID = this->textBox_AX25GSSSID->Text[0] - '0';
					if (this->groundSSID > 10)
						this->groundSSID += (('0' + 10) - 'A');
					this->satelliteCallsign = this->textBox_AX25SatCallsign->Text;
					this->groundCallsign = this->textBox_AX25GSCallsign->Text;

					//Enabling input controls
					setEnableInputs(true);
				}
				catch (UnauthorizedAccessException^) {
					logErr("Unauthorized Access.");
				}
				catch (...) {
					logErr("An error occured when attempting to initialize the port.");
				}
			}

			//Close Port Button
			private: System::Void button_closePort_Click(System::Object^  sender, System::EventArgs^  e) {
				try {
					//Make sure Port is open
					if (!(this->serialPort1->IsOpen)) {
						logErr("Could not close the port. The port is not open.");
						setEnableInputs(false);
					}
					//Make sure downlinks are complete
					else if (!transferForms->empty()) {
						logErr("Could not close the port. Downlinks in progress.");
					}
					else {
						//Stop Receiver thread
						if (this->backgroundWorker_Receiver->IsBusy) {
							//Causes an error to be thrown and stop the SerialPort.Read() call
							this->serialPort1->ReadTimeout = -1;
							backgroundWorker_Receiver->CancelAsync();
							System::Threading::Thread::CurrentThread->Sleep(30);
						}
						//Stop Resender thread
						if (this->backgroundWorker_Resender->IsBusy) {
							backgroundWorker_Resender->CancelAsync();
							System::Threading::Thread::CurrentThread->Sleep(500);
						}

						//Close the port if it is open
						this->serialPort1->Close();
						setEnableInputs(false);
					}

				}
				catch (...) {
					logErr("An error occured when attempting to close the port.");
				}
			}

			//Thread for receiving data from the serialPort
			private: System::Void backgroundWorker_Receiver_DoWork(System::Object^  sender, System::ComponentModel::DoWorkEventArgs^  e) {
				
				//Saving receiver thread ID to UI thread to enable for output to form's controls that are owned by the UIThread
				int rcvThrdID = System::Threading::Thread::CurrentThread->ManagedThreadId;
				backgroundWorker_Receiver->ReportProgress(10, rcvThrdID);
				//Stalling for the receiveThreadID to be updated
				System::Threading::Thread::CurrentThread->Sleep(100);
				
				//Check if serial port is open
				if (!(this->serialPort1->IsOpen)) {
					logErr("Could not start the receiver thread. The port is not open.");
					setEnableInputs(false);
					return;
				}

				log("Receiver Thread started successfully");

				int readBufferSize = this->serialPort1->ReadBufferSize;
				int bytesRead = 0;
				array<uint8_t>^ received;
				Array::Resize(received, readBufferSize);
				while (true) {
					Array::Clear(received, 0, readBufferSize);
					try {
						//Check for pending cancellation of the thread
						if (this->backgroundWorker_Receiver->CancellationPending) {
							e->Cancel = true;
							return;
						}

						//Read from the serial port
						bytesRead = this->serialPort1->Read(received, 0, readBufferSize);
						if (bytesRead > 0) {
							//Convert array to vector
							std::vector<uint8_t> receivedVect;
							receivedVect.resize(bytesRead);
							pin_ptr<uint8_t> pinned = &received[0];
							std::memcpy(receivedVect.data(), pinned, bytesRead);
							processReceivedPacketAX25(receivedVect);
						}
					}
					catch (Exception^ exc) {
						if (this->backgroundWorker_Receiver->CancellationPending) {
							e->Cancel = true;
							return;
						}
						std::string errorType = msclr::interop::marshal_as<std::string>(exc->GetType()->ToString());
						logErr(errorType + "Error occurred in the receiver thread when attempting to read off of the serial port.");
					}
				}
			}

			//Function that is executed on UI thread when backgroundWorker_Receiver->ReportProgress() is called
			private: System::Void backgroundWorker_Receiver_ProgressChanged(System::Object^  sender, System::ComponentModel::ProgressChangedEventArgs^  e) {
				if (e->ProgressPercentage == 10) {
					//For the system to know the threadID of the receiverBackgroundWorker
					this->receiverThreadID = safe_cast<int>(e->UserState);
				}
				else if (e->ProgressPercentage == 1) {
					//For the receiverBackgroundWorker to be able to log to the richTextBox
					log( msclr::interop::marshal_as<std::string>(e->UserState->ToString()) );
				}
				else if (e->ProgressPercentage == 2) {
					//For the receiverBackgroundWorker to be able to logErr to the richTextBox
					logErr( msclr::interop::marshal_as<std::string>(e->UserState->ToString()) );
				}
				else if (e->ProgressPercentage == 3) {
					//For new fileNames received to be added to combobox_downlinkFileName
					addDownlinkableFileName(e->UserState->ToString());
				}
				else if (e->ProgressPercentage == 4) {
					//For the progressBar_downlink to be updated
					downlinkProgressBarUpdate(e->UserState);
				}
				else if (e->ProgressPercentage == 5) {
					//For the controls to be returned to normal for another request to be made
					downlinkComplete(safe_cast<uint16_t>(e->UserState));
				}
				else if (e->ProgressPercentage == 6) {
					//For the UI thread to start the uplink backgroundWorker
					startBackgroundWorkerUplink(e->UserState);
				}
				else if (e->ProgressPercentage == 7) {
					//For the UI thread to start the downlink part request backgroundWorker
					startBackgroundWorkerDownlinkPartRequest(e->UserState);
				}
				else if (e->ProgressPercentage == 8) {
					//For the downlink controls to be disabled
					downlinkStarting(safe_cast<uint16_t>(e->UserState));
				}
			}

			//Function that runs after receiver thread is closed
			private: System::Void backgroundWorker_Receiver_RunWorkerCompleted(System::Object^  sender, System::ComponentModel::RunWorkerCompletedEventArgs^  e) {
				log("Receiver Thread closed.");
			}

			//Thread for resending high priority packets if they are not received
			private: System::Void backgroundWorker_Resender_DoWork(System::Object^  sender, System::ComponentModel::DoWorkEventArgs^  e) {
				while (1) {
					System::Threading::Thread::CurrentThread->Sleep(200);
					if (this->backgroundWorker_Receiver->CancellationPending) {
						e->Cancel = true;
						return;
					}

					sendAX25Frames();
				}
			}

			//Function that runs after resender thread is closed
			private: System::Void backgroundWorker_Resender_RunWorkerCompleted(System::Object^  sender, System::ComponentModel::RunWorkerCompletedEventArgs^  e) {
				log("Resender Thread closed.");
			}

			//Add downlinkable fileNames to comboBox_downlinkFileNames
			private: void addDownlinkableFileName(String^ fileName) {
				if (System::Threading::Thread::CurrentThread->ManagedThreadId == this->UIThreadID) {
					if (!this->comboBox_downlinkFileName->Items->Contains(fileName))
						this->comboBox_downlinkFileName->Items->Add(fileName);
				}
				else if (System::Threading::Thread::CurrentThread->ManagedThreadId == this->receiverThreadID) {
					if (backgroundWorker_Receiver->IsBusy)
						backgroundWorker_Receiver->ReportProgress(3, fileName);
				}
			}

			//Update the value of progressBar_downlink
			private: void downlinkProgressBarUpdate(System::Object^ args) {
				if (System::Threading::Thread::CurrentThread->ManagedThreadId == this->UIThreadID) {
					array <uint16_t>^ values = safe_cast< array <uint16_t>^ >(args);
					int progressPercentage = values[0];
					uint16_t transferID = values[1];
					if (transferForms->count(transferID)){
						if (progressPercentage > 100)
							transferForms[transferID]->progressBar_downlink->Value = 100;
						else if (progressPercentage < 0)
							transferForms[transferID]->progressBar_downlink->Value = 0;
						else 
							transferForms[transferID]->progressBar_downlink->Value = progressPercentage;
					}
				}
				else if (System::Threading::Thread::CurrentThread->ManagedThreadId == this->receiverThreadID) {
					if (backgroundWorker_Receiver->IsBusy)
						backgroundWorker_Receiver->ReportProgress(4, args);
				}
			}

			//Send frame via serialPort to the AX.25 TNC
			private: void sendSerial(std::vector<uint8_t> message) {
				//Check if serial port is open
				if (!(this->serialPort1->IsOpen)) {
					logErr("Could not send message. The port is not open.");
					setEnableInputs(false);
					return;
				}
				//TODO: Find a more efficient way of converting std::vector to System::array
				int msgSize = message.size();
				array<Byte>^ msgArr = gcnew array<uint8_t>(msgSize);
				for (int i = 0; i < msgSize; i++) {
					msgArr[i] = message[i];
				}
				//Write the message to the serial port
				this->serialPort1->Write(msgArr, 0, msgSize);
			}

			//File Operations
			public: bool checkIfFileExists(std::string file_path);
			public: uint32_t getFileSize(std::string file_path);
			public: std::vector<uint8_t> retrieveFileToTransfer(std::string input_path);
			public: std::vector< std::vector<uint8_t> > splitFileIntoPackets(std::vector<uint8_t> file);
			public: void compilePackets(std::string output_path, std::vector< std::vector<uint8_t> > packets);

			//Bit manipulations
			public: std::vector<uint8_t> splitSixteenBitInt(uint16_t sixteenBitInt);
			public: std::vector<uint8_t> splitThirtyTwoBitInt(uint32_t thirtyTwoBitInt);
			public: std::vector<uint8_t> splitSixtyFourBitInt(uint64_t sixtyFourBitInt);
			public: void insertSixteenBitIntInEightBitVector(std::vector<uint8_t> & eightBitIntsVector, std::vector<uint8_t>::iterator position, uint16_t sixteenBitInt);
			public: void insertThirtyTwoBitIntInEightBitVector(std::vector<uint8_t> & eightBitIntsVector, std::vector<uint8_t>::iterator position, uint32_t thirtyTwoBitInt);
			public: void insertSixtyFourBitIntInEightBitVector(std::vector<uint8_t> & eightBitIntsVector, std::vector<uint8_t>::iterator position, uint64_t sixtyFourBitInt);
			public: uint16_t makeSixteenBitInt(std::vector<uint8_t> eightBitIntsVector);
			public: uint32_t makeThirtyTwoBitInt(std::vector<uint8_t> eightBitIntsVector);
			public: uint64_t makeSixtyFourBitInt(std::vector<uint8_t> eightBitIntsVector);
			public: uint16_t getSixteenBitIntFromEightBitVector(std::vector<uint8_t> & eightBitIntsVector, int start);
			public: uint32_t getThirtyTwoBitIntFromEightBitVector(std::vector<uint8_t> & eightBitIntsVector, int start);
			public: uint64_t getSixtyFourBitIntFromEightBitVector(std::vector<uint8_t> & eightBitIntsVector, int start);
			public: std::string vectorToHexString(std::vector<uint8_t> & vectorOfEightBitInts);
			public: std::string integerToHexString(uint32_t integer, int hexDigits);

			/*/Xbee Specific Communication Functions
			public: int incrementFrameIDCountXB();
			public: uint8_t ChecksumCalculator(std::vector<uint8_t> & data);
			public: void FrameConstructorXB(std::vector<uint8_t> & data);
			public: std::vector< std::vector<uint8_t> > checkReceivedPacketForFramesXB(std::vector<uint8_t> & rx);
			public: void processReceivedPacketXB(std::vector<uint8_t> & packet);
			public: bool isRFPacketHighPriority(std::vector<uint8_t> frame);
			public: void resendHighPriorityFrames();
			public: std::vector<uint8_t> removeFrameFromSentFrames(std::vector<uint8_t> & responseFrame);
			public: void sendFrameXB(std::vector<uint8_t> & frame, int rightPadding);
			public: void sendRFPacketXB(std::vector<uint8_t> sixtyFourBitAddress, uint16_t sixteenBitAddress, std::vector<uint8_t> & packet);*/

			//AX25 KISS TNC Specific Communication Functions
			public: void processReceivedPacketAX25(std::vector<uint8_t> & packet);
			public: std::vector<std::vector<uint8_t>> kissCombineFrame(std::vector<uint8_t> & receivedMsg);
			public: void kissDecapsulate(std::vector<uint8_t> & receivedFrame);
			public: void kissEncapsulate(bool command, std::vector<uint8_t> & outgoingMsg);
			public: void kissTranspose(std::vector<uint8_t> & outgoingMsg);
			public: std::vector<uint8_t> ax25Decapsulate(std::vector<uint8_t> & kissdecappedMsg);
			public: void ax25Encapsulate(std::vector<uint8_t> AX25SatCallsignSSID, std::vector<uint8_t> & outgoingMsg);
			public: void sendRFPacketAX25(std::vector<uint8_t> AX25SatCallsignSSID, std::vector<uint8_t> & packet);
			public: void checkKissTNCBufferSize();
			public: void sendAX25Frames();
			public: bool isOutAX25PacketHP(std::vector<uint8_t> & frameToSend);
			public: void HPAX25PacketEncapsulate(std::vector<uint8_t> AX25SatCallsignSSID, std::vector<uint8_t> & frameToSend);
			public: void HPAX25PacketDecapsulate(std::vector<uint8_t> AX25SatCallsignSSID, std::vector<uint8_t> & receivedFrame);

			//Communications Handler Functions
			public: FileTransfer initializeFileTransfer(bool incoming, std::vector<uint8_t> & initializerPacket);
			public: bool transferComplete(uint16_t tID, std::vector<uint8_t> AX25SatCallsignSSID);
			public: std::vector<uint8_t> getPacket(FileTransfer * ft, uint32_t index);
			public: std::vector<uint8_t> getInitializationPacket(FileTransfer * ft);
			public: void addPacket(FileTransfer * ft, std::vector<uint8_t> & packet);
			//public: void allowUplinkTransferToContinue(uint8_t frameID);
			public: void sendRFPacket(std::vector<uint8_t> AX25SatCallsignSSID, std::vector<uint8_t> & packet);
			public: void uplinkTransfer(std::vector<uint8_t> AX25SatCallsignSSID, uint16_t tID, System::ComponentModel::DoWorkEventArgs^  e);
			public: void downlinkPartRequestTransfer(std::vector<uint8_t> AX25SatCallsignSSID, uint16_t tID, System::ComponentModel::DoWorkEventArgs^  e);
			public: void processIncomingPayload(std::vector<uint8_t> AX25SatCallsignSSID, std::vector<uint8_t> & payload);
			public: std::vector< std::vector<uint8_t> > transferOutPacketsConstruction(uint16_t tID, std::string fileName, std::vector<uint8_t> & file);
			public: void cancelIncomingTransfer(uint16_t tID);
			public: void newOutgoingTransfer(std::vector<uint8_t> AX25SatCallsignSSID, std::string fileName);
			public: void cancelOutgoingTransfer(uint16_t tID);
			public: void cancelAllTransfers();

			//Function to make sure the UIThread is the one that starts the Uplink background worker
			private: void startBackgroundWorkerUplink(System::Object^ args) {
				if (System::Threading::Thread::CurrentThread->ManagedThreadId == this->UIThreadID) {
					if (!(this->backgroundWorker_Uplink->IsBusy)) {
						this->backgroundWorker_Uplink->RunWorkerAsync(args);
					}
				}
				else if (System::Threading::Thread::CurrentThread->ManagedThreadId == this->receiverThreadID) {
					this->backgroundWorker_Receiver->ReportProgress(6, args);
				}
			}

			//Thread for uplinking a file
			private: System::Void backgroundWorker_Uplink_DoWork(System::Object^  sender, System::ComponentModel::DoWorkEventArgs^  e) {
				
				//Saving Uplink thread ID to UI thread to enable for output to form's controls that are owned by the UIThread
				int uplnkThrdID = System::Threading::Thread::CurrentThread->ManagedThreadId;
				this->backgroundWorker_Uplink->ReportProgress(10, uplnkThrdID);
				//Stalling for the UplinkThreadID to be updated
				System::Threading::Thread::CurrentThread->Sleep(100);

				array<uint8_t>^ args = safe_cast< array<uint8_t>^ >(e->Argument);
				uint16_t tID = (args[0] << 8) + args[1];
				std::vector<uint8_t> AX25SatCallsignSSID;
				for (int i = 2; ((i < args->Length) && (i < 9)); i++) {
					if (args[i] != ';')
						AX25SatCallsignSSID.push_back((uint8_t)(args[i]));
				}

				uplinkTransfer(AX25SatCallsignSSID, tID, e);
			}

			//Function that is executed on UI thread when backgroundWorker_Uplink->ReportProgress() is called
			private: System::Void backgroundWorker_Uplink_ProgressChanged(System::Object^  sender, System::ComponentModel::ProgressChangedEventArgs^  e) {
				if (e->ProgressPercentage == 10) {
					//For the system to know the threadID of the uplink BackgroundWorker
					this->UplinkThreadID = safe_cast<int>(e->UserState);
				}
				else if (e->ProgressPercentage == 1) {
					//For the receiverBackgroundWorker to be able to log to the richTextBox
					log(msclr::interop::marshal_as<std::string>(e->UserState->ToString()));
				}
				else if (e->ProgressPercentage == 2) {
					//For the receiverBackgroundWorker to be able to logErr to the richTextBox
					logErr(msclr::interop::marshal_as<std::string>(e->UserState->ToString()));
				}
				else if (e->ProgressPercentage == 4) {
					//For the progressBar_downlink to be updated
					uplinkProgressBarUpdate(safe_cast<int>(e->UserState));
				}
				else if (e->ProgressPercentage == 5) {
					//For the controls to be returned to normal for another request to be made
					uplinkComplete(safe_cast<bool>(e->UserState));
				}
			}

			//Function that runs after Uplink thread is closed or completes
			private: System::Void backgroundWorker_Uplink_RunWorkerCompleted(System::Object^  sender, System::ComponentModel::RunWorkerCompletedEventArgs^  e) {
				if (e->Cancelled) {
					uplinkComplete(false);
				}
				else {
					uplinkComplete(true);
				}
			}

			//Function to make sure the UIThread is the one that starts the Downlink Part request background worker
			private: void startBackgroundWorkerDownlinkPartRequest(System::Object^ args) {
				if (System::Threading::Thread::CurrentThread->ManagedThreadId == this->UIThreadID) {
					if (!(this->backgroundWorker_DownlinkPartRequest->IsBusy)) {
						this->backgroundWorker_DownlinkPartRequest->RunWorkerAsync(args);
					}
				}
				else if (System::Threading::Thread::CurrentThread->ManagedThreadId == this->receiverThreadID) {
					this->backgroundWorker_Receiver->ReportProgress(7, args);
				}
			}

			//Thread for Downlink part request
			private: System::Void backgroundWorker_DownlinkPartRequest_DoWork(System::Object^  sender, System::ComponentModel::DoWorkEventArgs^  e) {

				//Saving downlink part request thread ID to UI thread to enable for output to form's controls that are owned by the UIThread
				int dwnlnkPtRqThrdID = System::Threading::Thread::CurrentThread->ManagedThreadId;
				backgroundWorker_DownlinkPartRequest->ReportProgress(10, dwnlnkPtRqThrdID);
				//Stalling for the DownlinkPtRqThreadID to be updated
				System::Threading::Thread::CurrentThread->Sleep(100);

				array<uint8_t>^ args = safe_cast<array<uint8_t>^>(e->Argument);
				uint16_t tID = (args[0] << 8) + args[1];
				std::vector<uint8_t> AX25SatCallsignSSID;
				for (int i = 2; ((i < args->Length) && (i < 9)); i++) {
					if (args[i] != ';')
						AX25SatCallsignSSID.push_back((uint8_t)(args[i]));
				}

				downlinkPartRequestTransfer(AX25SatCallsignSSID, tID, e);
			}

			//Function that is executed on UI thread when backgroundWorker_DownlinkPartRequest->ReportProgress() is called
			private: System::Void backgroundWorker_DownlinkPartRequest_ProgressChanged(System::Object^  sender, System::ComponentModel::ProgressChangedEventArgs^  e) {
				if (e->ProgressPercentage == 10) {
					//For the system to know the threadID of the DownlinkPartRequest_BackgroundWorker
					this->DownlinkPtRqThreadID = safe_cast<int>(e->UserState);
				}
				else if (e->ProgressPercentage == 1) {
					//For the receiverBackgroundWorker to be able to log to the richTextBox
					log(msclr::interop::marshal_as<std::string>(e->UserState->ToString()));
				}
				else if (e->ProgressPercentage == 2) {
					//For the receiverBackgroundWorker to be able to logErr to the richTextBox
					logErr(msclr::interop::marshal_as<std::string>(e->UserState->ToString()));
				}
			}

			//Update the value of progressBar_uplink
			private: void uplinkProgressBarUpdate(int progressPercentage) {
				if (System::Threading::Thread::CurrentThread->ManagedThreadId == this->UIThreadID) {
					if (progressPercentage > 100)
						this->progressBar_uplink->Value = 100;
					else if (progressPercentage < 0)
						this->progressBar_uplink->Value = 0;
					else
						this->progressBar_uplink->Value = progressPercentage;
				}
				else if (System::Threading::Thread::CurrentThread->ManagedThreadId == this->UplinkThreadID) {
					if (backgroundWorker_Uplink->IsBusy)
						backgroundWorker_Uplink->ReportProgress(4, progressPercentage);
				}
			}

			//Get a std::string of the downlink Save Location
			private: std::string getDownlinkSavelocation() {
				System::String^ str = this->downlinkSavelocation;
				std::string ret = (msclr::interop::marshal_as<std::string>(str));
				return ret;
			}

			//Get a std::vector<uint8_t> of the satellite Callsign and SSID
			private: std::vector<uint8_t> getSatelliteCallsignSSID() {
				uint8_t ssid = this->satelliteSSID;
				System::String^ str = this->satelliteCallsign;
				std::string stdstr = (msclr::interop::marshal_as<std::string>(str));
				std::vector<uint8_t> ret (stdstr.begin(), stdstr.end());
				ret.push_back(ssid);
				return ret;
			}

			//Get a std::vector<uint8_t> of the Ground Station Callsign and SSID
			private: std::vector<uint8_t> getGroundCallsignSSID() {
				uint8_t ssid = this->groundSSID;
				System::String^ str = this->groundCallsign;
				std::string stdstr = (msclr::interop::marshal_as<std::string>(str));
				std::vector<uint8_t> ret(stdstr.begin(), stdstr.end());
				ret.push_back(ssid);
				return ret;
			}

			//Check Images taken Button
			private: System::Void button_imagesTaken_Click(System::Object^  sender, System::EventArgs^  e) {
				std::vector<uint8_t> payload;
				payload.push_back('I');
				payload.push_back('M');
				sendRFPacket(this->getSatelliteCallsignSSID(), payload);
			}

			//Check LoRa Packets Button
			private: System::Void button_LoRaPackets_Click(System::Object^  sender, System::EventArgs^  e) {
				std::vector<uint8_t> payload;
				payload.push_back('L');
				payload.push_back('R');
				sendRFPacket(this->getSatelliteCallsignSSID(), payload);
			}

			//Save log File Button
			private: System::Void button_saveLogFile_Click(System::Object^  sender, System::EventArgs^  e) {
				std::vector<uint8_t> payload;
				payload.push_back('L');
				payload.push_back('M');
				sendRFPacket(this->getSatelliteCallsignSSID(), payload);
			}

			//Check Old log Files Button
			private: System::Void button_oldLogFiles_Click(System::Object^  sender, System::EventArgs^  e) {
				std::vector<uint8_t> payload;
				payload.push_back('L');
				payload.push_back('F');
				sendRFPacket(this->getSatelliteCallsignSSID(), payload);
			}

			//Save new Config File and Apply Button
			private: System::Void button_saveNewConfigFile_Click(System::Object^  sender, System::EventArgs^  e) {
				std::vector<uint8_t> payload;
				payload.push_back('X');
				payload.push_back('C');
				payload.push_back(0x01);//Should be 0x00 if new settings are NOT to be applied
				sendRFPacket(this->getSatelliteCallsignSSID(), payload);
			}

			//Request HouseKeeping Data Button
			private: System::Void button_houseKeepingData_Click(System::Object^  sender, System::EventArgs^  e) {
				std::vector<uint8_t> payload;
				payload.push_back('H');
				payload.push_back('K');
				sendRFPacket(this->getSatelliteCallsignSSID(), payload);
			}

			//Check keys typed into the AX.25 Callsign text box
			private: System::Void textBox_AX25Callsign_KeyPress(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e) {
				if (!System::Text::RegularExpressions::Regex::IsMatch(e->KeyChar.ToString(), "^([a-zA-Z0-9])*$"))
				{
					e->Handled = true;
				}
			}

			//Check keys typed into the AX.25 SSID text box
			private: System::Void textBox_AX25SSID_KeyPress(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e) {
				if (!System::Text::RegularExpressions::Regex::IsMatch(e->KeyChar.ToString(), "^([a-fA-F0-9])*$"))
				{
					e->Handled = true;
				}
			}

			//Downlink Request button
			private: System::Void button_downlinkRequest_Click(System::Object^  sender, System::EventArgs^  e) {
				if (this->comboBox_downlinkFileName->Text == String::Empty) {
					logErr("Please select a file to request for downlink.");
					return;
				}

				std::vector<uint8_t> payload;
				payload.push_back('T');
				payload.push_back('R');
				payload.push_back(0x00); //Full file transfer request
				std::string fileName = msclr::interop::marshal_as<std::string>(this->comboBox_downlinkFileName->Text);
				payload.insert(payload.end(), fileName.begin(), fileName.end());
				sendRFPacket(this->getSatelliteCallsignSSID(), payload);
			}

			//To update the downlink buttons.
			private:  void downlinkStarting(uint16_t tID) {
				if (System::Threading::Thread::CurrentThread->ManagedThreadId == this->UIThreadID) {
					//This part is necessary for automatic downlinks (started by the satellite)
					System::String ^ fileToDownlink;
					if (CommsNaSPUoN::incomingTransfers.count(tID)) {
						fileToDownlink = gcnew System::String(CommsNaSPUoN::incomingTransfers[tID].fileName.c_str());
					}
					if ( !this->comboBox_downlinkFileName->Text->Equals( fileToDownlink ) ) {
						log("AutoDL -> Satellite has started an automatic downlink.");
						//this->comboBox_downlinkFileName->Text = fileToDownlink;
					}

					//Opening the new form to show the progress of the downlink
					transferForms[tID] = gcnew DownlinkTransfer;
					transferForms[tID]->transferID = tID;
					transferForms[tID]->textBox_transferID->Text = tID.ToString();
					transferForms[tID]->textBox_filename->Text = fileToDownlink;
					transferForms[tID]->form = this;
					transferForms[tID]->Show();
				}
				else if (System::Threading::Thread::CurrentThread->ManagedThreadId == this->receiverThreadID) {
					if (backgroundWorker_Receiver->IsBusy)
						backgroundWorker_Receiver->ReportProgress(8, tID);
				}
			}

			//When the downlink is complete this function closes the downlink dialog
			private: void downlinkComplete(uint16_t tID) {
				if (System::Threading::Thread::CurrentThread->ManagedThreadId == this->UIThreadID) {
					if (transferForms->count(tID)) {
						transferForms[tID]->Close();
						transferForms->erase(tID);
					}
				}
				else if (System::Threading::Thread::CurrentThread->ManagedThreadId == this->receiverThreadID) {
					if (backgroundWorker_Receiver->IsBusy)
						backgroundWorker_Receiver->ReportProgress(5, tID);
				}
			}

			//Uplink Request Button
			private: System::Void button_uplinkRequest_Click(System::Object^  sender, System::EventArgs^  e) {
				if (this->textBox_uplinkFileName->Text == String::Empty) {
					logErr("Please select a file to request for uplink.");
					return;
				}

				this->button_uplinkCancel->Visible = true;
				this->button_uplinkCancel->Enabled = true;
				this->button_uplinkRequest->Enabled = false;
				this->textBox_uplinkFileName->Enabled = false;
				this->button_closePort->Enabled = false;

				std::string fileNameWithPath = msclr::interop::marshal_as<std::string>(this->textBox_uplinkFileName->Text);
				newOutgoingTransfer(this->getSatelliteCallsignSSID(), fileNameWithPath);
			}

			//Uplink Cancel Button
			private: System::Void button_uplinkCancel_Click(System::Object^  sender, System::EventArgs^  e) {
				cancelOutgoingTransfer(this->currentUplinkTransferID);

				std::vector<uint8_t> payload;
				payload.push_back('T');
				payload.push_back('Z');
				payload.push_back(0x00); //Delete specific transfer
				payload.push_back(0x01); //Uplink (incoming into the sat)
				insertSixteenBitIntInEightBitVector(payload, payload.end(), this->currentUplinkTransferID);
				sendRFPacket(this->getSatelliteCallsignSSID(), payload);

				uplinkComplete(false);
			}

			//When the uplink is complete this function reenables controls for starting another uplink
			private: void uplinkComplete(bool complete) {
				if (System::Threading::Thread::CurrentThread->ManagedThreadId == this->UIThreadID) {
					this->button_uplinkCancel->Enabled = false;
					this->button_uplinkCancel->Visible = false;
					this->button_uplinkRequest->Enabled = true;
					this->textBox_uplinkFileName->Enabled = true;
					this->button_closePort->Enabled = true;
					uplinkProgressBarUpdate((complete) ? 100 : 0);
				}
				else if (System::Threading::Thread::CurrentThread->ManagedThreadId == this->UplinkThreadID) {
					if (backgroundWorker_Uplink->IsBusy)
						backgroundWorker_Uplink->ReportProgress(5, complete);
				}
			}

			//When the About button is clicked
			private: System::Void button_about_Click(System::Object^  sender, System::EventArgs^  e) {
				if (about == nullptr || !about->Visible) {
					about = gcnew AboutDialog;
					about->Show();
				}
			}
};

System::Void DownlinkTransfer::DownlinkTransfer_FormClosing(System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e) {
	uint16_t tID = this->transferID;
	if (CommsNaSPUoN::incomingTransfers.count(tID)) {
		std::vector<uint8_t> payload;
		payload.push_back('T');
		payload.push_back('Z');
		payload.push_back(0x00); //Delete specific transfer
		payload.push_back(0x00); //Downlink (outgoing from the sat)
		form->insertSixteenBitIntInEightBitVector(payload, payload.end(), this->transferID);

		uint8_t ssid = form->satelliteSSID;
		System::String^ str = form->satelliteCallsign;
		std::string callsign = (msclr::interop::marshal_as<std::string>(str));
		std::vector<uint8_t> callsignSSID (callsign.begin(), callsign.end());
		callsignSSID.push_back(ssid);
		form->sendRFPacket(callsignSSID, payload);

		System::Threading::Thread::CurrentThread->Sleep(100); //Stalling to make sure the satellite is no longer sending data

		form->cancelIncomingTransfer(this->transferID);
	}

	if (form->transferForms->count(this->transferID)) {
		form->transferForms->erase(this->transferID);
	}
}

}
