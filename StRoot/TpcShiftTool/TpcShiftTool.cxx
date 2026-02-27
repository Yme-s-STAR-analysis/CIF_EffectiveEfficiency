
#include <iostream>

#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TString.h"

#include "TpcShiftTool.h"
#include "RunNumber.h"

BES2Processing::TpcShiftTool::TpcShiftTool(const std::string& energy): 
    curRun(-1),
    runMap(RunNumber::getRunMap(energy)) {
    if (!runMap) {
        std::cout << "[WARNING] TpcShiftTool: Run map for energy [" << energy << "] not found!" << std::endl;
    } else {
        std::cout << "[LOG] TpcShiftTool: Loaded run map for energy [" << energy << "] (" << runMap->size() << " runs)" << std::endl;
    }
    
    const char* file = RunNumber::getShiftFile(energy);
    if (!file || std::string(file).empty()) {
        std::cout << "[ERROR] TpcShiftTool: Shift file for energy [" << energy << "] not found!" << std::endl;
        rootFileName = "";
    } else {
        rootFileName = file;
        std::cout << "[LOG] TpcShiftTool: Shift file for energy [" << energy << "] is: " << rootFileName << std::endl;
    }
}

bool BES2Processing::TpcShiftTool::Init() {
    std::cout << "[LOG] - TpcShiftTool: The shift root file would be " << rootFileName << ".\n";
    TFile* tf = new TFile(rootFileName.c_str());
    if (!tf){
        std::cout << "[WARNING] - TpcShiftTool: File cannot open.\n";
        return false;
    }
    if (tf->IsZombie()) {
        std::cout << "[WARNING] - TpcShiftTool: Zombie file.\n";
        return false;
    }
    tf->Close();
    return true;
}

int BES2Processing::TpcShiftTool::GetPtBin(double pT) {
    if (pT < 0.0) { return -1; }
    else if (pT < 0.3) { return 0; }
    else if (pT < 0.5) { return 1; }
    else if (pT < 0.7) { return 2; }
    else if (pT < 0.9) { return 3; }
    else if (pT < 1.1) { return 4; }
    else if (pT < 1.3) { return 5; }
    else if (pT < 1.6) { return 6; }
    else if (pT < 2.5) { return 7; }
    else { return -1; }
}

int BES2Processing::TpcShiftTool::GetEtaBin(double eta) {
    if (eta < -2.0) { return -1; }
    else if (eta < -1.6) { return 0; }
    else if (eta < -1.3) { return 1; }
    else if (eta < -1.0) { return 2; }
    else if (eta < -0.8) { return 3; }
    else if (eta < -0.6) { return 4; }
    else if (eta < -0.4) { return 5; }
    else if (eta < -0.2) { return 6; }
    else if (eta < +0.0) { return 7; }
    else if (eta < +0.2) { return 8; }
    else if (eta < +0.4) { return 9; }
    else if (eta < +0.6) { return 10; }
    else if (eta < +0.8) { return 11; }
    else if (eta < +1.0) { return 12; }
    else if (eta < +1.3) { return 13; }
    else if (eta < +1.6) { return 14; }
    else if (eta < +2.0) { return 15; }
    else { return -1; }
}

int BES2Processing::TpcShiftTool::GetFinalBin(double pT, double eta) {
    if (pT > 2.5 || pT < 0.0 || eta > 2.0 || eta < -2.0) {
        return -1;
    }
    int ptBin = GetPtBin(pT);
    int etaBin = GetEtaBin(eta);
    return (ptBin < 0 || etaBin < 0) ? -1 : ptBin + etaBin*8;
}

double BES2Processing::TpcShiftTool::GetShift(int runId, double pT, double eta) {
    if (runId != curRun) {
        TFile* tf = new TFile(rootFileName.c_str());
        curRun = runId;
        auto it = runMap->find(runId);
        if (it == runMap->end()) { return -999; }
        int cvtId = it->second;
        std::cout << "[LOG] - TpcShiftTool: New Run ID found, now reading shift map for run number " << runId << " (" << cvtId << ")\n";
        runDepShift = (TH1F*)(tf->Get(Form("NSGP_shift_runid%d", cvtId))->Clone());
    }
    int finalBin = GetFinalBin(pT, eta);
    return finalBin < 0 ? 0 : runDepShift->GetBinContent(finalBin+1);
}
