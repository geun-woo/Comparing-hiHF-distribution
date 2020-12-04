#include <iostream>

#include "TCanvas.h"
#include "TFile.h"
#include "TH1D.h"
#include "TROOT.h"
#include "TString.h"
#include "TTree.h"

using namespace std;

int analyze_fout_20200521() {
    UInt_t nthreads = 26;
    ROOT::EnableImplicitMT(nthreads);

    TFile *file = new TFile("~/cms/fout_20200521.root", "read");
    TFile *new_file = new TFile("result_from_fout_20200521.root", "recreate");

    Float_t hiHF;
    Int_t primaryVertexFilter;
    Int_t hfCoincFilter2Th4;
    Int_t clusterCompatibilityFilter;
    Int_t HLT_HIZeroBias_v1;
    Int_t HLT_HIL1_ZDC_AND_OR_MinimumBiasHF2_AND_BptxAND_v1;

    TTree *HiTree = (TTree *)file->Get("hiEvtAnalyzer/HiTree");
    TTree *HltTree1 = (TTree *)file->Get("hltanalysis/HltTree");
    TTree *HltTree2 = (TTree *)file->Get("skimanalysis/HltTree");
    HiTree->SetBranchAddress("hiHF", &hiHF);
    HltTree1->SetBranchAddress("HLT_HIZeroBias_v1", &HLT_HIZeroBias_v1);
    HltTree1->SetBranchAddress(
        "HLT_HIL1_ZDC_AND_OR_MinimumBiasHF2_AND_BptxAND_v1",
        &HLT_HIL1_ZDC_AND_OR_MinimumBiasHF2_AND_BptxAND_v1);
    HltTree2->SetBranchAddress("pprimaryVertexFilter", &primaryVertexFilter);
    HltTree2->SetBranchAddress("phfCoincFilter2Th4", &hfCoincFilter2Th4);
    HltTree2->SetBranchAddress("pclusterCompatibilityFilter",
                               &clusterCompatibilityFilter);

    TTree *newTree = new TTree("newTree", "newTree");
    Float_t nHLT_HIZeroBias_v1_vs_hiHF;
    Float_t nHLT_HIL1_ZDC_AND_OR_MinimumBiasHF2_AND_BptxAND_v1_vs_hiHF;
    newTree->Branch("nHLT_HIZeroBias_v1_vs_hiHF", &nHLT_HIZeroBias_v1_vs_hiHF,
                    "nHLT_HIZeroBias_v1_vs_hiHF/F");
    newTree->Branch(
        "nHLT_HIL1_ZDC_AND_OR_MinimumBiasHF2_AND_BptxAND_v1_vs_hiHF",
        &nHLT_HIL1_ZDC_AND_OR_MinimumBiasHF2_AND_BptxAND_v1_vs_hiHF,
        "nHLT_HIL1_ZDC_AND_OR_MinimumBiasHF2_AND_BptxAND_v1_vs_hiHF/F");

    Int_t nEvents = HiTree->GetEntries();
    Int_t count1 = 0;
    Int_t count2 = 0;
    for (Int_t ii = 0; ii < nEvents; ++ii) {
        HltTree2->GetEntry(ii);
        if ((primaryVertexFilter == 1) && (hfCoincFilter2Th4 == 1) &&
            (clusterCompatibilityFilter == 1)) {
            HiTree->GetEntry(ii);
            HltTree1->GetEntry(ii);

            nHLT_HIZeroBias_v1_vs_hiHF = -1;
            nHLT_HIL1_ZDC_AND_OR_MinimumBiasHF2_AND_BptxAND_v1_vs_hiHF = -1;
            if (HLT_HIZeroBias_v1 == 1) {
                nHLT_HIZeroBias_v1_vs_hiHF = hiHF;
                count1++;
                cout << ii << "/" << nEvents << "::" << count1 << "::" << count2
                     << endl;
            }
            if (HLT_HIL1_ZDC_AND_OR_MinimumBiasHF2_AND_BptxAND_v1 == 1) {
                nHLT_HIL1_ZDC_AND_OR_MinimumBiasHF2_AND_BptxAND_v1_vs_hiHF =
                    hiHF;
                count2++;
            }
            newTree->Fill();
        }
    }
    new_file->cd();
    newTree->Write();
    new_file->Close();

    return 0;
}
