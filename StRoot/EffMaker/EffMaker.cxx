#include "EffMaker.h"
#include "TH1D.h"
#include "TH2F.h"
#include "TFile.h"
#include "TF1.h"
#include "TString.h"
#include <algorithm>
#include <iostream>
#include <cstring>
#include <string>

bool EffMaker::Init(std::string energy, std::string sysTag) {
    this->energy = energy;
    std::vector<std::string> validEnergy = {
        "7", "9", "11", "14", "17", "19", "27"
    };
    if (std::count(validEnergy.begin(), validEnergy.end(), energy) == 0) {
        std::cout << "[ERROR] - From EffMaker Module: Initializaion encountered with an invalid energy " << energy << "." << std::endl;
        return false;
    } 
    std::cout << "[LOG] - From EffMaker Module: Initialize EffMaker with energy " << energy << "." << std::endl;
    ReadInEffFile(
        Form("/star/u/yghuang/Work/DataAnalysis/BES2/OverAll/4EmbedList/x4EffFiles/CIF.TPC.EFF.%s.root", energy.c_str()),
        Form("/star/u/yghuang/Work/DataAnalysis/BES2/OverAll/4EmbedList/x4EffFiles/CIF.TOF.EFF.%s.root", energy.c_str()),
        Form("/star/u/yghuang/Work/DataAnalysis/BES2/OverAll/4EmbedList/x4EffFiles/CIF.PID.EFF.%s.root", energy.c_str()),
        sysTag
    );
    return true;
}

void EffMaker::ReadInEffFile(const char* tpc, const char* tof, const char* pid, std::string sysTag) {
    std::string sysTagReal;
    if (sysTag.compare(0, 3, "dca") == 0 || sysTag.compare(0, 4, "nhit") == 0) {
        sysTagReal = sysTag;
    } else {
        sysTagReal = "default";
    }
    std::cout << "[LOG] - From EffMaker Module: Will use [" << sysTagReal << "] as the systematic tag." << std::endl;
    std::string nSigTag;
    if (sysTag.find("nsig") == 0) { 
        nSigTag = sysTag.substr(4);
    } else {
        nSigTag = "2p0";
    }
    std::cout << "[LOG] - From EffMaker Module: Will use [" << nSigTag << "] as the nSigma proton tag." << std::endl;

    TFile* tf_tpc = 0;
    TFile* tf_tof = 0;
    TFile* tf_pid = 0;
    if (strcmp(tpc, "none")) { // true for IS DIFFERENT
	    std::cout << "[LOG] - From EffMaker Module: TPC Efficiency root file path: " << tpc << ".\n";
        tf_tpc = TFile::Open(tpc);
        tpcOff = false;
    } else {
	    std::cout << "[LOG] - From EffMaker Module: TPC Efficiency OFF.\n";
        tpcOff = true;
    }
    if (strcmp(tof, "none")) { // true for IS DIFFERENT
	    std::cout << "[LOG] - From EffMaker Module: TOF Efficiency root file path: " << tof << ".\n";
        tf_tof = TFile::Open(tof);
        tofOff = false;
    } else {
	    std::cout << "[LOG] - From EffMaker Module: TOF Efficiency OFF.\n";
        tofOff = true;
    }
    if (strcmp(pid, "none")) { // true for IS DIFFERENT
        std::cout << "[LOG] - From EffMaker Module: PID Efficiency root file path: " << pid << " with nSigma Tag: " << nSigTag << ".\n";
        tf_pid = TFile::Open(pid);
        pidOff = false;
    } else {
	    std::cout << "[LOG] - From EffMaker Module: PID Efficiency OFF.\n";
        pidOff = true;
    }

    for (int iVz=0; iVz<nVz; iVz++) {
        for (int iCent=0; iCent<nCent; iCent++) {
            if (energy == "27") { if (iVz > 2) { continue; } }
            if (!tpcOff) {
                if (energy != "9.2") {
                    tf_tpc->GetObject(
                        Form("TpcEff_cent%d_vz%d_Pro_%s", iCent, iVz, sysTagReal.c_str()),
                        tpc_pro[iCent][iVz][0]
                    );
                    tf_tpc->GetObject(
                        Form("TpcEff_cent%d_vz%d_Pbar_%s", iCent, iVz, sysTagReal.c_str()),
                        tpc_pbar[iCent][iVz][0]
                    );

                } else { // for 9.2 GeV, we have 2 regions using different TPC tracking efficiency
                    tf_tpc->GetObject(
                        Form("TpcEff_cent%d_vz%d_Pro_%s_0", iCent, iVz, sysTagReal.c_str()),
                        tpc_pbar[iCent][iVz][0]
                    );
                    tf_tpc->GetObject(
                        Form("TpcEff_cent%d_vz%d_Pbar_%s_0", iCent, iVz, sysTagReal.c_str()),
                        tpc_pbar[iCent][iVz][0]
                    );
                    tf_tpc->GetObject(
                        Form("TpcEff_cent%d_vz%d_Pro_%s_1", iCent, iVz, sysTagReal.c_str()),
                        tpc_pro[iCent][iVz][1]
                    );
                    tf_tpc->GetObject(
                        Form("TpcEff_cent%d_vz%d_Pbar_%s_1", iCent, iVz, sysTagReal.c_str()),
                        tpc_pbar[iCent][iVz][1]
                    );
                }
            }
            if (!tofOff) {
                tf_tof->GetObject(
                    Form("TofEff_cent%d_vz%d_Pro_%s", iCent, iVz, sysTagReal.c_str()),
                    tof_pro[iCent][iVz][0]
                );
                tf_tof->GetObject(
                    Form("TofEff_cent%d_vz%d_Pbar_%s", iCent, iVz, sysTagReal.c_str()),
                    tof_pbar[iCent][iVz][0]
                );
            }
        }
    }
    if (!pidOff) {
        tf_pid->GetObject(
            Form("PidEff_%s_Pro", nSigTag.c_str()),
            pid_pro
        );
        tf_pid->GetObject(
            Form("PidEff_%s_Pbar", nSigTag.c_str()),
            pid_pbar
        );
    }
    return;
}

