/*
	Author: AlvyneZ

	Code for logging all output to the form's richTextBox
*/

#ifndef LOGGER_H
#define LOGGER_H



#include <ctime>
#include <fstream>
#include <string>

#define LOG_FILE_PREFIX "log_"


namespace LOGGER {
	int savedSize = 0;
}


std::string generateTimestamp() {
	//A function for creating the timestamp for logs
	time_t now = time(0);
	time_t *timeNow = &now;
	tm *localTime = localtime(timeNow);
	std::string stamp = "";
	if ((localTime->tm_hour) < 10)
		stamp += std::to_string(0);
	stamp += std::to_string(localTime->tm_hour);
	stamp += ":";
	if ((localTime->tm_min) < 10)
		stamp += std::to_string(0);
	stamp += std::to_string(localTime->tm_min);
	stamp += ":";
	if ((localTime->tm_sec) < 10)
		stamp += std::to_string(0);
	stamp += std::to_string(localTime->tm_sec);
	return stamp;
}


void SatUI::MyForm::log(std::string message) {
	System::String^ messageSys = gcnew System::String(message.c_str());
	if (System::Threading::Thread::CurrentThread->ManagedThreadId == this->UIThreadID) {
		System::String^ time_stamp = gcnew System::String(generateTimestamp().c_str());
		System::String^ out = L"\n" + time_stamp + L" -> " + messageSys + L"\n";
		this->richTextBox_output->Text += out;
	}
	else if (System::Threading::Thread::CurrentThread->ManagedThreadId == this->receiverThreadID) {
		if (backgroundWorker_Receiver->IsBusy)
			this->backgroundWorker_Receiver->ReportProgress(1, messageSys);
	}
	else if (System::Threading::Thread::CurrentThread->ManagedThreadId == this->UplinkPtRqThreadID) {
		if (backgroundWorker_UplinkPartRequest->IsBusy)
			this->backgroundWorker_UplinkPartRequest->ReportProgress(1, messageSys);
	}
	else {
		int thread = System::Threading::Thread::CurrentThread->ManagedThreadId;
		for (cliext::map<uint16_t, int>::iterator it = DownlinkThreadID->begin(); it != DownlinkThreadID->end(); it++) {
			if (thread == it->second) {
				if (backgroundWorker_Downlink[it->first]->IsBusy)
					return this->backgroundWorker_Downlink[it->first]->ReportProgress(1, messageSys);
			}
		}
	}
}

void SatUI::MyForm::logErr(std::string message) {
	System::String^ messageSys = gcnew System::String(message.c_str());
	if (System::Threading::Thread::CurrentThread->ManagedThreadId == this->UIThreadID) {
		System::String^ time_stamp = gcnew System::String(generateTimestamp().c_str());
		System::String^ out = L"\n*****ERROR*****\n" + time_stamp + L" -> " + messageSys + L"\n*******************\n";
		this->richTextBox_output->Text += out;
	}
	else if (System::Threading::Thread::CurrentThread->ManagedThreadId == this->receiverThreadID) {
		if (backgroundWorker_Receiver->IsBusy)
			this->backgroundWorker_Receiver->ReportProgress(2, messageSys);
	}
	else if (System::Threading::Thread::CurrentThread->ManagedThreadId == this->UplinkPtRqThreadID) {
		if (backgroundWorker_UplinkPartRequest->IsBusy)
			this->backgroundWorker_UplinkPartRequest->ReportProgress(2, messageSys);
	}
	else {
		int thread = System::Threading::Thread::CurrentThread->ManagedThreadId;
		for (cliext::map<uint16_t, int>::iterator it = DownlinkThreadID->begin(); it != DownlinkThreadID->end(); it++) {
			if (thread == it->second) {
				if (backgroundWorker_Downlink[it->first]->IsBusy)
					return this->backgroundWorker_Downlink[it->first]->ReportProgress(2, messageSys);
			}
		}
	}
}

void SatUI::MyForm::logFileSave() {
	if (System::Threading::Thread::CurrentThread->ManagedThreadId == this->UIThreadID) {
		System::String^ logs = this->richTextBox_output->Text;
		std::string stdLogs = msclr::interop::marshal_as<std::string>(logs);
		std::vector<uint8_t> file(stdLogs.begin() + LOGGER::savedSize, stdLogs.end());

		std::string fileNameWithPath = getDownlinkSatFilesLocation() + "\\lf\\" + LOG_FILE_PREFIX;
		fileNameWithPath += generateTimestamp() + ".txt";

		if (checkIfFileExists(fileNameWithPath)) {
			logErr("The file specified for output '" + fileNameWithPath + "' already exists.");
			return;
		}
		log("Beginning Logs output to '" + fileNameWithPath + "'.");
		std::ofstream output_file;
		output_file.open(fileNameWithPath.c_str(), std::ios::binary | std::ios::out | std::ios::app);
		for (unsigned int i = 0; i < file.size(); i++) {
			output_file << file[i];
		}
		log("Logs Output to '" + fileNameWithPath + "' Complete.");
		output_file.close();
		LOGGER::savedSize = stdLogs.size();
	}
	else if (System::Threading::Thread::CurrentThread->ManagedThreadId == this->receiverThreadID) {
		if (backgroundWorker_Receiver->IsBusy)
			this->backgroundWorker_Receiver->ReportProgress(9);
	}
}

void SatUI::MyForm::clearOutput() {
	this->richTextBox_output->Text = L"Output:\n";
}


#endif
