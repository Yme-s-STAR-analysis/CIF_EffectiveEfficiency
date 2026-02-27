#include <iostream>
#include "TriggerTool.h"
#include "StPicoEvent/StPicoEvent.h"

BES2Processing::TriggerTool::TriggerTool(const std::string& energy) {
    const std::map<std::string, std::map<Int_t, Int_t>> dieMap = {
        {"7", {
            {810010, 0},
            {810020, 1},
            {810030, 2},
            {810040, 3}
        }},
        {"9", {
            {780010, 0},
            {780020, 1}
        }},
        {"11", {
            {710010, 0}
        }},
        {"14", {
            {650000, 0}
        }},
        {"17", {
            {870010, 0}
        }},
        {"19", {
            {640001, 0},
            {640011, 1},
            {640021, 2},
            {640031, 3},
            {640041, 4},
            {640051, 5}
        }},
        {"27", {
            {610011, 0},
            {610021, 1},
            {610051, 2}
        }}
    };
    auto it = dieMap.find(energy);
    if (it == dieMap.end()) {
        mTriggers = {{0, 0}};
        std::cout << "[WARNING] TriggerTool: Energy tag [" << energy << "] not found!" << std::endl;  
    } else{
        mTriggers = it->second;
        std::cout << "[LOG] TriggerTool: Energy tag [" << energy << "]" << std::endl;
        for (const auto& item : mTriggers) {
            std::cout << "\t" << item.first << " - " << item.second << std::endl;
        }
    }
}

Int_t BES2Processing::TriggerTool::GetTriggerID(StPicoEvent* event) {
    for (auto item : mTriggers) {
        if (event->isTrigger(item.first)) {
            return item.first;
        }
    }
    return -1;
}

Int_t BES2Processing::TriggerTool::GetConvertedTriggerID(StPicoEvent* event) {
    for (auto item : mTriggers) {
        if (event->isTrigger(item.first)) {
            return item.second;
        }
    }
    return -1;
}

Int_t BES2Processing::TriggerTool::GetConvertedTriggerID(int trgid) {
    for (auto item : mTriggers) {
        if (trgid == item.first) {
            return item.second;
        }
    }
    return -1;
}