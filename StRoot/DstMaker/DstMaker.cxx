
#include <iostream>
#include <limits>
#include <cstdio>
#include <vector>
#include <utility>
#include <map>
#include "Riostream.h"

#include "TObject.h"
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TString.h"
#include "TVector3.h"
#include "TF1.h"
#include "TH2F.h"
#include "TProfile.h"

#include "StMaker.h"
#include "StPicoEvent/StPicoBTofPidTraits.h"
#include "StPicoEvent/StPicoETofPidTraits.h"
#include "StPicoEvent/StPicoTrack.h"
#include "StPicoEvent/StPicoEvent.h"
#include "StPicoEvent/StPicoDst.h"
#include "StPicoEvent/StPicoPhysicalHelix.h"
#include "StPicoDstMaker/StPicoDstMaker.h"
#include "StTriggerData.h"
#include "StTriggerIdCollection.h"
#include "StRunInfo.h"

#include "StBTofUtil/tofPathLength.hh"
#include "StarClassLibrary/StPhysicalHelixD.hh"
#include "StarClassLibrary/StLorentzVectorF.hh"
#include "phys_constants.h"

#include "DstMaker.h"

ClassImp(DstMaker)
DstMaker::DstMaker(char *name, const std::string& energy) : StMaker(name) {
	this->energy = energy;
	LastRegion = 0; // possible to be 1 only for 9.2 GeV
}

DstMaker::~DstMaker() {
	delete mPicoDstMaker;
}

