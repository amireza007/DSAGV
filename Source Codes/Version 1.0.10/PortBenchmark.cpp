//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "PortBenchmark.h"
#include "Global_ext.h"
#include "stdio.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TBenchOptionForm *BenchOptionForm;
//---------------------------------------------------------------------------
__fastcall TBenchOptionForm::TBenchOptionForm(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall TBenchOptionForm::Button1Click(TObject *Sender)
{
       Close() ;
}
//---------------------------------------------------------------------------
void __fastcall TBenchOptionForm::Button2Click(TObject *Sender)
{
     char   Fname3[100];
     sprintf(&Fname3[0],"%s",BenchOptionForm->Edit3->Text);
     Port_Buff.Fout3   = fopen(Fname3,"w+t");
     fclose(Port_Buff.Fout3);
}
//---------------------------------------------------------------------------
void __fastcall TBenchOptionForm::RadioButton1Click(TObject *Sender)
{
       CheckBox1->Enabled  = true;
}
//---------------------------------------------------------------------------

void __fastcall TBenchOptionForm::RadioButton2Click(TObject *Sender)
{
       CheckBox1->Enabled  = false;
       CheckBox1->Checked  = false;
        
}
//---------------------------------------------------------------------------

void __fastcall TBenchOptionForm::RadioButton3Click(TObject *Sender)
{
       CheckBox1->Enabled  = false;
       CheckBox1->Checked  = false;
        
}
//---------------------------------------------------------------------------

