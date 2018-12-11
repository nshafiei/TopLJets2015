#include "TopLJets2015/TopAnalysis/interface/GeneratorTools.h"
#include "TH1F.h"
#include <iostream>

using namespace std;

//
std::vector< WeightSysts_t > getWeightSysts(TFile *f,TString sample){
  std::vector< WeightSysts_t > systsOfInterest;

  TH1 *h=(TH1 *) f->Get("analysis/generator_initrwgt");
  if(h==0) return systsOfInterest;

  for(Int_t xbin=1; xbin<=h->GetNbinsX(); xbin++){
    TString label=h->GetXaxis()->GetBinLabel(xbin);
    if(label.Length()==0) continue;

    if(sample=="EWKAJJ2017") {
      if(label.Contains("mur=0.5 muf=1")   )   systsOfInterest.push_back( WeightSysts_t("muRdn",    xbin-1) );
      if(label.Contains("mur=2 muf=1")     )   systsOfInterest.push_back( WeightSysts_t("muRup",    xbin-1) );
      if(label.Contains("mur=1 muf=0.5")   )   systsOfInterest.push_back( WeightSysts_t("muFdn",    xbin-1) );
      if(label.Contains("mur=1 muf=2")     )   systsOfInterest.push_back( WeightSysts_t("muFup",    xbin-1) );
      if(label.Contains("mur=0.5 muf=0.5") )   systsOfInterest.push_back( WeightSysts_t("muRmuFdn", xbin-1) );
      if(label.Contains("mur=2 muf=2")     )   systsOfInterest.push_back( WeightSysts_t("muRmuFup", xbin-1) );
      if(label.Contains("NNPDF31_nnlo_hessian_pdfas")) {
        Int_t start=label.Index("Member ")+7;
        TString id=label(start,3);
        if(id.EndsWith(" ")) id=label(start,2);
        if(id.EndsWith("o")) id=label(start,1);
        systsOfInterest.push_back( WeightSysts_t("PDF"+id,xbin-1) );
      }
    }
    
    if(sample=="TTJets2016") {
      if(label.Contains("muR=0.5 muF=1")   && label.Contains("hdamp=mt"))   systsOfInterest.push_back( WeightSysts_t("muRdn",    xbin-1) );
      if(label.Contains("muR=2 muF=1")     && label.Contains("hdamp=mt"))   systsOfInterest.push_back( WeightSysts_t("muRup",    xbin-1) );
      if(label.Contains("muR=1 muF=0.5")   && label.Contains("hdamp=mt"))   systsOfInterest.push_back( WeightSysts_t("muFdn",    xbin-1) );
      if(label.Contains("muR=1 muF=2")     && label.Contains("hdamp=mt"))   systsOfInterest.push_back( WeightSysts_t("muFup",    xbin-1) );
      if(label.Contains("muR=0.5 muF=0.5") && label.Contains("hdamp=mt"))   systsOfInterest.push_back( WeightSysts_t("muRmuFdn", xbin-1) );
      if(label.Contains("muR=2 muF=2")     && label.Contains("hdamp=mt"))   systsOfInterest.push_back( WeightSysts_t("muRmuFup", xbin-1) );
      if(label.Contains("PDF set = 260")) {
        Int_t start=label.Index("PDF set = 260")+13;
        TString id=label(start,3);
        systsOfInterest.push_back( WeightSysts_t("PDF"+id,xbin-1) );
      }
      if(label.Contains("PDF set = 265000")) systsOfInterest.push_back( WeightSysts_t("PDF101",xbin-1) );
      if(label.Contains("PDF set = 266000")) systsOfInterest.push_back( WeightSysts_t("PDF102",xbin-1) );
    }
  }
  
  return systsOfInterest;
}


//b fragmentation
std::map<TString, TGraph*> getBFragmentationWeights(TString era) {
  std::map<TString, TGraph*> bfragMap;

  TString bfragWgtUrl(era+"/bfragweights.root");
  gSystem->ExpandPathName(bfragWgtUrl);
  TFile *fIn=TFile::Open(bfragWgtUrl);
  bfragMap["upFrag"]       = (TGraph *)fIn->Get("upFrag");
  bfragMap["centralFrag"]  = (TGraph *)fIn->Get("centralFrag");
  bfragMap["downFrag"]     = (TGraph *)fIn->Get("downFrag");
  bfragMap["PetersonFrag"] = (TGraph *)fIn->Get("PetersonFrag");
  return bfragMap;
}