Int_t DstMaker::Init(){
	TString filename = "";
	filename.Append(mFileIndex);
	filename.Append(".fDst.root");
	filename.Prepend(mOutDir);
	mOutfile = new TFile(filename, "recreate");

	mtDca = new BES2Processing::MeanDcaTool();
	mtCent = new BES2Processing::CentCorrTool();
	mtMult = new BES2Processing::StCFMult(energy == "27");
	mtShift = new BES2Processing::TpcShiftTool(energy);
	mtTrg = new BES2Processing::TriggerTool(energy);
	mtVtx = new BES2Processing::VtxShiftTool(energy);

	// mean dca tool setup
	mtDca->ReadParams(energy);

	// centrality tool
	mtCent->InitParams(energy);

	// Multiplicity and shift tool
	mtShift->Init();
	mtMult->ImportShiftTool(mtShift);

	// tag, dca, nHitsFit, nSigmaProton, mass2Low, mass2High
	if (energy == "27") {
		sysCuts = {
			{"default", 1.0, 15, 2, 0.6, 1.2},
			{"dca0p8", 0.8, 15, 2, 0.6, 1.2},
			{"dca0p9", 0.9, 15, 2, 0.6, 1.2},
			{"dca1p1", 1.1, 15, 2, 0.6, 1.2},
			{"dca1p2", 1.2, 15, 2, 0.6, 1.2},
			{"nsig1p6", 1.0, 15, 1.6, 0.6, 1.2},
			{"nsig1p8", 1.0, 15, 1.8, 0.6, 1.2},
			{"nsig2p2", 1.0, 15, 2.2, 0.6, 1.2},
			{"nsig2p5", 1.0, 15, 2.5, 0.6, 1.2},
			{"mass21", 1.0, 15, 2, 0.5, 1.1},
			{"mass22", 1.0, 15, 2, 0.55, 1.15},
			{"mass23", 1.0, 15, 2, 0.65, 1.25},
			{"mass24", 1.0, 15, 2, 0.7, 1.3},
			{"nhit12", 1.0, 12, 2, 0.6, 1.2},
			{"nhit17", 1.0, 17, 2, 0.6, 1.2}
		};
		vzCut = 27;
		ymax = 0.5;
		ybins = 5;
	} else {
		sysCuts = {
			{"default", 1.0, 20, 2, 0.6, 1.2},
			{"dca0p8", 0.8, 20, 2, 0.6, 1.2},
			{"dca0p9", 0.9, 20, 2, 0.6, 1.2},
			{"dca1p1", 1.1, 20, 2, 0.6, 1.2},
			{"dca1p2", 1.2, 20, 2, 0.6, 1.2},
			{"nsig1p6", 1.0, 20, 1.6, 0.6, 1.2},
			{"nsig1p8", 1.0, 20, 1.8, 0.6, 1.2},
			{"nsig2p2", 1.0, 20, 2.2, 0.6, 1.2},
			{"nsig2p5", 1.0, 20, 2.5, 0.6, 1.2},
			{"mass21", 1.0, 20, 2, 0.5, 1.1},
			{"mass22", 1.0, 20, 2, 0.55, 1.15},
			{"mass23", 1.0, 20, 2, 0.65, 1.25},
			{"mass24", 1.0, 20, 2, 0.7, 1.3},
			{"nhit15", 1.0, 15, 2, 0.6, 1.2},
			{"nhit18", 1.0, 18, 2, 0.6, 1.2},
			{"nhit22", 1.0, 22, 2, 0.6, 1.2},
			{"nhit25", 1.0, 25, 2, 0.6, 1.2}
		};
		vzCut = 50;
		ymax = 0.6;
		ybins = 6;
	}

	// set vz cut for extended rapidity (y0p6)
	std::map<std::string, double> vzMapExt = {
		{"7", 20},
		{"9", 30},
		{"11", 30},
		{"14", 40},
		{"17", 40},
		{"19", 40},
		{"27", 27}
	};
	auto it = vzMapExt.find(energy);
	if (it != vzMapExt.end()) {
		vzCutExt = it->second;
	} else {
		vzCutExt = vzCut;
		std::cout << "[WARNING] - DstMaker: Energy tag [" << energy << "] not found!" << std::endl;
	}

	// set efficiency maker
	for (auto& item : sysCuts) {
		auto nSigTag =  (item.tag.find("nsig") == 0) ? item.tag.substr(4) : "2p0";
		EffMaker* mEffTmp = new EffMaker();
		bool effSuccess = mEffTmp->Init(energy, item.tag);
		if (!effSuccess) {
			std::cout << "[ERROR] - DstMaker: Fail to initialize the efficiency maker." << std::endl;
			return kFatal;
		}
		mEffTmp->SetRegion(0);
		mEffMap[item.tag] = mEffTmp;
	}

	// init Th1F maps here
	for (const auto& cut : sysCuts) {
		std::string histNameP = "hPro_" + cut.tag;
		std::string histNameA = "hPbar_" + cut.tag;
		hPro[cut.tag] = new TProfile(
			histNameP.c_str(), Form("%s;y;N(proton)", cut.tag.c_str()), 
			ybins, 0, ymax
		);
		hPbar[cut.tag] = new TProfile(
			histNameA.c_str(), Form("%s;y;N(antiproton)", cut.tag.c_str()), 
			ybins, 0, ymax
		);
	}
	return kStOK;
}

Int_t DstMaker::Finish() {
	mOutfile->cd();
	for (const auto& cut : sysCuts) {
		hPro[cut.tag]->Write();
		hPbar[cut.tag]->Write();
	}
	return kStOK;
}

