#ifndef ST_FEMTO_DSTMAKER_h
#define ST_FEMTO_DSTMAKER_h

// C++ headers
#include <vector>
#include <string>

// ROOT headers
#include "TObject.h"
#include "TClonesArray.h"
#include "TVector3.h"
#include "StPicoEvent/StPicoTrack.h"

#include "StRoot/StPicoDstMaker/StPicoDstMaker.h"
#include "TVector3.h"

#include "StRoot/CentCorrTool/CentCorrTool.h"
#include "StRoot/MeanDcaTool/MeanDcaTool.h"
#include "StRoot/TpcShiftTool/TpcShiftTool.h"
#include "StRoot/StCFMult/StCFMult.h"
#include "StRoot/TriggerTool/TriggerTool.h"
#include "StRoot/VtxShiftTool/VtxShiftTool.h"
#include "StRoot/EffMaker/EffMaker.h"

class TH1F;
class TProfile;
class TF1;

struct SystematicCuts {
	std::string tag;
	double dca;
	int nHitsFit;
	double nSigmaProton;
	double mass2Low;
	double mass2High;
};

//_________________
class DstMaker : public StMaker {
	public:
		DstMaker(char * name, const std::string& energy);
		virtual ~DstMaker();

		Int_t Init();
		Int_t Finish();
		Int_t Make();
		void SetFileIndex(char *val) {mFileIndex=val;}
		void SetOutDir(char *val) {mOutDir=val;}
		// helper function, regardless of specific energy configurations
		bool IsQualifiedTrack(double dca, int nHitsFit, double dcaCut, double nHitsFitCut);
		bool IsIdentifiedProtonTrack(double nSigmaProton, double mass2, double nSigmaProtonCut, double mass2Low, double mass2High, int CutType=0);
		// we already know its energy -> only need pt, y, charge
		// 0 -> normal (only TPC), 1 -> as cut, 2 -> need TOF
		int GetCutType(double pt, double y, bool positive);

	private:
	
		StPicoDstMaker * mPicoDstMaker;

		char *mFileIndex;
		char *mOutDir;
		TFile * mOutfile;

		std::map<std::string, TProfile*> hPro;
		std::map<std::string, TProfile*> hPbar;

		std::vector<SystematicCuts> sysCuts;
		double vzCut;
		double vzCutExt;
		double ymax;
		int ybins; // it is related to ymax

		BES2Processing::StCFMult* mtMult;
		BES2Processing::MeanDcaTool* mtDca;
		BES2Processing::CentCorrTool* mtCent;
		BES2Processing::TpcShiftTool* mtShift;
		BES2Processing::TriggerTool* mtTrg;
		BES2Processing::VtxShiftTool* mtVtx;
		std::map<std::string, EffMaker*> mEffMap;

		// EffMaker related
		int LastRegion;

		std::string energy;

		ClassDef(DstMaker,1)
};


#endif
