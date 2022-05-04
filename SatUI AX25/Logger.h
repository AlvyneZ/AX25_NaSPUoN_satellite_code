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
	switch (localTime->tm_wday) {
	case 1: stamp += "Mon"; break;
	case 2: stamp += "Tue"; break;
	case 3: stamp += "Wed"; break;
	case 4: stamp += "Thu"; break;
	case 5: stamp += "Fri"; break;
	case 6: stamp += "Sat"; break;
	default: stamp += "Sun";
	}
	stamp += "-";
	stamp += std::to_string(localTime->tm_mday);
	stamp += "-";
	switch (localTime->tm_mon) {
	case 1: stamp += "Feb"; break;
	case 2: stamp += "March"; break;
	case 3: stamp += "Apr"; break;
	case 4: stamp += "May"; break;
	case 5: stamp += "June"; break;
	case 6: stamp += "July"; break;
	case 7: stamp += "Aug"; break;
	case 8: stamp += "Sep"; break;
	case 9: stamp += "Oct"; break;
	case 10: stamp += "Nov"; break;
	case 11: stamp += "Dec"; break;
	default: stamp += "Jan";
	}
	stamp += "-";
	stamp += std::to_string(localTime->tm_year + 1900);
	stamp += "@";
	if ((localTime->tm_hour) < 10)
		stamp += std::to_string(0);
	stamp += std::to_string(localTime->tm_hour);
	stamp += ";";
	if ((localTime->tm_min) < 10)
		stamp += std::to_string(0);
	stamp += std::to_string(localTime->tm_min);
	stamp += ";";
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
	else if (System::Threading::Thread::CurrentThread->ManagedThreadId == this->resenderThreadID) {
		if (backgroundWorker_Resender->IsBusy)
			this->backgroundWorker_Resender->ReportProgress(1, messageSys);
	}
	else if (System::Threading::Thread::CurrentThread->ManagedThreadId == this->UplinkPtRqThreadID) {
		if (backgroundWorker_UplinkPartRequest->IsBusy)
			this->backgroundWorker_UplinkPartRequest->ReportProgress(1, messageSys);
	}
	else {
		int thread = System::Threading::Thread::CurrentThread->ManagedThreadId;
		for (cliext::map<uint16_t, int>::iterator it = DownlinkThreadID->begin(); it != DownlinkThreadID->end(); it++) {
			if (thread == it->second) {
				if ((backgroundWorker_Downlink->count(it->first)) && (backgroundWorker_Downlink[it->first]->IsBusy))
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
	else if (System::Threading::Thread::CurrentThread->ManagedThreadId == this->resenderThreadID) {
		if (backgroundWorker_Resender->IsBusy)
			this->backgroundWorker_Resender->ReportProgress(2, messageSys);
	}
	else if (System::Threading::Thread::CurrentThread->ManagedThreadId == this->UplinkPtRqThreadID) {
		if (backgroundWorker_UplinkPartRequest->IsBusy)
			this->backgroundWorker_UplinkPartRequest->ReportProgress(2, messageSys);
	}
	else {
		int thread = System::Threading::Thread::CurrentThread->ManagedThreadId;
		for (cliext::map<uint16_t, int>::iterator it = DownlinkThreadID->begin(); it != DownlinkThreadID->end(); it++) {
			if (thread == it->second) {
				if ((backgroundWorker_Downlink->count(it->first)) && (backgroundWorker_Downlink[it->first]->IsBusy))
					return this->backgroundWorker_Downlink[it->first]->ReportProgress(2, messageSys);
			}
		}
	}
}

void SatUI::MyForm::logFileSave() {
	if (System::Threading::Thread::CurrentThread->ManagedThreadId == this->UIThreadID) {
		System::String^ logs = this->richTextBox_output->Text;
		std::string stdLogs = msclr::interop::marshal_as<std::string>(logs);
		int offset = 0;
		if (LOGGER::savedSize < stdLogs.size())
			offset = LOGGER::savedSize;
		std::vector<uint8_t> file(stdLogs.begin() + offset, stdLogs.end());
		log("Size of file to save: " + std::to_string(file.size()) + " And textBox size: " + std::to_string(stdLogs.length()) );

		std::string fileName = generateTimestamp() + ".txt";
		for (int i = 0; i < fileName.size(); i++) {
			if (fileName[i] == ':')
				fileName[i] = ';';
		}
		std::string fileNameWithPath = getDownlinkSatFilesLocation() + "\\lf\\" + LOG_FILE_PREFIX;
		fileNameWithPath += fileName;

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
