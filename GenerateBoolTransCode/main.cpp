#include <cstdlib>
#include <iostream>
#include <fstream>

using namespace std;

int main() 
{
    unsigned int amnt=0;
    cin >> amnt;
    ofstream outf("outputCode");
    
    outf << "make_tuple(";
    for(unsigned int i=0; i<amnt-1; i++)
        outf << "(wantedData[" << i << "] ? get<" << i << ">(data) : NULL),";
    outf << "(wantedData[" << amnt-1 << "] ? get<" << amnt-1 << ">(data) : NULL));\n\n";
    
    for(unsigned int i=0; i<amnt; i++)
        outf << "get<" << i << ">(empty)=NULL;\n";
    outf << "\n\n";
    
    for(unsigned int i=0; i<amnt; i++)
        outf << "case " << i << ":\n    return helper2(get<" << i << ">(data), data, condInd);\n";
    outf << "\n\n";
    
    for(unsigned int i=0; i<amnt; i++)
        outf << "case " << i << ":\n    return helper3(*datapoint,*(get<" << i << ">(data)),condInd);\n";
    outf << "\n\n";
    outf.close();
    return 0;
}

