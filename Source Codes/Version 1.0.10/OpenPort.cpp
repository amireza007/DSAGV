//---------------------------------------------------------------------------

//#include <vcl.h>
#pragma hdrstop

#include "OpenPort.h"
#include "PortLayout.h"
#include "PortContainer.h"
#include "PortAGV.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TOpenPortForm *OpenPortForm;
//---------------------------------------------------------------------------
__fastcall TOpenPortForm::TOpenPortForm(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TOpenPortForm::Button1Click(TObject *Sender)
{
     Close();
}
//---------------------------------------------------------------------------


void __fastcall TOpenPortForm::Table2BeforeDelete(TDataSet *DataSet)
{

       Set  <TLocateOption,0,1> flags;
       flags << loCaseInsensitive << loPartialKey;

       Variant locvalues[2];

       // Deleting any port layout information from Layout Files
       locvalues[0] = Table2->FieldByName("portname")->AsString;
       bool b = PortLayoutForm->Table1->Locate("Portname",Table2->FieldByName("portname")->AsString , flags);
       while(b)
       {
         PortLayoutForm->Table1->Delete();
         bool b = PortLayoutForm->Table1->Locate("Portname", Table2->FieldByName("portname")->AsString , flags);
         if(PortLayoutForm->Table1->FieldByName("Portname")->AsString != Table2->FieldByName("portname")->AsString)
            break ;
         if (PortLayoutForm->Table1->Eof ) break;
       }

       // Deleting container Jobs from Container Job Files
       locvalues[0] = Table2->FieldByName("portname")->AsString;

       b = PortContainerForm->Table1->Locate("Portname",Table2->FieldByName("portname")->AsString , flags);
       while(b)
       {
         PortContainerForm->Table1->Delete();
         bool b = PortContainerForm->Table1->Locate("Portname", Table2->FieldByName("portname")->AsString , flags);
         if(PortContainerForm->Table1->FieldByName("Portname")->AsString != Table2->FieldByName("portname")->AsString)
            break ;
         if (PortContainerForm->Table1->Eof ) break;
       }

       // Deleting AGVs from AGV Files
       locvalues[0] = Table2->FieldByName("portname")->AsString;

       b = PortAGVForm->Table1->Locate("Portname",Table2->FieldByName("portname")->AsString , flags);
       while(b)
       {
         PortAGVForm->Table1->Delete();
         bool b = PortAGVForm->Table1->Locate("Portname", Table2->FieldByName("portname")->AsString , flags);
         if(PortAGVForm->Table1->FieldByName("Portname")->AsString != Table2->FieldByName("portname")->AsString)
            break ;
         if (PortAGVForm->Table1->Eof ) break;
       }
}
//---------------------------------------------------------------------------


