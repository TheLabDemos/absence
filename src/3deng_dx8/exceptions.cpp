#include "exceptions.h"

EngineInitException::EngineInitException() {
	str = "Unknown";
}

EngineInitException::EngineInitException(string reason) {
	str = reason;
}

string EngineInitException::GetReason() const {
	string tmp = "An error occured while initializing the 3D Engine\nReason: ";
	tmp += str;
	return tmp;
}

EngineModuleNotFoundException::EngineModuleNotFoundException() {
	str = "Unknown Module";
}

EngineModuleNotFoundException::EngineModuleNotFoundException(string modulename) {
	str = modulename;
}

string EngineModuleNotFoundException::GetReason() const {
	string tmp = "The 3D Engine could not link a required module.\n";
	tmp += "Please contact the program ventor to obtain the missing file.\nMissing File: ";
	tmp += str;
	return tmp;
}

EngineGeneralException::EngineGeneralException() {
	str = "Unknown";
}

EngineGeneralException::EngineGeneralException(string reason) {
	str = reason;
}

string EngineGeneralException::GetReason() const {
	string tmp = "An error occured in the 3D Engine\nReason: ";
	tmp += str;
	return tmp;
}
