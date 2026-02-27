#ifndef __CENT_TOOL_HEADER__
#define __CENT_TOOL_HEADER__

#include <string>
#include <vector>
#include <iostream>

#include "TF1.h"
#include "TRandom3.h"

namespace BES2Processing{

    class CentCorrTool {
    
        private:
    
            TF1* PileUpCurve[3][2]; // [tofMult, tofMatch, tofBeta][upper lower]
            std::vector<double> vzPars; // we do this only for RefMult3X
            std::vector<int> centSplitEdge;
            std::vector<int> centSplitEdgeAlter;
            TRandom3* rd;

            bool is27;
            TF1* APUUpper;
            TF1* APULower;
    
        public:
            CentCorrTool();
            ~CentCorrTool(){}
    
            // -------------------------------------------------------------------
            void InitParams(const std::string& energy);
    
            bool IsPileUp(int refMult, int tofMult, int tofMatch, int tofBeta);
            int VzCorrection(int ref3, double vz); // this ref3 is actually RefMult3X for any energy except 27 GeV (is27 != true)
            int GetCorrectedRefMult3(int ref3, int refMult, int tofMult, int tofMatch, int tofBeta, double vz, int refMult4=0); // note, refMult4 will be used only for 27 GeV (is27 == true)
            int GetCentralityClass9(int ref3Corr);
            int GetCentralityClass24(int ref3Corr);
    
    };
    
}

#endif