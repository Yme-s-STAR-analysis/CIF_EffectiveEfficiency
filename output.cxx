#include <map>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>

#include "TFile.h"
#include "TProfile.h"
#include "TString.h"

void output() {
    // std::string fNameIn = "7GeV.hadd.root";
    std::string fNameIn = "01.fDst.root";
    std::string energy = "7";
    std::string fNameOutPro = "ProEff." + energy + ".csv";
    std::string fNameOutPbar = "PbarEff." + energy + ".csv";

    // open root files
    auto fIn = TFile::Open(fNameIn.c_str(), "read");
    std::vector<std::string> sysTags;
    std::map<std::string, TProfile*> tpros;
    std::map<std::string, TProfile*> tpbars;

    if (energy == "27") {
        sysTags = {"default", "dca0p8", "dca0p9", "dca1p1", "dca1p2", "nhit12", "nhit17", "mass21", "mass22", "mass23", "mass24", "nsig1p6", "nsig1p8", "nsig2p2", "nsig2p5"};
	} else {
		sysTags = {"default", "dca0p8", "dca0p9", "dca1p1", "dca1p2", "nhit15", "nhit18", "nhit22", "nhit25", "mass21", "mass22", "mass23", "mass24", "nsig1p6", "nsig1p8", "nsig2p2", "nsig2p5"};
	}

    // read TProfiles 
    int nRapidity = 0;
    for (const auto& cut : sysTags) {
        auto tpro = (TProfile*)(fIn->Get(Form("hPro_%s", cut.c_str()))->Clone());
        auto tpbar = (TProfile*)(fIn->Get(Form("hPbar_%s", cut.c_str()))->Clone());
        tpros[cut] = tpro;
        tpbars[cut] = tpbar;
    }
    nRapidity = tpros["default"]->GetNbinsX();
    std::cout << "Number of rapidity bins: " << nRapidity << std::endl;
    std::cout << "Number of systematic cuts: " << sysTags.size() << std::endl;

    // combine efficiencies
    std::vector<double> rapidityBins;
    std::map<std::string, std::vector<double>> inveffpros; // key 1: systematic tag, order 2: rapidity bin
    std::map<std::string, std::vector<double>> inveffpbars; // -> N / sum(1/eff) for the very rapidity bin
    std::map<std::string, std::vector<double>> entrypros; // each differential bin, like 0.1 -> 0.2
    std::map<std::string, std::vector<double>> entrypbars;
    std::map<std::string, std::vector<double>> acumuinveffpros;  // accumulative bin, like 0 -> 0.2 = 0 -> 0.1 + 0.1 -> 0.2
    std::map<std::string, std::vector<double>> acumuinveffpbars;
    std::map<std::string, std::vector<double>> acumuentrypros; 
    std::map<std::string, std::vector<double>> acumuentrypbars;
    std::map<std::string, std::vector<double>> effeffpros; // final effective efficiency that will be used later
    std::map<std::string, std::vector<double>> effeffpbars;
    for (const auto& cut : sysTags) {
        inveffpros[cut] = std::vector<double>();
        inveffpbars[cut] = std::vector<double>();
        entrypros[cut] = std::vector<double>();
        entrypbars[cut] = std::vector<double>();
        acumuinveffpros[cut] = std::vector<double>();
        acumuinveffpbars[cut] = std::vector<double>();
        acumuentrypros[cut] = std::vector<double>();
        acumuentrypbars[cut] = std::vector<double>();
    }
    for (int i=0; i<nRapidity; i++) {
        rapidityBins.push_back(tpros["default"]->GetBinLowEdge(i+2));
        for (const auto& cut : sysTags) {
            inveffpros[cut].push_back(tpros[cut]->GetBinContent(i+1) * tpros[cut]->GetBinEntries(i+1)); 
            entrypros[cut].push_back(tpros[cut]->GetBinEntries(i+1) * 1.0); 
            inveffpbars[cut].push_back(tpbars[cut]->GetBinContent(i+1) * tpbars[cut]->GetBinEntries(i+1)); 
            entrypbars[cut].push_back(tpbars[cut]->GetBinEntries(i+1) * 1.0); 
            if (i == 0) { // first bin, diff. = acumu.
                acumuentrypros[cut].push_back(entrypros[cut].back());
                acumuentrypbars[cut].push_back(entrypbars[cut].back());
                acumuinveffpros[cut].push_back(inveffpros[cut].back());
                acumuinveffpbars[cut].push_back(inveffpbars[cut].back());
            } else { // other bins, diff. + last one = acumu.
                acumuentrypros[cut].push_back(acumuentrypros[cut].back() + entrypros[cut].back());
                acumuentrypbars[cut].push_back(acumuentrypbars[cut].back() + entrypbars[cut].back());
                acumuinveffpros[cut].push_back(acumuinveffpros[cut].back() + inveffpros[cut].back());
                acumuinveffpbars[cut].push_back(acumuinveffpbars[cut].back() + inveffpbars[cut].back());
            }
            effeffpros[cut].push_back(acumuentrypros[cut].back() / acumuinveffpros[cut].back());
            effeffpbars[cut].push_back(acumuentrypbars[cut].back() / acumuinveffpbars[cut].back());
        }
    }

    // 
    std::ofstream fOutPro;
    fOutPro.open(fNameOutPro.c_str());
    fOutPro << "rapidity,";
    for (const auto& cut : sysTags) {
        fOutPro << cut.c_str();
        if (cut != sysTags[sysTags.size()-1]) { fOutPro << ","; }
    }
    fOutPro << "\n";
    for (int i=0; i<nRapidity; i++) {
        fOutPro << rapidityBins[i] << ",";
        for (const auto& cut : sysTags) {
            fOutPro << effeffpros[cut][i];
            if (cut != sysTags[sysTags.size()-1]) { fOutPro << ","; }
        }
        if (i != nRapidity) { fOutPro << "\n"; }
    }
    fOutPro.close();

    std::ofstream fOutPbar;
    fOutPbar.open(fNameOutPbar.c_str());
    fOutPbar << "rapidity,";
    for (const auto& cut : sysTags) {
        fOutPbar << cut.c_str();
        if (cut != sysTags[sysTags.size()-1]) { fOutPbar << ","; }
    }
    fOutPbar << "\n";
    for (int i=0; i<nRapidity; i++) {
        fOutPbar << rapidityBins[i] << ",";
        for (const auto& cut : sysTags) {
            fOutPbar << effeffpbars[cut][i];
            if (cut != sysTags[sysTags.size()-1]) { fOutPbar << ","; }
        }
        if (i != nRapidity) { fOutPbar << "\n"; }
    }
    fOutPbar.close();

}