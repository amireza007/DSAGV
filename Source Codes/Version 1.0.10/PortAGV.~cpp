//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "PortAGV.h"
#include "OpenPort.h"
#include "MCFModel1_3.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TPortAGVForm *PortAGVForm;
//---------------------------------------------------------------------------
__fastcall TPortAGVForm::TPortAGVForm(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TPortAGVForm::FormActivate(TObject *Sender)
{
         ListBox1->Clear();
         OpenPortForm->Table2->First();
         while (!OpenPortForm->Table2->Eof)
         {
            ListBox1->Items->Add(OpenPortForm->Table2->FieldByName("Portname")->AsString);
            OpenPortForm->Table2->Next();
         }
}
//---------------------------------------------------------------------------
void __fastcall TPortAGVForm::BitBtn1Click(TObject *Sender)
{
        DataSource1->Enabled = false;
        Close();
}
//---------------------------------------------------------------------------
void __fastcall TPortAGVForm::ListBox1Click(TObject *Sender)
{
     TLocateOptions Opts;
     int i1,i2;
     AnsiString AGVNameStrS , StartLocationStrS;

     DataSource1->Enabled = true;

     Opts.Clear();
     Variant locvalues[2];
     locvalues[0] = ListBox1->Items->Strings[ListBox1->ItemIndex];

     bool b = OpenPortForm->Table2->Locate("Portname", VarArrayOf(locvalues, 0), Opts);
     if (b)
     {
         Edit1->Text  = OpenPortForm->Table2->FieldByName("NumberOfBlockYard")->AsString;
         Edit2->Text  = OpenPortForm->Table2->FieldByName("NumberOfWorkingPosition")->AsString;
         Edit4->Text  = OpenPortForm->Table2->FieldByName("NumberOfAGVs")->AsString;
     }

     Opts.Clear();
     locvalues[0] = ListBox1->Items->Strings[ListBox1->ItemIndex];

     b = Table1->Locate("Portname", VarArrayOf(locvalues, 0), Opts);

     if(b == false )
     {
       for(int i=1; i<=StrToInt(Edit4->Text);i++)
       {
          int r = 1+random(2);
          switch(r)
          {
             case 1: // AGV is currently in the Yard
                i1 = 1+ random(StrToInt(Edit1->Text));
                StartLocationStrS = "Block "; StartLocationStrS = StartLocationStrS + i1;
                break;
             case 2: // Berth to Yard
                i1 = 1+ random(StrToInt(Edit2->Text));
                StartLocationStrS = "W/P "; StartLocationStrS = StartLocationStrS + i1;
                break;
          }
          Table1->Append();
          AnsiString AGVNameStrS = "AGV-"; AGVNameStrS = AGVNameStrS + i;
          Table1->FieldValues["AGVName"]= AGVNameStrS;
          Table1->FieldValues["Portname"]= ListBox1->Items->Strings[ListBox1->ItemIndex];
          Table1->FieldValues["AGVNumber"]= i;
          Table1->FieldValues["StartLocation"]= StartLocationStrS;
          Table1->FieldValues["Load"]= false;
          Table1->FieldValues["ReadyTime"]= 1+random(StrToInt(Edit6->Text));
          Table1->Post();
       }
     }
     Table2->Filter =   AnsiString("Portname = '") + ListBox1->Items->Strings[ListBox1->ItemIndex] + "'";
     Table2->Filtered = true;
}
//---------------------------------------------------------------------------

void __fastcall TPortAGVForm::Table2AfterRefresh(TDataSet *DataSet)
{
   if(PortAGVForm->Table2->RecNo > 0 )
   {
     AnsiString AGVName =  PortAGVForm->Table2->FieldValues["AGVName"];
     MCFAlgorithmForm->Label31->Caption = "Container Jobs for " + AGVName;
     MCFAlgorithmForm->Label31->Refresh() ;
   }

}
//---------------------------------------------------------------------------

void __fastcall TPortAGVForm::FormClose(TObject *Sender,
      TCloseAction &Action)
{
        DataSource1->Enabled = false;
}
//---------------------------------------------------------------------------


