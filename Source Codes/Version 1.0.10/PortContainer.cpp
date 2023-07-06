//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "PortContainer.h"
#include "OpenPort.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TPortContainerForm *PortContainerForm;
//---------------------------------------------------------------------------
__fastcall TPortContainerForm::TPortContainerForm(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TPortContainerForm::BitBtn1Click(TObject *Sender)
{
     DataSource1->Enabled = false;
     Close();
}
//---------------------------------------------------------------------------
void __fastcall TPortContainerForm::ListBox1Click(TObject *Sender)
{
     TLocateOptions Opts;
     int i1,i2, TWindow;
     AnsiString BlockStrS , BlockStrD;

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

     Table2->Filter   =   AnsiString("Portname = '") + ListBox1->Items->Strings[ListBox1->ItemIndex] + "'";
     Table2->Filtered =  true;
     Edit3->Text = Table2->RecordCount ;
}
//---------------------------------------------------------------------------
void __fastcall TPortContainerForm::FormActivate(TObject *Sender)
{
     PortContainerForm->ListBox1->Clear();
     OpenPortForm->Table2->First();
     while (!OpenPortForm->Table2->Eof)
     {
        PortContainerForm->ListBox1->Items->Add(OpenPortForm->Table2->FieldByName("Portname")->AsString);
        OpenPortForm->Table2->Next();
     }
}
//---------------------------------------------------------------------------

void __fastcall TPortContainerForm::FormClose(TObject *Sender,
      TCloseAction &Action)
{
     DataSource1->Enabled = false;
}
//---------------------------------------------------------------------------