Int_t DstMaker::Make() {
	mPicoDstMaker = (StPicoDstMaker *)GetMaker("PicoDst");
	if (!mPicoDstMaker) {
		fputs("[ERROR] - DstMaker::Init() - Can't get pointer to StPicoDstMaker!", stderr);
		return kStFATAL;
	}

	StPicoDst *mPicoDst = NULL;
	mPicoDst = mPicoDstMaker->picoDst();
	if (!mPicoDst) {
		fputs("[ERROR] - DstMaker::Init() - Can't get pointer to StPicoDst!", stderr);
		return kStFATAL;
	}

	StPicoEvent *event = mPicoDst->event();

	if (!event) {
		cout << "[ERROR] - DstMaker::Make() No Event Found!" << endl;
		return kStOK;
	}

	Int_t runId = event->runId();
	Int_t region = 0;
	if (energy == "9.2" && runId >= 21229042) {
		region = 1;
	}
	if (region != LastRegion) {
		for (auto& item : mEffMap) {
			item.second->SetRegion(region);
		}
		LastRegion = region;
	}

	// Vertex Cut
	TVector3 pVtx = event->primaryVertex();
	Float_t vx = event->primaryVertex().X();
	Float_t vy = event->primaryVertex().Y();
	Float_t vz = event->primaryVertex().Z();

	if (fabs(vx) < 1.e-5 && 
		fabs(vy) < 1.e-5 &&
		fabs(vz) < 1.e-5) {
		return kStOK;
	}

	// wide vertex cut -> tight cut now
	if (fabs(vz) > vzCut) { return kStOK; }
	auto vr = mtVtx->GetShiftedVr(vx, vy);
	if (vr > 1) { return kStOK; }

	Int_t nTracks = mPicoDst->numberOfTracks();

	// check trigger ID
	Int_t trgid = mtTrg->GetTriggerID(event);
	if (trgid < 0) { return kStOK; }

	// centrality
	mtMult->make(mPicoDst);
	Int_t refMult = mtMult->mRefMult;
	Int_t tofMult = mtMult->mTofMult;
	Int_t nTofMatch = mtMult->mNTofMatch;
	Int_t nTofBeta = mtMult->mNTofBeta;
	Int_t refMult4 = mtMult->mRefMult4;

	Int_t refMult3X = mtMult->mRefMult3X;// this will be RefMult3 for 27 GeV (implemented in StCFMult)
	refMult3X = mtCent->GetCorrectedRefMult3(
		refMult3X, refMult, tofMult, nTofMatch, nTofBeta,
		vz, refMult4
	);

	if (refMult3X < 0) { return kStOK; } // pile up events: -1

	// in case we need this centrality class one day...
	int centX = mtCent->GetCentralityClass24(refMult3X);
	if (centX < 0) { return kStOk; }

	// check DCA
	if (!mtDca->Make(mPicoDst)) { return kStOK; }
	if (mtDca->IsBadMeanDcaZEvent(mPicoDst) || mtDca->IsBadMeanDcaXYEvent(mPicoDst)) {
		return kStOK;
	}
	// track loop start
	const Float_t mField = event->bField();
	for (Int_t i = 0; i < nTracks; i++){
		StPicoTrack *mPicoTrack = mPicoDst->track(i);
		if (!mPicoTrack) { continue; }
		if (!mPicoTrack->isPrimary()){ continue; }
		TVector3 momentum = mPicoTrack->pMom();
		// Float_t dca = fabs(mPicoTrack->gDCA(vx, vy, vz));
		// we will do some wide cut first / and hard cuts (like nHitsRatio)
		StPicoPhysicalHelix helix = mPicoTrack->helix(mField);
		Double_t dca = fabs(helix.geometricSignedDistance(pVtx));
		if (dca > 1.2){ continue; }
		Float_t nHitsFit = mPicoTrack->nHitsFit();
		Float_t nHitsMax = mPicoTrack->nHitsMax();
		Float_t nHitsDedx = mPicoTrack->nHitsDedx();

		if (nHitsDedx < 5) { continue; }
		if (nHitsFit <= 12){ continue; }
		if ((nHitsFit / nHitsMax) < 0.52){ continue; }

		Float_t q = mPicoTrack->charge();
		bool positive = q > 0;
		Float_t pz = momentum.Z();
		Float_t pt = momentum.Perp();
		Float_t pcm = momentum.Mag();
		Float_t eta = momentum.PseudoRapidity();
		if (pt <= 0.4 || pt >= 2){ continue; }

		Float_t EP = sqrt(pcm * pcm + 0.938272 * 0.938272);
		Float_t YP = TMath::Log((EP + pz) / (EP - pz)) * 0.5;
		if (fabs(YP) > ymax) { continue; }
		if (fabs(YP) >= 0.6) { continue; }
		else if (fabs(YP) >= 0.5 && fabs(vz) > vzCutExt) { continue; }

		double nSigProton = mPicoTrack->nSigmaProton();
		nSigProton -= mtShift->GetShift(runId, pt, eta);
		if (fabs(nSigProton) > 2.5){ continue; }

		// check the track quality
		for (const auto& cut : sysCuts) {
			if (!IsQualifiedTrack(dca, nHitsFit, cut.dca, cut.nHitsFit)) { continue; }
			int CutType = GetCutType(pt, YP, positive);

			double eff = 1.0;
			double pid_eff = mEffMap[cut.tag]->GetPidEff(positive, pt, YP, CutType == 1);

			double tpc_eff = mEffMap[cut.tag]->GetTpcEff(positive, pt, YP, centX, vz);
			double tof_eff = mEffMap[cut.tag]->GetTofEff(positive, pt, YP, centX, vz);

			eff = tpc_eff * pid_eff;
			if (CutType == 2) { eff *= tof_eff; }
            eff = eff > 1.0 ? 1.0 : eff;
            eff = eff < 0.0 ? 0.0 : eff;

			if (eff == 0) { continue; }
			if (positive) {
				hPro[cut.tag]->Fill(fabs(YP), 1.0/eff);
			} else {
				hPbar[cut.tag]->Fill(fabs(YP), 1.0/eff);
			}
		}
	}

	return kStOK;
}

