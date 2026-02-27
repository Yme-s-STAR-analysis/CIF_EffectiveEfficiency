#ifndef __VTXSHIFTTOOL__
#define __VTXSHIFTTOOL__

#include <string>

namespace BES2Processing {

    class VtxShiftTool {
        
        private:
            double vx0;
            double vy0;
        
        public:
            VtxShiftTool(const std::string& energy);
            ~VtxShiftTool(){}
            
            double GetShiftedVr(double vx, double vy);
    };
    
}

#endif