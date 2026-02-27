#ifndef __STCFMULT_HEADER__
#define __STCFMULT_HEADER__

#include <string>
#include <limits>
#include "TVector3.h"

#include "StPicoEvent/StPicoDst.h"
#include "StPicoEvent/StPicoEvent.h"
#include "StPicoEvent/StPicoTrack.h"
#include "StPicoEvent/StPicoPhysicalHelix.h"
#include "StPicoDstMaker/StPicoDstMaker.h"
#include "StThreeVectorF.hh"
#include "StPicoEvent/StPicoBTofPidTraits.h"
#include "StBTofUtil/tofPathLength.hh"
#include "phys_constants.h"

#include "TpcShiftTool/TpcShiftTool.h"

namespace BES2Processing {

    class StCFMult {
        public:

            Int_t mRefMult;
            Int_t mRefMult3;
            Int_t mRefMult4; // for 27 GeV
            Int_t mRefMult3X;
            Int_t mNTofBeta;
            Int_t mNTofMatch;
            Int_t mTofMult;

            TpcShiftTool* mtShift;

            bool is27;

            StCFMult(bool is27);
            ~StCFMult(){}

            void clean();
            
            void ImportShiftTool(TpcShiftTool* shift_ptr);
            bool make(StPicoDst *picoDst);
    };

}

#endif
