#include "MyForm.h"

#include "Logger.h"
#include "fileOperations.h"
#include "AX25Communication.h"
#include "bitManipulation.h"
#include "CommunicationsHandler.h"

using namespace System;
using namespace System::Windows::Forms;

[STAThreadAttribute]
int main(array < String^ > ^ args) {
	FreeConsole();
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false);
	SatUI::MyForm form;

	Application::Run(%form);
	return 0;
}