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

void readFile(TString inFile, vector<vector<double>> DataArray, char delim) {
    DataArray.clear();

    ifstream fileIn;
    string value, line, temp;
    vector<double> row;

    fileIn.open(inFile.Data());


    while (fileIn >> temp) {
        row.clear();

        getline(fileIn, line);

        stringstream s(line);

        while(getline(s, value, delim)) {
            row.push_back(stof(value));
        }

        DataArray.push_back(row);
    }
}

std::ostream& operator<<(std::ostream &out, std::vector<std::vector<double>> const&v) {
    for (auto &&i: v) {
        for (auto &&j : i) out << j << " ";
        out << std::endl;
    }
}

void usage() {
    printf("Root_principle calculates the ");
}

int main(int argc, char *argv[]) {
    TString* testFile = new TString("/home/dragon/Research/LJ/MDA/Source_Data/inputs/DYB_ImprovedDE.txt");
    char* delim = " ";
    vector<vector<double>> Data;

    readFile(*testFile, Data, *delim);

    std::cout << "Success" << std::endl;
    // std::cout << Data << std::endl;
}