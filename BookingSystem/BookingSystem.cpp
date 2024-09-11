#include <iostream>
#include <fstream>
#include <string>

using namespace std;


class Parser {
private:
    string file_path;
    string file_content;
    ifstream config;
public:
    Parser(const string& file_path) : file_path(file_path), config(file_path) {
        if (!config) {
            cerr << "Can't reach the file" << endl;
            return;
        }
    };
};



int main()
{
    Parser parser("confi.txt");
    cout << "Hello World!\n";
}
