class StChain;
StChain *chain = 0;

void loadLibs();

void makeFDst(Int_t nEvents = 100000, char *fileList="file.list", char *outDir = "./", char *index = "01", char *energy = "7") {

	// Start the stopwatch
	Double_t allowedRunTime(9999999);
	TStopwatch timer;
	timer.Start();

	// Load the Necessary Libraries
	loadLibs();

	gSystem->Load("DstMaker");
	gSystem->Load("TpcShiftTool");
	gSystem->Load("StCFMult");
	gSystem->Load("CentCorrTool");
	gSystem->Load("TriggerTool");
	gSystem->Load("MeanDcaTool");
	gSystem->Load("VtxShiftTool");
	gSystem->Load("EffMaker");

	// The Main Chain
	chain = new StChain("StChain");
	// chain->SetDebug();

	// Add The Makers to the Chain
	StPicoDstMaker *picoDstMaker = new StPicoDstMaker(2, fileList, "PicoDst");
	picoDstMaker->SetStatus("*", 0);
	picoDstMaker->SetStatus("PicoDst", 1);
	picoDstMaker->SetStatus("Event", 1);
	// picoDstMaker->SetStatus("Epd*", 1);
	picoDstMaker->SetStatus("Track", 1);
	picoDstMaker->SetStatus("BTofPidTraits", 1);
	picoDstMaker->SetStatus("ETofPidTraits", 1);

	DstMaker *femtoMaker = new DstMaker("DstMaker", energy);
	
	femtoMaker->SetFileIndex(index);
	femtoMaker->SetOutDir(outDir);

	// Initialize
	Int_t initStat = chain->Init();
	if (initStat)
	chain->Fatal(initStat, "Failure During Init()");

	Int_t istat, iev = 1;
	Bool_t timeStatus = true;
	EventLoop:
	if (iev <= nEvents && istat != 2 && timeStatus == true)
	{
	chain->Clear();
	//    cout << "---------------------- Processing Event : " << iev << " ----------------------" << endl;
	istat = chain->Make(iev); // This should call the Make() method in ALL makers
	//    cout << "===   Made Event.   === " << endl;
	if (istat == 2)
	{
		cout << "Last  Event Processed. Status = " << istat << endl;
	}
	if (istat == 3)
	{
		cout << "Error Event Processed. Status = " << istat << endl;
	}
	if (timer.RealTime() / 60. > allowedRunTime)
	{
		cout << "Maximum time reched after processing this many events: " << iev << endl;
		timeStatus = false;
	}
	timer.Continue();
	iev++;
	goto EventLoop;
	}

	return;
}

void loadLibs() {
	//  gSystem->Load("libTable");
	//  gSystem->Load("libPhysics");
	gSystem->Load("St_base");            //
	gSystem->Load("StChain");            //
	gSystem->Load("St_Tables");          //
	gSystem->Load("StUtilities");        //        // new addition 22jul99
	gSystem->Load("StTreeMaker");        //
	gSystem->Load("StIOMaker");          //
	gSystem->Load("StarClassLibrary");   //
	gSystem->Load("StTriggerDataMaker"); // new starting from April 2003
	gSystem->Load("StBichsel");          //
	gSystem->Load("StEvent");            //
	gSystem->Load("StEventUtilities");   //
	gSystem->Load("StDbLib");            //
	gSystem->Load("StEmcUtil");          //
	gSystem->Load("StTofUtil");          //
	//  gSystem->Load("StPmdUtil");
	//  gSystem->Load("StPreEclMaker");
	gSystem->Load("StStrangeMuDstMaker"); //
	gSystem->Load("StMuDSTMaker");        //
	gSystem->Load("libStarAgmlUtil");
	gSystem->Load("StPicoEvent");
	gSystem->Load("StPicoDstMaker");

	gSystem->ListLibraries();
}
