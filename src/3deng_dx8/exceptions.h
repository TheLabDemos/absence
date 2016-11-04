#ifndef _EXCEPTIONS_H_
#define _EXCEPTIONS_H_

#include <string>

using std::string;

class EngineBaseException {
protected:
	string str;

public:

	virtual string GetReason() const = 0;
};

class EngineInitException : protected EngineBaseException {
public:
	EngineInitException();
	EngineInitException(string reason);

	virtual string GetReason() const;
};

class EngineModuleNotFoundException : protected EngineBaseException {
public:
	EngineModuleNotFoundException();
	EngineModuleNotFoundException(string modulename);

	virtual string GetReason() const;
};

class EngineGeneralException : protected EngineBaseException {
public:
	EngineGeneralException();
	EngineGeneralException(string reason);

	virtual string GetReason() const;
};

#endif	// _EXCEPTIONS_H_