#include <iostream>
#include <map>
#include "TMath.h"
#include "TF1.h"
#include "StPicoEvent/StPicoDst.h"
#include "StPicoEvent/StPicoEvent.h"
#include "StPicoEvent/StPicoTrack.h"
#include "StPicoDstMaker/StPicoDstMaker.h"
#include "StThreeVectorF.hh"

#include "MeanDcaTool.h"

BES2Processing::MeanDcaTool::MeanDcaTool() {
    funcUpperZ = new TF1("z_upper", "[0]+[1]/pow(x, [2])", 1, 600);
    funcUpperXY = new TF1("xy_upper", "[0]+[1]/pow(x, [2])", 1, 600);
    funcLowerZ = new TF1("z_lower", "[0]+[1]/pow(x, [2])", 1, 600);
    funcLowerXY = new TF1("xy_lower", "[0]+[1]/pow(x, [2])", 1, 600);
    SetUpperCurveParZ(1.0, 1.0, 2.0);
    SetUpperCurveParXY(1.0, 1.0, 2.0);
    SetLowerCurveParZ(1.0, -1.0, 2.0);
    SetLowerCurveParXY(1.0, -1.0, 2.0);
    clean();
}

void BES2Processing::MeanDcaTool::clean() {
    haveCache = false;
    mDCAz = 0;
    mDCAxy = 0;
    for (int i=0; i<N_MAX_DCA_TRACKS; i++) {
        aDCAz[i] = 0;
        aDCAxy[i] = 0;
    }
}

void BES2Processing::MeanDcaTool::SetUpperCurveParZ(double p0, double p1, double p2) {
    funcUpperZ->SetParameters(p0, p1, p2);
}
void BES2Processing::MeanDcaTool::SetUpperCurveParXY(double p0, double p1, double p2) {
    funcUpperXY->SetParameters(p0, p1, p2);
}
void BES2Processing::MeanDcaTool::SetLowerCurveParZ(double p0, double p1, double p2) {
    funcLowerZ->SetParameters(p0, p1, p2);
}
void BES2Processing::MeanDcaTool::SetLowerCurveParXY(double p0, double p1, double p2) {
    funcLowerXY->SetParameters(p0, p1, p2);
}

void BES2Processing::MeanDcaTool::ReadParams(const std::string& energy) {

    struct DCAPars {
        double DcaZUpperPars[3];
        double DcaZLowerPars[3];
        double DcaXYUpperPars[3];
        double DcaXYLowerPars[3];
    };
    
    std::map<std::string, DCAPars> dieMap = {
        {"7", {
            {-0.0422282, 2.83201, 0.550019},
            {0.0443011, -2.83318, 0.549238},
            {0.0599502, 3.60717, 0.669437},
            {-0.117658, -3.6066, 0.660038}
        }},
        {"9", {
            {-0.0901543, 4.0999, 0.585973},
            {0.0895117, -4.09181, 0.585405},
            {0.124171, 4.3685, 0.696959},
            {-0.158262, -4.4024, 0.68222}
        }},
        {"11", {
            {-0.0470324, 2.42636, 0.519896},
            {0.0480378, -2.41647, 0.517483},
            {0.166485, 2.84054, 0.63307},
            {-0.189927, -2.85686, 0.624115}
        }},
        {"14", {
            {-0.0638051, 2.06239, 0.465281},
            {0.0638185, -2.06531, 0.464807},
            {0.0603307, 2.57594, 0.556896},
            {-0.117867, -2.57567, 0.544328}
        }},
        {"17", {
            {-0.0591431, 2.38159, 0.496611},
            {0.0597762, -2.38383, 0.49625},
            {0.00151579, 2.86461, 0.556274},
            {-0.0651759, -2.86696, 0.541912}
        }},
        {"19", {
            {-0.028147, 2.50602, 0.548613},
            {0.0273647, -2.50816, 0.548273},
            {0.0649239, 2.9342, 0.610599},
            {-0.139503, -2.92802, 0.594963}
        }},
        {"27", {
            {-0.0377978, 2.36735, 0.446036},
            {0.0470615, -2.37667, 0.448459},
            {0.363222, 2.80508, 0.62448},
            {-0.326341, -2.81834, 0.628987}
        }}
    };
    auto it = dieMap.find(energy);
    DCAPars diePars;
    if (it == dieMap.end()) {
        diePars = dieMap.at("7");
        std::cout << "[WARNING] - From MeanDcaTool: Current energy tag: [" << energy << "] not found. I will use 7.7 GeV's parameters!\n";
    } else {
        diePars = it->second;
        std::cout << "[LOG] - From MeanDcaTool: Current energy tag: [" << energy << "]\n";
    }

    std::cout << "[LOG] - From MeanDcaTool: Now reading mean DCA parameters:\n";
    std::cout << "\t [DCAz] upper curve parameters: {" << diePars.DcaZUpperPars[0] << ", " << diePars.DcaZUpperPars[1] << ", " << diePars.DcaZUpperPars[2] << "}\n";
    std::cout << "\t [DCAz] lower curve parameters: {" << diePars.DcaZLowerPars[0] << ", " << diePars.DcaZLowerPars[1] << ", " << diePars.DcaZLowerPars[2] << "}\n";
    std::cout << "\t [sDCAxy] upper curve parameters: {" << diePars.DcaXYUpperPars[0] << ", " << diePars.DcaXYUpperPars[1] << ", " << diePars.DcaXYUpperPars[2] << "}\n";
    std::cout << "\t [sDCAxy] lower curve parameters: {" << diePars.DcaXYLowerPars[0] << ", " << diePars.DcaXYLowerPars[1] << ", " << diePars.DcaXYLowerPars[2] << "}\n";
    SetUpperCurveParZ(
       diePars.DcaZUpperPars[0],
       diePars.DcaZUpperPars[1],
       diePars.DcaZUpperPars[2]
    );
    SetLowerCurveParZ(
       diePars.DcaZLowerPars[0],
       diePars.DcaZLowerPars[1],
       diePars.DcaZLowerPars[2]
    );
    SetUpperCurveParXY(
       diePars.DcaXYUpperPars[0],
       diePars.DcaXYUpperPars[1],
       diePars.DcaXYUpperPars[2]
    );
    SetLowerCurveParXY(
       diePars.DcaXYLowerPars[0],
       diePars.DcaXYLowerPars[1],
       diePars.DcaXYLowerPars[2]
    );
}

