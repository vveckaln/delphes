/*
Simple macro showing how to access branches from the delphes output root file,
loop over events, and plot simple quantities such as the jet pt and the di-electron invariant
mass.

root -l examples/Example1.C'("delphes_output.root")'
*/


#ifdef __CLING__
R__LOAD_LIBRARY(libDelphes)
#include "external/ExRootAnalysis/ExRootTreeReader.h"
#include "classes/DelphesClasses.h"
#endif

//------------------------------------------------------------------------------

void Example1(const char *inputFile, const char * channel)
{
  //gSystem->Load("libDelphes");

  // Create chain of root trees
  TChain chain("Delphes");
  chain.Add(inputFile);

  // Create object of class ExRootTreeReader
  ExRootTreeReader *treeReader = new ExRootTreeReader(&chain);
  Long64_t numberOfEntries = treeReader->GetEntries();
  printf("number of entries %u\n", treeReader -> GetEntries());
  // Get pointers to branches used in this analysis
  TClonesArray *branchJet = treeReader->UseBranch("Jet");
  TClonesArray *branchElectron = treeReader->UseBranch("Electron");
  TClonesArray *branchMuon = treeReader->UseBranch("Muon");
  TClonesArray *branchEvent = treeReader->UseBranch("Event");

  // Book histograms
  TH1 *histJetPT = new TH1F("jet_pt", "jet P_{T}", 100, 0.0, 100.0);
  TH1 *histMass = new TH1F("mass", "M_{inv}(e_{1}, e_{2})", 100, 0.0, 30.0);
  TH1 *histTOFmuons = new TH1F("TOFmuons", "TOF #mu; t [ns]; N", 15, 0.0, 15.0);
  TH1 *histTOFelectrons = new TH1F("TOFelectrons", "TOF electrons; t [ns]; N", 15, 0.0, 15.0);

  // Loop over all events
  for(Int_t entry = 0; entry < numberOfEntries; ++entry)
  {
    // Load selected branches with data from specified event
    treeReader -> ReadEntry(entry);

    
    //HepMCEvent *event = (HepMCEvent*) branchEvent -> At(0);
    //LHEFEvent *event = (LHEFEvent*) branchEvent -> At(0);
    //Float_t weight = event->Weight;

    // If event contains at least 1 jet
    if(branchJet && branchJet -> GetEntries() > 0)
    {
      // Take first jet
      Jet *jet = (Jet*) branchJet -> At(0);

      // Plot jet transverse momentum
      histJetPT -> Fill(jet ->PT);

      // Print jet transverse momentum
      //      cout << "Jet pt: "<<jet->PT << endl;
    }

    Electron *elec1, *elec2;

    // If event contains at least 2 electrons
    if (branchElectron -> GetEntries() > 1)
    {
      // Take first two electrons
      elec1 = (Electron *) branchElectron->At(0);
      elec2 = (Electron *) branchElectron->At(1);
      const float mass = ((elec1->P4()) + (elec2->P4())).M();
      //      printf("%f\n", mass);
      // Plot their invariant mass
      histMass -> Fill(((elec1->P4()) + (elec2->P4())).M());
    }

    Muon * muon(nullptr);
    if (branchMuon -> GetEntries() > 0)
    {
      muon = (Muon*) branchMuon -> At(0);
      double tof = (muon -> TOuter - muon -> T)*1E9;
      //      printf("muon %.12f, %.12f, %.12f\n", muon -> TOuter * 1E9, muon -> T * 1E9, (muon -> TOuter - muon -> T)*1E9);
      histTOFmuons -> Fill(tof);
    }

    Electron * electron(nullptr);
    //    printf("electrons %u\n", branchElectron -> GetEntries());
    if (branchElectron -> GetEntries() > 0)
    {
      electron = (Electron*) branchElectron -> At(0);
      double tofelectron = (electron -> TOuter - electron -> T)*1E9;
      //      printf("el %.12f, %.12f, %.12f\n", electron -> TOuter * 1E9, electron -> T * 1E9, (electron -> TOuter - electron -> T)*1E9);
      histTOFelectrons -> Fill(tofelectron);
    }

    
  }

  // Show resulting histograms
  histJetPT -> Draw();
  histMass -> Draw();
  TCanvas * cTOFmuons = new TCanvas;
  histTOFmuons -> Draw();
  cTOFmuons -> SaveAs((TString("histTOFmuons_") + channel + ".png").Data());

  TCanvas * cTOFelectrons = new TCanvas;
  histTOFelectrons -> Draw();
  cTOFelectrons -> SaveAs((TString("histTOFelectrons_") + channel + ".png").Data());

}

