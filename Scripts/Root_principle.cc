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

template <size_t rows, size_t cols>

void readFile(TString inFile, double (&DataArray)[rows][cols]) {
    double x;
    string lineA;
    ifstream fileIn;
    fileIn.open(inFile.Data());
    int columnsA, RowsA;
    RowsA = 0;

    while(fileIn.good()) {
        while(getline(fileIn, lineA)){
            istringstream streamA(lineA);
            columnsA = 0;
            while(streamA >>x){
                DataArray[RowsA][columnsA] = x;
                columnsA++;
            }
        RowsA++;
        }
    }
}

void usage() {
    printf("Root_principle calculates the ");
}

int main(int argc, char *argv[]) {
    
}