#include <iostream>
#include <cstdlib>
#include <fstream>

using namespace std;

int test_words_path_program(int, char** argv)
{
    const char *sources_path = argv[1];

    char cmd[256];
    snprintf(cmd, sizeof cmd,
            "./words_path %1$s/inwords.txt %1$s/vocab.txt > testout.txt",
             sources_path);
    std::system (cmd);

    std::ifstream outwords(string(sources_path)+"/outwords.txt");
    std::ifstream testout ("testout.txt");
    if (!outwords.good() || !testout.good())
    {
        cout << "ERROR: can't open input files" << endl;
        return 1;
    }

    std::string file1{istreambuf_iterator<char>(outwords),{}};
    std::string file2{istreambuf_iterator<char>(testout), {}};

    if (!(file1 == file2))
    {
        cout << "ERROR: files don't match" << endl;
        return 1;
    }

    return 0;
}
