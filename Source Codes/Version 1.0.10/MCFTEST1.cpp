//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "MCFTEST1.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TAppletModule1 *AppletModule1;
//---------------------------------------------------------------------------
__fastcall TAppletModule1::TAppletModule1(TComponent* Owner)
        : TAppletModule(Owner)
{
}
//---------------------------------------------------------------------------