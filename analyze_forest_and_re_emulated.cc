#include <iostream>
#include <map>

#include "TFile.h"
#include "TH1D.h"
#include "TROOT.h"
#include "TString.h"
#include "TTree.h"

using namespace std;

void analyze_forest_and_re_emulated() {
    unsigned int nthreads = 26;
    ROOT::EnableImplicitMT(nthreads);

    TFile *file_forest = new TFile("fout_20200721_addZDC_run327554.root", "read");
    TFile *file_re_emulated = new TFile(
        "HLTBitAna_OnlineGT_103X_dataRun2_HLT_v1_ForwardPD_Run327554_merged.root", "read");
    TFile *new_file =
        new TFile("result_from_forest_and_re_emulated.root", "recreate");

    unsigned long long event_in_forest;
    unsigned long long event_in_re_emulated;
    float hiHF;
    // Event selection
    int primaryVertexFilter;
    int hfCoincFilter2Th4;
    int clusterCompatibilityFilter;
    int HLT_HIZeroBias_v1; // ZeroBias Trigger
    int HLT_HIL1_ZDC_AND_OR_MinimumBiasHF2_AND_BptxAND_v1; // ZDC2 Trigger
    int HLT_HIMinimumBias_part2_v1; // MinBias Trigger
    
    map<unsigned long long, float> map_hiHF;
    map<unsigned long long, int> map_primaryVertexFilter;
    map<unsigned long long, int> map_clusterCompatibilityFilter;
    map<unsigned long long, int> map_hfCoincFilter2Th4;

    TTree *HiTree = (TTree *)file_forest->Get("hiEvtAnalyzer/HiTree");
    HiTree->SetBranchAddress("evt", &event_in_forest);
    HiTree->SetBranchAddress("hiHF", &hiHF);

    TTree *HltTree1 = (TTree *)file_forest->Get("skimanalysis/HltTree");
    HltTree1->SetBranchAddress("pprimaryVertexFilter", &primaryVertexFilter);
    HltTree1->SetBranchAddress("pclusterCompatibilityFilter", &clusterCompatibilityFilter);
    HltTree1->SetBranchAddress("phfCoincFilter2Th4", &hfCoincFilter2Th4);

    TTree *HltTree2 = (TTree *)file_re_emulated->Get("hltanalysis/HltTree");
    HltTree2->SetBranchAddress("Event", &event_in_re_emulated);
    HltTree2->SetBranchAddress("HLT_HIZeroBias_v1", &HLT_HIZeroBias_v1);
    HltTree2->SetBranchAddress("HLT_HIL1_ZDC_AND_OR_MinimumBiasHF2_AND_BptxAND_v1",
                                &HLT_HIL1_ZDC_AND_OR_MinimumBiasHF2_AND_BptxAND_v1);
    HltTree2->SetBranchAddress("HLT_HIMinimumBias_part2_v1", &HLT_HIMinimumBias_part2_v1);
                                

    TTree *newTree = new TTree("newTree", "newTree");
    float noTrigger;
    float passZeroBias;
    float passZDC2;
    float passMinBias;
    newTree->Branch("primaryVertexFilter", &primaryVertexFilter, "primaryVertexFilter/I");
    newTree->Branch("clusterCompatibilityFilter", &clusterCompatibilityFilter, "clusterCompatibilityFilter/I");
    newTree->Branch("hfCoincFilter2Th4", &hfCoincFilter2Th4, "hfCoincFilter2Th4/I");
    newTree->Branch("noTrigger", &noTrigger, "noTrigger/F");
    newTree->Branch("passZeroBias", &passZeroBias, "passZeroBias/F");
    newTree->Branch("passZDC2", &passZDC2, "passZDC2/F");
    newTree->Branch("passMinBias", &passMinBias, "passMinBias/F");
    
    // mapping
    int nEvents = HiTree->GetEntries();
    for (int ii = 0; ii < nEvents; ++ii) {
        HltTree1->GetEntry(ii);
        HiTree->GetEntry(ii);
        cout << ii << "/" << nEvents << " Making a hiHF map..." << endl;
        map_hiHF.insert(make_pair(event_in_forest, hiHF));
        map_primaryVertexFilter.insert(make_pair(event_in_forest, primaryVertexFilter));
        map_clusterCompatibilityFilter.insert(make_pair(event_in_forest, clusterCompatibilityFilter));
        map_hfCoincFilter2Th4.insert(make_pair(event_in_forest, hfCoincFilter2Th4));
    }
    // triggering
    nEvents = HltTree2->GetEntries();
    int count1 = 0;
    int count2 = 0;
    int count3 = 0;
    for (int ii = 0; ii < nEvents; ++ii) {
        HltTree2->GetEntry(ii);
        if (map_hiHF.count(event_in_re_emulated)) {
            hiHF = map_hiHF.find(event_in_re_emulated)->second;
            primaryVertexFilter = map_primaryVertexFilter.find(event_in_re_emulated)->second;
            clusterCompatibilityFilter = map_clusterCompatibilityFilter.find(event_in_re_emulated)->second;
            hfCoincFilter2Th4 = map_hfCoincFilter2Th4.find(event_in_re_emulated)->second;
            noTrigger = hiHF;
            passZeroBias = -1;
            passZDC2 = -1;
            passMinBias = -1;
            if (HLT_HIZeroBias_v1 == 1) {
                passZeroBias = hiHF;
                count1++;
            }
            if (HLT_HIL1_ZDC_AND_OR_MinimumBiasHF2_AND_BptxAND_v1 == 1) {
                passZDC2 = hiHF;
                count2++;
            }
            if (HLT_HIMinimumBias_part2_v1 == 1) {
                passMinBias = hiHF;
                count3++;
            }
            cout << ii << "/" << nEvents << "::" << count1 << "::" << count2 << "::" << count3 << endl;
            newTree->Fill();
        }
    }
    new_file->cd();
    newTree->Write();
    new_file->Close();
}
