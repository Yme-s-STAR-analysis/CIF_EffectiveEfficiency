#ifndef __TRGTOOL__
#define __TRGTOOL__

#include <string>
#include <map>
#include "TMath.h"

class StPicoEvent;

namespace BES2Processing {

    class TriggerTool {

        private:
            std::map<Int_t, Int_t> mTriggers;

        public:
            TriggerTool(const std::string& energy);
            ~TriggerTool(){}

            Int_t GetTriggerID(StPicoEvent* event); // from StPicoEvent
            Int_t GetConvertedTriggerID(StPicoEvent* event); // from StPicoEvent
            Int_t GetConvertedTriggerID(int trgid); // just convert the trigger ID

    };

}

#endif