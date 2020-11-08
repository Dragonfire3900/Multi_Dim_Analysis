#include <math.h>
#include "Math/Minimizer.h"
#include "Math/Factory.h"
#include "Math/Functor.h"
#include "TColor.h"
#include "TMarker.h"
#include <iostream>

#include "../../Global_Fits/GlobalAnalyzer.hh"
#include "/home/dragon/progs/builds/cpp_progress_bar/progress_bar.hpp"

using namespace std;

static const double sigma235=6.69;
static const double sigma238=10.10;
static const double sigma239=4.40;
static const double sigma241=6.03;

void usage() {
    printf("Histo_calc calculates the 2D histograms of a specific set of input files and then saves them as root files");
    printf("Example: ./Histo_calc indir outdir x_pt_num y_pt_num source_names\n");
    printf("indir: Where the sources are held\n"
        "outdir: where to output the root files\n"
        "fitType: The fitType for the minimizations\n"
        "x_pt_num: number of points on the x axis\n"
        "y_pt_num: number of points on the y axis\n"
        "source_names: A space seperated list of the source experiments\n");
    //printf("Fit type should be a number from 1-11:\n 1 = U235 only \n 2 = P239 only \n 3 = U235+239 fit \n 4 = U235+239+238 fit \n 5 = OSC only \n 6 = 235+OSC only \n 7 = 239+OSC only  \n 8 = Eq \n 9 = 5+Eq \n 10 = 9+Eq \n 11 = linear fit to 239 data \n");

    exit(1);
}

double calcSep(const double lower, const double upper, int pt_num) {
    return (upper - lower) / (pt_num - 1);
}

int main(int argc, char *argv[]) {
    //Main Config
    const double x_range[] = {pow(10, -2), 1};
    const double y_range[] = {pow(10, -1), pow(10, 1)};

    double step[6] = {0.0001,0.0001,0.0001,0.0001,0.0001,0.0001};
    double variable[6] = {sigma235,sigma238,sigma239,sigma241,0,0.48};
    double minRange[6]={0.0,0.0,0.0,0.0,0.0,0.0};
    double maxRange[6]={40,40,40,40,1,4};

    TString prefix_covStat = "_covstat.txt";
    TString prefix_covSyst = "_covsyst.txt";

    //Define & Calculate User inputs
    if (argc < 6) {
        usage();
    }

    TString indir = argv[1];
    TString outDir = argv[2];

    outDir = outDir + "/";
    indir = indir + "/";

    int fitType = stoi(argv[3]);
    const int source_num = argc - 6;

    int pt_nums[2] = {stoi(argv[4]), stoi(argv[5])};
    
    TString* sources[source_num];
    for (int i = 6; i < argc; i++) {
        sources[i - 6] = new TString(argv[i]);
    }

    ProgressBar prog_bar(pt_nums[0] * source_num, "Percentage done:");

    //Main Loop
    printf("Using git commit 73e465240018f164bd51222c5d3798aaa497992c \n");
    printf("Fit type: %i \n",fitType);

    double x_sep = calcSep(x_range[0], x_range[1], pt_nums[0]);
    double y_sep = calcSep(y_range[0], y_range[1], pt_nums[1]);

    for (int i = 0; i < source_num; i++) {
        //Printing basic information
        printf("Output file name: %s \n", (outDir + sources[i] -> Data() + ".pdf").Data());

        //Opening file to write to and making graph
        TFile* curr_File = new TFile(outDir + sources[i] -> Data() + ".root", "RECREATE");

        TH2F* outGraph = new TH2F(
            /* name */ "s22t-dm2",
            /* title */ sources[i] -> Data(),
            /* X-dims */ pt_nums[0], x_range[0], x_range[1],
            /* Y-dims */ pt_nums[1], y_range[0], y_range[1]
        );

        //Setup the analyzer function using the experimental data
        GlobalAnalyzer* globalAnalyzer = new GlobalAnalyzer();
        globalAnalyzer -> InitializeAnalyzer(indir + sources[i] -> Data() + ".txt", indir + sources[i] -> Data() + prefix_covStat, indir + sources[i] -> Data() + prefix_covSyst);
        globalAnalyzer -> SetupExperiments(fitType);

        //Setting up the minimizer
        ROOT::Math::Minimizer* minimum = ROOT::Math::Factory::CreateMinimizer("Minuit2", "");
        minimum -> SetMaxFunctionCalls(100000);
        minimum -> SetTolerance(0.0001);
        minimum -> SetPrintLevel(-1); //Sets the debug print

        minimum -> SetFunction(*globalAnalyzer);

        //Set which variables are free
        minimum->SetVariable(0,"U235",variable[0], step[0]);
        minimum->SetVariable(1,"U238",variable[1], step[1]);
        minimum->SetVariable(2,"P239",variable[2], step[2]);
        minimum->SetVariable(3,"P241",variable[3], step[3]);
        minimum->SetVariable(4,"s22t",variable[4], step[4]);
        minimum->SetVariable(5,"dm2",variable[5], step[5]);
  
        minimum->SetVariableLimits(0,minRange[0],maxRange[0]);
        minimum->SetVariableLimits(1,minRange[1],maxRange[1]);
        minimum->SetVariableLimits(2,minRange[2],maxRange[2]);
        minimum->SetVariableLimits(3,minRange[3],maxRange[3]);
        minimum->SetVariableLimits(4,minRange[4],maxRange[4]);
        minimum->SetVariableLimits(5,minRange[5],maxRange[5]);
        if(fitType>4&& fitType<8)minimum->FixVariable(5);

        // Do the actual minimization
        minimum->Minimize();
        if(fitType>4&& fitType<8){
            minimum->ReleaseVariable(5);
            minimum->Minimize();
        }

        //Prepping minimums for storage
        const double *xs = minimum->X();
        double min = minimum -> MinValue();
        TVectorD min_vals(7);

        min_vals[0] = xs[0];
        min_vals[1] = xs[1];
        min_vals[2] = xs[2];
        min_vals[3] = xs[3];
        min_vals[4] = xs[4];
        min_vals[5] = xs[5];

        min_vals[6] = minimum -> MinValue();

        printf("\rEvaluating for source set: %s\n", sources[i] -> Data());

        for (int xi = 0; xi < pt_nums[0]; xi++) {
            for (int yi = 0; yi < pt_nums[1]; yi++) {
                double test_vals[] = {xs[0], xs[1], xs[2], xs[3], xi * x_sep + x_range[0], yi * y_sep + y_range[0]};
                outGraph -> Fill(xi * x_sep + x_range[0], yi * y_sep + y_range[0], globalAnalyzer -> DoEval(test_vals) - min_vals[6]);
            }
            prog_bar.Progressed(xi + pt_nums[0] * i);
        }

        printf("\nSaving file at: %s\n\n", (outDir + sources[i] -> Data() + ".root").Data());

        outGraph -> Write();
        min_vals.Write("minValues");

        curr_File -> Close();
        // delete outGraph, curr_File, min_vals, globalAnalyzer, minimum;
    }
}