bool DstMaker::IsQualifiedTrack(double dca, int nHitsFit, double dcaCut, double nHitsFitCut) {
	return dca < dcaCut && nHitsFit > nHitsFitCut;
}

bool DstMaker::IsIdentifiedProtonTrack(double nSigmaProton, double mass2, double nSigmaProtonCut, double mass2Low, double mass2High, int CutType) {
	if (CutType == 0) { // only TPC
		return fabs(nSigmaProton) < nSigmaProtonCut;
	} else if (CutType == 1) { // asymmetric TPC cut
		return nSigmaProton > 0 && nSigmaProton < nSigmaProtonCut;
	} else if (CutType == 2) {
		return fabs(nSigmaProton) < nSigmaProtonCut && mass2 > mass2Low && mass2 < mass2High;
	} else {
		return -1;
	}
}

int DstMaker::GetCutType(double pt, double y, bool positive) {
	// this y is abs. value of y (processed in Make)
	bool AsCut = false;
	bool NeedTof = false;
	// particular conditions depend on collision energy
	if (energy == "7") {
		if (positive) {
			AsCut = (pt > 0.7 && y >= 0.5);
			NeedTof = pt > 0.8;
		} else {
			if (y < 0.4) { NeedTof = pt > 0.7; }
			else {
				AsCut = true; // pt > 0.4
				NeedTof = pt > 0.8;
			}
		}
	} else if (energy == "9") {
		if (y < 0.5) { NeedTof = pt > 0.8; }
		else { // y >= 0.5
			AsCut = pt > 0.8;
			NeedTof = pt > 0.9;
		}
	} else if (energy == "11") {
		if (y < 0.5) { NeedTof = pt > 0.8; }
		else { // y >= 0.5
			if (positive) {
				AsCut = pt > 0.9;
				NeedTof = pt > 1.1;
			} else {
				AsCut = pt > 0.7;
				NeedTof = pt > 0.9;
			}
		}
	} else if (energy == "14") {
		if (y < 0.5) { NeedTof = pt > 0.8; }
		else { // y >= 0.5
			AsCut = pt > 0.8;
			NeedTof = pt > 1.0;
		}
	} else if (energy == "17") {
		if (y < 0.5) { NeedTof = pt > 0.8; }
		else { // y >= 0.5
			AsCut = pt > 0.8;
			NeedTof = pt > 1.0;
		}
	} else if (energy == "19") {
		if (y < 0.5) { NeedTof = pt > 0.8; }
		else { // y >= 0.5
			if (positive) {
				AsCut = pt > 0.9;
				NeedTof = pt > 1.1;
			} else {
				AsCut = pt > 0.7;
				NeedTof = pt > 1.0;
			}
		}
	} else if (energy == "27") {
		NeedTof = pt > 0.8;
	}

	// return: 0 -> normal (only TPC), 1 -> asymmetric cut, 2 -> need TOF
	if (NeedTof) { return 2; }
	else if (AsCut) { return 1; }
	else { return 0; }
}
