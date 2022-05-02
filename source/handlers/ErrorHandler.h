#pragma once

class ErrorHandler
{
public:
    inline static auto &Instance()
    {
        static ErrorHandler instance;
        return instance;
    }

    static void Initialize();
    static void SetupCallbacks();

private:
    ErrorHandler() {}
    ErrorHandler(ErrorHandler const &);
    void operator=(ErrorHandler const &);

    void ErrorCallback(int error_code, const char *description);
};
