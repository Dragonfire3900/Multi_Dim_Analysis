// Standard cpp library imports
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <math.h>

#include "TPrincipal.h"
#include "TFile.h"
#include "TString.h"

using namespace std;

void readFile(TString inFile, vector<vector<double>>& DataArray, char delim) {
    DataArray.clear();

    ifstream fileIn;
    string value, line, temp;
    vector<double> row;

    fileIn.open(inFile.Data());


    while (!fileIn.eof()) {

        getline(fileIn, line);

        // std::cout << "Success" << std::endl;

        row.clear();

        stringstream s(line);

        while(getline(s, value, delim)) {
            row.push_back(stof(value));
        }

        DataArray.push_back(row);
    }
}

// std::ostream& operator<< (std::ostream& out, const TVectorT<double>& v) {
//     out << "< ";

//     size_t last = v.GetNrows() - 1;

//     for (size_t i = 0; i < v.GetNrows(); i++) {
//         out << v.;

//         if (i != last) {
//             out << ", ";
//         }
//     }

//     out << " >";
// }

template <typename T>
std::ostream& operator<<(std::ostream& out, std::vector<std::vector<T>>& v) {
    out << "[ ";

    size_t row_last = v.size() - 1;

    for (size_t i = 0; i < v.size(); i++) {
        size_t col_last = v[i].size() - 1;

        if (i != 0) {
            out << "  ";
        }
        
        for (size_t j = 0; j < v[i].size(); j++) {
            out << v[i][j];
            if (j != col_last) {
                out << ", ";
            }
        }

        if (i != row_last) {
            out << std::endl;
        }
    }

    out << " ]";
}

void usage() {
    printf("Root_principle calculates the ");
}

int main(int argc, char *argv[]) {
    TString* testFile = new TString("../Source_Data/inputs/DYB_ImprovedDE.txt");
    vector<vector<double>> Data;

    readFile(*testFile, Data, *" ");

    TPrincipal* test = new TPrincipal();
    
    for (size_t i = 0; i < Data.size(); i++) {
        Double_t x[Data[i].size()];
        for (size_t j = 0; j < Data[i].size(); j++) {
            x[j] = Data[i][j];
        }
        test -> AddRow(x);
    }

    *(test -> GetRow(1));

    // std::cout << *test -> GetEigenValues() << endl;
    test -> GetEigenValues() -> Print();
}