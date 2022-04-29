/*
	Author: AlvyneZ

	Code for logging all output to the form's richTextBox
*/

#ifndef LOGGER_H
#define LOGGER_H



#include <ctime>
#include <string>


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
	else if (System::Threading::Thread::CurrentThread->ManagedThreadId == this->UplinkThreadID) {
		if (backgroundWorker_Uplink->IsBusy)
			this->backgroundWorker_Uplink->ReportProgress(1, messageSys);
	}
	else if (System::Threading::Thread::CurrentThread->ManagedThreadId == this->DownlinkPtRqThreadID) {
		if (backgroundWorker_DownlinkPartRequest->IsBusy)
			this->backgroundWorker_DownlinkPartRequest->ReportProgress(1, messageSys);
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
	else if (System::Threading::Thread::CurrentThread->ManagedThreadId == this->UplinkThreadID) {
		if (backgroundWorker_Uplink->IsBusy)
			this->backgroundWorker_Uplink->ReportProgress(2, messageSys);
	}
	else if (System::Threading::Thread::CurrentThread->ManagedThreadId == this->DownlinkPtRqThreadID) {
		if (backgroundWorker_DownlinkPartRequest->IsBusy)
			this->backgroundWorker_DownlinkPartRequest->ReportProgress(2, messageSys);
	}
}

void SatUI::MyForm::clearOutput() {
	this->richTextBox_output->Text = L"Output:\n";
}


#endif