double EffMaker::GetTpcEff(bool positive, double pt, double y, int cent, double vz_) {
    if (tpcOff) { return 1.0; }
    if (cent < 0 || cent >= nCent) { return -1; }
    int vz = VzSplit(vz_);
    if (vz < 0) { return -1; }
    double eff = -1;
    if (positive) {
        eff = tpc_pro[cent][vz][region]->Interpolate(y, pt);
    } else {
        eff = tpc_pbar[cent][vz][region]->Interpolate(y, pt);
    }
    if (eff < 0 || eff > 1) { return -1; }
    return eff;
}

double EffMaker::GetTofEff(bool positive, double pt, double y, int cent, double vz_) {
    if (tofOff) { return 1.0; }
    if (cent < 0 || cent >= nCent) { return -1; }
    int vz = VzSplit(vz_);
    if (vz < 0) { return -1; }
    double eff = -1;
    if (positive) {
        eff = tof_pro[cent][vz][region]->Interpolate(y, pt);
    } else {
        eff = tof_pbar[cent][vz][region]->Interpolate(y, pt);
    }
    if (eff < 0 || eff > 1) { return -1; }
    return eff;
}

double EffMaker::GetPidEff(bool positive, double pt, double y, bool asCut) {
    if (pidOff) { return 1.0; }
    if (positive) {
        h2 = pid_pro;
    } else {
        h2 = pid_pbar;
    }
    int ybin = h2->GetXaxis()->FindBin(y);
    int ptbin = h2->GetYaxis()->FindBin(pt);
    double eff = h2->GetBinContent(ybin, ptbin);
    if (eff < 0 || eff > 1) { return -1; }
    if (asCut) { eff *= 0.5; }
    return eff;
}

int EffMaker::VzSplit(double vz) {
    /*
        This depends on your vz split method.
        -1 means invalid vz
    */
    if (energy == "27") { // only 3 Vz bins for 27 GeV
        if (-27 < vz && vz < -10) {
            return 0;
        } else if (-10 < vz && vz < 10) {
            return 1;
        } else if (10 < vz && vz < 27) {
            return 2;
        } else {
            return -1;
        }
    } else {
        if (-30 < vz && vz < -10) {
            return 0;
        } else if (-10 < vz && vz < 10) {
            return 1;
        } else if (10 < vz && vz < 30) {
            return 2;
        } else if (-50 < vz && vz < -30) {
            return 3;
        } else if (30 < vz && vz < 50) {
            return 4;
        } else {
            return -1;
        }
    }
}
