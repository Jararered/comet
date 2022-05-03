#pragma once

class ErrorHandler
{
public:
    ErrorHandler();
    ~ErrorHandler();

    void SetupCallbacks();

private:
    void ErrorCallback(int error_code, const char *description);
};
