#include "IO_Tester.hpp"
#include "Utils.hpp"
#include <cstring>

void IOTester::Version()
{
    std::cout << "IO Tester (" << VERSION << ")" << std::endl;
    std::cout << "Written by Martin OLIVIER, Student at EPITECH Paris" << std::endl;
    exit(0);
}

IOTester::IOTester(int ac, char **av) :
    m_passed(0), m_failed(0), m_crashed(0), m_position(0), m_details(false)
{
    if (ac < 2)
        ErrorHandling::Help(av[0], 84);
    if (strcmp(av[ac - 1], "-h") == 0 || strcmp(av[ac - 1], "--help") == 0)
        ErrorHandling::Help(av[0], 0);
    if (strcmp(av[ac - 1], "-v") == 0 || strcmp(av[ac - 1], "--version") == 0)
        IOTester::Version();
    if (ac > 2 && (strcmp(av[ac - 1], "-d") == 0 || strcmp(av[ac - 1], "--details") == 0)) {
        m_details = true;
        ac--;
    }
    for (int i = 1; i < ac; i++) {
        if (i != 1)
            std::cout << std::endl;
        if (ac > 2)
            std::cout << CYN << av[i] << ":" << RESET << std::endl << std::endl;
        m_file = ErrorHandling::CheckFile(av[i]);
        apply();
        resetValues();
    }
}

void IOTester::resetValues()
{
    m_position = 0;
    m_passed = 0;
    m_failed = 0;
    m_crashed = 0;
}

void IOTester::apply()
{
    while (true) {
        comparator(getTestData());
        while (true) {
            if (m_position >= m_file.size()) {
                printFinalResults();
                return;
            }
            else if (m_file[m_position].empty())
                m_position++;
            else
                break;
        }
    }
}

Test IOTester::getTestData()
{
    Test t;
    std::string line = m_file[m_position];
    size_t pos = 1;
    for (; pos < line.size(); pos++) {
        if (line[pos] == ']')
            break;
        else
            t.m_name += line[pos];
    }
    pos++;
    for (; pos < line.size(); pos++)
        t.m_cmd += line[pos];
    m_position++;
    for (; m_file[m_position] != "[END]"; m_position++)
        t.m_output += m_file[m_position] + '\n';
    m_position++;
    t.m_output.pop_back();
    return t;
}

void IOTester::comparator(Test t)
{
    Utils::CMD c = Utils::get_cmd_output(t.m_cmd);

    if (c.error)
        Utils::my_exit(84, "Pipe Error, Exiting...");
    else if ((c.returnValue >= 8 && c.returnValue <= 11) || (c.returnValue >= 132 && c.returnValue <= 139))
        t.m_status = Test::CRASH;
    else if ((WEXITSTATUS(c.returnValue) >= 8 && WEXITSTATUS(c.returnValue) <= 11) || (WEXITSTATUS(c.returnValue) >= 132 && WEXITSTATUS(c.returnValue) <= 139))
        t.m_status = Test::CRASH;
    else
        t.m_status = Test::PASS;

    if (t.m_status == Test::CRASH) {
        std::cout << YEL << "[SF]" << RESET << " > " << t.m_name << std::endl;
        m_crashed++;
    }
    else if (t.m_output == c.output) {
        std::cout << GRN << "[OK]" << RESET << " > " << t.m_name << std::endl;
        m_passed++;
    } else {
        std::cout << RED << "[KO]" << RESET << " > " << t.m_name << std::endl;
        m_failed++;
        if (0 /* if --diff flag */) {
            std::string filename1 = "\"/tmp/io_tester_got_" + t.m_name + "\"";
            std::string filename2 = "\"/tmp/io_tester_expected_" + t.m_name + "\"";
            std::string s1 = "echo \"" + c.output + "\" > " + filename1;
            std::string s2 = "echo \"" + t.m_output + "\" " + filename2;
            std::string command = "vimdiff " + filename1 + " " + filename2;

            system(s1.c_str());
            system(s2.c_str());
            system(command.c_str());
            command = "rm " + filename1 + " " + filename2;
            system(command.c_str());
        } else if (m_details) {
            std::cout << BLU << "[REAL OUTPUT] :" << RESET << std::endl << c.output << std::endl << BLU << "[EXPECTED OUTPUT] :" << RESET << std::endl << t.m_output << std::endl;
        }
    }
}

void IOTester::printFinalResults() const
{
    std::cout << std::endl << "> Synthesis: Tested " << BLU << m_crashed + m_passed + m_failed << RESET;
    std::cout << " | Passing: " << GRN << m_passed << RESET;
    std::cout << " | Failing: " << RED << m_failed << RESET;
    std::cout << " | Crashing: " << YEL << m_crashed << RESET << std::endl;
}

int main(int ac, char **av)
{
    IOTester(ac, av);
    return EXIT_SUCCESS;
}