#ifndef __TPCSHIFT__
#define __TPCSHIFT__

#include <string>
#include <map>

class TH1F;
class TH2F;


namespace BES2Processing {
    class TpcShiftTool {
        private:
            TH1F* runDepShift;

            int curRun; // record current run ID
            std::string rootFileName;

            const std::map<int, int>* runMap; // it is actually a pointer to the real run ID map (according to energy)
        
        public:

            TpcShiftTool(const std::string& energy);
            ~TpcShiftTool(){}

            bool Init();
            double GetShift(int runId, double pT, double eta);
            int GetPtBin(double pT);
            int GetEtaBin(double eta);
            int GetFinalBin(double pT, double eta);

    };
}
#endif
