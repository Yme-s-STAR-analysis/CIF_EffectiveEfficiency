#ifndef __RunNumberHeader__
#define __RunNumberHeader__

#include <map>

#include "Headers/RunNumber_7.h"
#include "Headers/RunNumber_9.h"
#include "Headers/RunNumber_11.h"
#include "Headers/RunNumber_14.h"
#include "Headers/RunNumber_17.h"
#include "Headers/RunNumber_19.h"
#include "Headers/RunNumber_27.h"

namespace RunNumber {

    static const std::map<std::string, const std::map<int, int>*> mRunIdxMap = {
        {"7",  &RunNumber_7GeV::mRunIdxMap},
        {"9",  &RunNumber_9GeV::mRunIdxMap},
        {"11", &RunNumber_11GeV::mRunIdxMap},
        {"14", &RunNumber_14GeV::mRunIdxMap},
        {"17", &RunNumber_17GeV::mRunIdxMap},
        {"19", &RunNumber_19GeV::mRunIdxMap},
        {"27", &RunNumber_27GeV::mRunIdxMap}
    };
    
    static const std::map<std::string, const char*> mShiftFilePath = {
        {"7",  RunNumber_7GeV::mShiftFile},
        {"9",  RunNumber_9GeV::mShiftFile},
        {"11", RunNumber_11GeV::mShiftFile},
        {"14", RunNumber_14GeV::mShiftFile},
        {"17", RunNumber_17GeV::mShiftFile},
        {"19", RunNumber_19GeV::mShiftFile},
        {"27", RunNumber_27GeV::mShiftFile}
    };

	static const std::map<int, int>* getRunMap(const std::string& energy) {
        auto it = mRunIdxMap.find(energy);
        return (it != mRunIdxMap.end()) ? it->second : nullptr;
    }
    
    static const char* getShiftFile(const std::string& energy) {
        auto it = mShiftFilePath.find(energy);
        return (it != mShiftFilePath.end()) ? it->second : "";
    }
}


#endif