float computeBFragmentationWeight(MiniEvent_t &ev, TGraph* wgtGr) {
  float weight = 1.;
  for (int i = 0; i < ev.ng; i++) {
    if (abs(ev.g_id[i])==5) weight *= wgtGr->Eval(ev.g_xb[i]);
  }
  return weight;
}

std::map<TString, std::map<int, float> > getSemilepBRWeights(TString era) {
  std::map<TString, TGraph*> bfragMap;
  std::map<TString, std::map<int, float> > brMap;

  TString bfragWgtUrl(era+"/bfragweights.root");
  gSystem->ExpandPathName(bfragWgtUrl);
  TFile *fIn=TFile::Open(bfragWgtUrl);
  bfragMap["semilepbrUp"] = (TGraph *)fIn->Get("semilepbrUp");
  bfragMap["semilepbrDown"] = (TGraph *)fIn->Get("semilepbrDown");
  
  Double_t x,y;
  for (auto const& gr : bfragMap) {
    for (int i = 0; i < gr.second->GetN(); ++i) {
      gr.second->GetPoint(i,x,y);
      brMap[gr.first][int(x)] = float(y);
    }
  }

  fIn->Close();
  
  return brMap;
}

float computeSemilepBRWeight(MiniEvent_t &ev, std::map<int, float> corr, int pid, bool useabs) {
  float weight = 1.;
  for (int i = 0; i < ev.ng; i++) {
    cout << ev.g_isSemiLepBhad[i] << endl;
    if (!ev.g_isSemiLepBhad[i]) continue;
    cout << ev.g_bid[i] << " " << corr.count(ev.g_bid[i]) << endl;
    if (corr.count(ev.g_bid[i]) == 0) continue;
    if (!useabs and (pid == 0 or pid == ev.g_bid[i])) weight *= corr[ev.g_bid[i]];
    else if (useabs and (pid == 0 or pid == abs(ev.g_bid[i]))) {
      weight *= (corr[ev.g_bid[i]]+corr[-ev.g_bid[i]])/2.;
    }
  }
  cout << weight << endl;
  return weight;
}

//
TF1 *getRBW(float m,float g) {
  //define the relativistic Breit-Wigner function
  TF1 *bwigner=new TF1("bwigner",
                       "[0]*([1]*[2]*sqrt([1]*[1]*([1]*[1]+[2]*[2]))/sqrt([1]*[1]+sqrt([1]*[1]*([1]*[1]+[2]*[2]))))/(TMath::Power(x*x-[1]*[1],2)+TMath::Power([1]*[2],2))",
                       max(float(0.),m-50*g),m+50*g);

  bwigner->SetParName(0,"N");
  bwigner->SetParameter(0,1.0);
  bwigner->SetParName(1,"m_{0}");
  bwigner->FixParameter(1,m);
  bwigner->SetParName(2,"#Gamma_{t}");
  bwigner->FixParameter(2,g);

  return bwigner;
}

//
float weightBW(TF1 *bwigner,std::vector<float> &obsm,float g,float m,float gini,float mini) {

  //
  if(bwigner==NULL) return 1.;

  bwigner->FixParameter(1,mini);
  bwigner->FixParameter(2,gini);
  float nini=bwigner->Integral(max(m-50*g,float(0.)),m+50*g);
      
  bwigner->FixParameter(1,m);
  bwigner->FixParameter(2,g);
  float n=bwigner->Integral(max(m-50*g,float(0.)),m+50*g);

  float wgt(1.0);
  for(auto m : obsm){
    bwigner->FixParameter(1,mini);
    bwigner->FixParameter(2,gini);
    float vini=bwigner->Eval(m);

    bwigner->FixParameter(1,m);
    bwigner->FixParameter(2,g);
    float v=bwigner->Eval(m);

    wgt *= (v/n) / (vini/nini);
  }

  return wgt;
}
