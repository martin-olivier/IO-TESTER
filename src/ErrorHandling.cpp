#include "ErrorHandling.hpp"
#include "Utils.hpp"

enum CheckStatus {Input, Output};

void ErrorHandling::Help(const char *bin, int returnValue)
{
    std::cout << "Usage: \t\t\t" << bin << " [files]" << std::endl;
    std::cout << "Help menu : \t\t" << bin << " [-h || --help]" << std::endl;
    std::cout << "Show version : \t\t" << bin << " [-v || --version]" << std::endl;
    std::cout << "Show details : \t\t" << bin << " [files] [--details]" << std::endl;
    std::cout << "Show diff on VS Code : \t" << bin << " [files] [--diff]" << std::endl;
    exit(returnValue);
}

void ErrorHandling::isInput(const std::string &line)
{
    if (line[0] != '[')
        Utils::my_exit(84, "Parsing Error at line :\n" + line);
    if (line.find(']') == std::string::npos)
        Utils::my_exit(84, "Parsing Error at line :\n" + line);
    if (line.find(']') == line.size() - 1 || line.find(']') == 1)
        Utils::my_exit(84, "Parsing Error at line :\n" + line);
}

std::vector<std::string> ErrorHandling::CheckFile(char *path)
{
    std::vector<std::string> file = Utils::string_to_vector(Utils::get_file_content(path), '\n');
    CheckStatus status = Input;

    if (file.empty())
        Utils::my_exit(84, "Error : File is Empty");
    for (const auto& line : file) {
        if (line.empty())
            continue;
        else if (status == Input) {
            isInput(line);
            status = Output;
        }
        else if (status == Output) {
            if (line == "[END]")
                status = Input;
        }
    }
    if (status == Output)
        Utils::my_exit(84, "Parsing Error at the last line");
    return file;
}