bool BES2Processing::MeanDcaTool::Make(StPicoDst *pico) {
    int nTracks = pico->numberOfTracks();
    if (nTracks> N_MAX_DCA_TRACKS) {
        std::cout << "[WARNING] - From MeanDcaTool Number of tracks out of range: " << nTracks << " > " << N_MAX_DCA_TRACKS << ".\n";
        return false;
    }

    int nTrk_valid = 0;
    clean();
    StPicoEvent* event = (StPicoEvent*)pico->event();
    if (!event) { return false; }

    TVector3 vertex = event->primaryVertex();
    double vx = vertex.X();
    double vy = vertex.Y();
    double vz = vertex.Z();
    double mField = event->bField();

    for (int i=0; i<nTracks; i++) {
        StPicoTrack* track = (StPicoTrack*)pico->track(i);
        if (!track) { continue; }
        if (!track->isPrimary()) { continue; }
        aDCAz[nTrk_valid] = track->gDCAz(vz);
        aDCAxy[nTrk_valid] = track->helix(mField).geometricSignedDistance(vx, vy);
        nTrk_valid ++;
    }

    if (nTrk_valid == 0) { return false; }

    for (int i=0; i<nTrk_valid; i++) {
        mDCAz += aDCAz[i];
        mDCAxy += aDCAxy[i];
    }
    mDCAz = mDCAz / nTrk_valid;
    mDCAxy = mDCAxy / nTrk_valid;

    haveCache = true;

    return true;
}

bool BES2Processing::MeanDcaTool::IsBadMeanDcaZEvent(StPicoDst* pico) {
    bool res = true;
    if (!haveCache) {
        res = Make(pico);
    }
    if (!res) { return true; }
    int refMult = pico->event()->refMult();
    if (funcUpperZ->Eval(refMult) < mDCAz || funcLowerZ->Eval(refMult) > mDCAz) { return true; }
    return false;
}

bool BES2Processing::MeanDcaTool::IsBadMeanDcaXYEvent(StPicoDst* pico) {
    bool res = true;
    if (!haveCache) {
        res = Make(pico);
    }
    if (!res) { return true; }
    int refMult = pico->event()->refMult();
    if (funcUpperXY->Eval(refMult) < mDCAxy || funcLowerXY->Eval(refMult) > mDCAxy) { return true; }
    return false;
}
