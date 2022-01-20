#include <unistd.h>

#include "io_tester.hpp"
#include "tools.hpp"

#ifdef __APPLE__
const std::string VSCodePath = "/Applications/Visual Studio Code.app/Contents/Resources/app/bin/code";
#else
const std::string VSCodePath = "code";
#endif

void io_tester::VSCodeDiff(const test &test, const std::string &output)
{
    const std::string filename1 = "\"/tmp/GOT(" + test.m_name + ")\"";
    const std::string filename2 = "\"/tmp/EXPECTED(" + test.m_name + ")\"";
    const std::string s1 = "printf \"" + output + "\" > " + filename1;
    const std::string s2 = "printf \"" + test.m_output + "\" > " + filename2;

    system(std::string(s1 + " ; " + s2).c_str());
    system(std::string("\"" + VSCodePath + "\" --diff " + filename1 + " " + filename2).c_str());
}

bool io_tester::checkVSCodeBin()
{
#ifdef __APPLE__
    if (access(VSCodePath.c_str(), X_OK) != -1)
        return true;
    return false;
#else
    char* env_p = std::getenv("PATH");
    if (env_p == NULL)
        return false;
    auto PATHList = tools::string_to_vector(env_p, ':');

    for (auto &path : PATHList) {
        if (access((std::string(path + "/code").c_str()), X_OK) != -1)
            return true;
    }
    return false;
#endif
}