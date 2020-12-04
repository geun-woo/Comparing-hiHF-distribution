#include <iostream>

#include "TFile.h"
#include "TH1F.h"
#include "TROOT.h"
#include "TString.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TLegend.h"

void hist_ZB_MB_ZDC() {
    TFile *file = new TFile("result_from_forest_and_re_emulated.root", "read");

    TTree *newTree = (TTree *)file->Get("newTree");

    int nBin = 200;
    TH1F *h_noTrigger = new TH1F("h_noTrigger", "PV;hiHF;Entries", nBin, 0, 200);
    TH1F *h_ZeroBias  = new TH1F("h_ZeroBias", ";hiHF;Entries", nBin, 0, 200);
    TH1F *h_ZDC2      = new TH1F("h_ZDC2", ";hiHF;Entries", nBin, 0, 200);
    TH1F *h_MinBias   = new TH1F("h_MinBias", ";hiHF;Entries", nBin, 0, 200);

    TCanvas *c1 = new TCanvas("c1","c1",800,650);
    TCanvas *c2 = new TCanvas("c2","c2",800,650);
    gStyle->SetOptStat(0);
    // TCanvas *c3 = new TCanvas("c3","c3",800,650);
    // TCanvas *c4 = new TCanvas("c4","c4",800,650);
    
    c1->SetLogy();
    // c2->SetLogy();
    // c3->SetLogy();
    // c4->SetLogy();

    c1->cd();
    newTree->Draw("noTrigger>>h_noTrigger", "primaryVertexFilter == 1");
    h_noTrigger->SetAxisRange(0.5, 100000, "Y");
    h_noTrigger->SetLineColor(kRed);
    newTree->Draw("passZDC2>>h_ZDC2", "passZDC2 != -1 && primaryVertexFilter == 1");
    h_ZDC2->SetLineColor(kGreen);
    newTree->Draw("passMinBias>>h_MinBias", "passMinBias != -1 && primaryVertexFilter == 1");
    h_MinBias->SetLineColor(kBlue);

    h_noTrigger->Draw();
    h_ZDC2->Draw("same");
    h_MinBias->Draw("same");

    TLegend *leg1 = new TLegend(.65, .65, .88, .88);
    leg1->AddEntry("h_noTrigger", "pass ZeroBias");
    leg1->AddEntry("h_ZDC2", "pass ZDCAND_OR_HF2");
    leg1->AddEntry("h_MinBias", "pass MinBias");
    leg1->Draw();

    TH1F *h_ZDC2_ZB = (TH1F*)h_ZDC2->Clone("h_ZDC2_ZB");
    TH1F *h_MB_ZB = (TH1F*)h_MinBias->Clone("h_MB_ZB");
    h_ZDC2_ZB->Divide(h_noTrigger);
    h_ZDC2_ZB->SetTitle("Ratio (PV)");
    h_ZDC2_ZB->SetAxisRange(0, 1.3, "Y");
    h_MB_ZB->Divide(h_noTrigger);
    c2->cd();
    h_ZDC2_ZB->Draw();
    h_MB_ZB->Draw("same");

    TLegend *leg2 = new TLegend(.65, .75, .88, .88);
    leg2->AddEntry("h_ZDC2_ZB", "ZDCAND_OR_HF2/ZB");
    leg2->AddEntry("h_MB_ZB", "MB/ZB");
    leg2->Draw();
}