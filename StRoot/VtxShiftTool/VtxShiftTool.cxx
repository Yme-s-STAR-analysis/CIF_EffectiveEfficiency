#include <map>
#include <iostream>
#include "TMath.h"
#include "VtxShiftTool.h"

BES2Processing::VtxShiftTool::VtxShiftTool(const std::string& energy) {

    struct vertex_xy { double x; double y; };
    const std::map<std::string, vertex_xy> dieMap = {
        {"7", {-0.2902, -0.2632}},
        {"9", {-0.2261, -0.2577}},
        {"11", {-0.2208, -0.2161}},
        {"14", {-0.2386, -0.2243}},
        {"17", {-0.2148, -0.1982}},
        {"19", {0.0417, -0.2715}},
        {"27", {0.09038, -0.1498}}
    };

    auto it = dieMap.find(energy);
    if (it == dieMap.end()) {
        vx0 = 0;
        vy0 = 0;
        std::cout << "[WARNING] VtxShiftTool: Energy tag [" << energy << "] not found!" << std::endl;
    } else {
        vx0 = it->second.x;
        vy0 = it->second.y;
    }

    std::cout << "[LOG] VtxShiftTool: Energy tag [" << energy << "]" << std::endl;
    std::cout << "\tVx will shift: " << vx0 << ", Vy will shift: " << vy0 << std::endl;  
}

double BES2Processing::VtxShiftTool::GetShiftedVr(double vx, double vy) {
    vx = vx - vx0;
    vy = vy - vy0;
    return TMath::Sqrt(vx*vx + vy*vy);
}