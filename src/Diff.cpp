#include "IO_Tester.hpp"
#include <unistd.h>

void IOTester::VSCodeDiff(const Test &t, const std::string &output)
{
    std::string filename1 = "\"/tmp/GOT(" + t.m_name + ")\"";
    std::string filename2 = "\"/tmp/EXPECTED(" + t.m_name + ")\"";
    std::string s1 = "echo \"" + output + "\" > " + filename1;
    std::string s2 = "echo \"" + t.m_output + "\" > " + filename2;

    system(std::string(s1 + " ; " + s2).c_str());
    system(std::string(VSCodePath + filename1 + " " + filename2).c_str());
}

bool IOTester::checkVSCodeBin()
{
#ifdef __APPLE__
    if (access("/Applications/Visual Studio Code.app/Contents/Resources/app/bin/code", X_OK) != -1)
        return true;
    return false;
#else
    char* env_p = std::getenv("PATH");
    if (env_p == NULL)
        return false;
    auto PATHList = Utils::string_to_vector(env_p, ':');

    for (auto &path : PATHList) {
        if (access((std::string(path + "/code").c_str()), X_OK) != -1)
            return true;
    }
    return false;
#endif
}