#ifndef __EFFMAKER__
#define __EFFMAKER__

#include <string>
#include <vector>

class TH1D;
class TH2F;
class TF1;

class EffMaker{
    private:
        static const int nCent = 24;
        static const int nVz = 5;
        static const int nRegion = 2; // this is for 9.2 GeV

        int region;
        std::string energy;

        TH2F* pid_pro;
        TH2F* pid_pbar;
        TH2F* tpc_pro[nCent][nVz][nRegion];
        TH2F* tpc_pbar[nCent][nVz][nRegion];
        TH2F* tof_pro[nCent][nVz][nRegion];
        TH2F* tof_pbar[nCent][nVz][nRegion];

        TH2F* h2; // will be used multiple times

        bool tpcOff;
        bool tofOff;
        bool pidOff;

    public:
        EffMaker():region(0){}
        ~EffMaker(){}

        bool Init(std::string energy, std::string sysTag);
        void ReadInEffFile(const char* tpc, const char* tof, const char* pid, std::string sysTag);
        double GetTpcEff(bool positive, double pt, double y, int cent, double vz);
        double GetTofEff(bool positive, double pt, double y, int cent, double vz);
        double GetPidEff(bool positive, double pt, double y, bool asCut);
        int VzSplit(double vz);
        void SetRegion(int region) { this->region = region; } // for 9.2 GeV
};

#endif