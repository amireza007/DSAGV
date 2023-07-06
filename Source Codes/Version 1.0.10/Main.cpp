//---------------------------------------------------------------------------
//#include <vcl.h>
#pragma hdrstop

#include "Main.h"
#include "PortLayout.h"
#include "OpenPort.h"
#include "PortAbout.h"
#include "PortContainer.h"
#include "MCFModel1_2.h"
#include "PortAGV.h"
#include "stdlib.h"
#include "stdio.h"
#include <utilcls.h>
#include  "sysvari.h"

//---------------------------------------------------------------------------
#pragma link "Word_2K_SRVR"
#pragma link "PowerPoint_2K_SRVR"
#pragma resource "*.dfm"
TMainForm *MainForm;

DEVMODE DevMode;
BOOL Status;
DWORD Mode;
HDC hdc;
DWORD bitsperpixel, x, y;


//---------------------------------------------------------------------------
__fastcall TMainForm::TMainForm(TComponent* Owner)
	: TForm(Owner)
{
}
//----------------------------------------------------------------------------
void __fastcall TMainForm::FormCreate(TObject *Sender)
{
        LONG Result;

        DEVMODE MyDevMode;
        BOOL MyStatus;
        DWORD MyMode;
        HDC Myhdc;
        DWORD Mybitsperpixel, Myx, Myy;

	Application->OnHint = ShowHint;

        // Information about the current display mode
        hdc=GetDC(Application->Handle);
        bitsperpixel  = GetDeviceCaps(hdc, BITSPIXEL);
        x             = GetDeviceCaps(hdc, HORZRES);
        y             = GetDeviceCaps(hdc, VERTRES);
        ReleaseDC(Application->Handle, hdc);

        DevMode.dmSize=sizeof(DevMode);

        MyMode = 28;
        MyDevMode.dmSize=sizeof(MyDevMode);
        EnumDisplaySettings(
            NULL, // LPCTSTR lpszDeviceName -> specifies the display device
            MyMode,    // DWORD iModeNum -> specifies the graphics mode
            &MyDevMode // LPDEVMODE lpDevMode	-> points to structure to receive settings
            );

        // See Win32.hlp
        // ChangeDisplaySettings asks the graphic driver to change the display mode
/*        Result=ChangeDisplaySettings(
            &MyDevMode, // LPDEVMODE lpDevMode,
            CDS_UPDATEREGISTRY // DWORD dwflags (change the mode dynamically and
                               //                update the registry)
         );
     */    
}
//----------------------------------------------------------------------------
void __fastcall TMainForm::ShowHint(TObject *Sender)
{
	StatusLine->SimpleText = Application->Hint;
}
//----------------------------------------------------------------------------
void __fastcall TMainForm::FileOpen(TObject *Sender)
{
         OpenPortForm->PortDatabase->Open();

         OpenPortForm->Table2->Open();

         PortLayoutForm->Table2->Open();
         PortLayoutForm->Table1->Open();

         PortLayoutForm->Table4->Open();
         PortLayoutForm->Table3->Open();

         PortContainerForm->Table2->Open();
         PortContainerForm->Table1->Open();

         PortAGVForm->Table1->Open();
         PortAGVForm->Table2->Open();

         MCFAlgorithmForm->Table1->Open();
         MCFAlgorithmForm->Table4->Open();
         MCFAlgorithmForm->Table5->Open();


         MainForm->LayoutandEquipmentinPort1->Enabled = true;

         MainForm->SpeedButton6->Visible = true;
         MainForm->EditPorts1->Enabled = true;

         MainForm->SpeedButton7->Visible = true;
         MainForm->ContainersJobMenu->Enabled = true;

         MainForm->FileOpenItem->Enabled = false;
         MainForm->SpeedButton1->Visible = false;

         MainForm->SpeedButton5->Visible = true;

         MainForm->SpeedButton8->Visible = true;
         MainForm->AGVsinThePort1->Enabled = true;

         MainForm->CloseTablesofPorts1->Enabled = true;

         MainForm->SpeedButton3->Visible = true;
         CloseTablesofPorts1->Enabled = true;

         MainForm->SpeedButton9->Visible = true;
         MainForm->MinimumCostFlowAlgorithms1->Enabled = true;

       //  MessageDlg ("The Tables of Ports are opened\
       //               \nNow Equipment of any Port can be changed by Options Menu ",
       //               mtConfirmation , TMsgDlgButtons() << mbOK,0);

         Application->MessageBox("The Tables of Ports are opened\nNow Options and Process menus are activated to do anything", "Open Tables", MB_OK);

}
//----------------------------------------------------------------------------
void __fastcall TMainForm::FilePrint(TObject *Sender)
{
	if (PrintDialog->Execute())
	{
		//---- Add code to print current file ----
	}
}
//----------------------------------------------------------------------------
void __fastcall TMainForm::FilePrintSetup(TObject *Sender)
{
	PrintSetupDialog->Execute();
}
//----------------------------------------------------------------------------
void __fastcall TMainForm::FileExit(TObject *Sender)
{
       OpenPortForm->Table2->Close();

       PortLayoutForm->Table2->Close();
       PortLayoutForm->Table1->Close();

       PortContainerForm->Table2->Close();
       PortContainerForm->Table1->Close();

       MCFAlgorithmForm->Table1->Close();
       MCFAlgorithmForm->Table4->Close();

       PortAGVForm->Table2->Close();
       PortAGVForm->Table1->Close();

       OpenPortForm->PortDatabase->Close();

       Close();
}
//----------------------------------------------------------------------------
void __fastcall TMainForm::EditUndo(TObject *Sender)
{
	//---- Add code to perform Edit Undo ----
}
//----------------------------------------------------------------------------
void __fastcall TMainForm::EditCut(TObject *Sender)
{
	//---- Add code to perform Edit Cut ----
}
//----------------------------------------------------------------------------
void __fastcall TMainForm::EditCopy(TObject *Sender)
{
	//--- Add code to perform Edit Copy ----
}
//----------------------------------------------------------------------------
void __fastcall TMainForm::EditPaste(TObject *Sender)
{
	//---- Add code to perform Edit Paste ----
}
//----------------------------------------------------------------------------
void __fastcall TMainForm::WindowTile(TObject *Sender)
{
	Tile();
}
//----------------------------------------------------------------------------
void __fastcall TMainForm::WindowCascade(TObject *Sender)
{
	Cascade();
}
//----------------------------------------------------------------------------
void __fastcall TMainForm::WindowArrange(TObject *Sender)
{
	ArrangeIcons();
}
//----------------------------------------------------------------------------
void __fastcall TMainForm::HelpContents(TObject *Sender)
{
	Application->HelpCommand(HELP_CONTENTS, 0);
}
//----------------------------------------------------------------------------
void __fastcall TMainForm::HelpSearch(TObject *Sender)
{
	Application->HelpCommand(HELP_PARTIALKEY, Longint(""));
}
//----------------------------------------------------------------------------
void __fastcall TMainForm::HelpHowToUse(TObject *Sender)
{
	Application->HelpCommand(HELP_HELPONHELP, 0);
}
//----------------------------------------------------------------------------
void __fastcall TMainForm::HelpAbout(TObject *Sender)
{
	//---- Add code to show program's About Box ----
        AboutForm->Show() ;
}
//----------------------------------------------------------------------------

void __fastcall TMainForm::LayoutandEquipmentinPort1Click(TObject *Sender)
{
       PortLayoutForm->Show();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::CloseTablesofPorts1Click(TObject *Sender)
{
       OpenPortForm->Table2->Close();


       PortLayoutForm->Table2->Close();
       PortLayoutForm->Table1->Close();

       PortLayoutForm->Table4->Close();
       PortLayoutForm->Table3->Close();

       PortContainerForm->Table2->Close();
       PortContainerForm->Table1->Close();

       MCFAlgorithmForm->Table4->Close();
       MCFAlgorithmForm->Table5->Close();

       PortAGVForm->Table2->Close();
       PortAGVForm->Table1->Close();

       OpenPortForm->PortDatabase->Close();

       MainForm->LayoutandEquipmentinPort1->Enabled = false;

       MainForm->SpeedButton6->Visible = false;
       MainForm->EditPorts1->Enabled = false;

       MainForm->SpeedButton7->Visible = false;
       MainForm->ContainersJobMenu->Enabled = false;

       MainForm->SpeedButton5->Visible = false;
       MainForm->SpeedButton3->Visible = false;

       MainForm->SpeedButton8->Visible = false;
       MainForm->AGVsinThePort1->Enabled = false;

       CloseTablesofPorts1->Enabled = false;
       CloseTablesofPorts1->Enabled = false;

       FileOpenItem->Enabled = true;
       SpeedButton1->Visible = true;

       MainForm->SpeedButton9->Visible = false;
       MainForm->MinimumCostFlowAlgorithms1->Enabled = false;


    //   MessageDlg ("The Tables of Ports are closed\
     //             \nNow the Equipment of Ports can not be changed",
     //            mtWarning , TMsgDlgButtons() << mbOK,0);


       Application->MessageBox("The Tables of Ports are closed\nNow Options and Process menus are deactivated", "Close Tables", MB_OK);
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::EditPorts1Click(TObject *Sender)
{
       OpenPortForm->Show();
}
//---------------------------------------------------------------------------



void __fastcall TMainForm::ContainersJobMenuClick(TObject *Sender)
{
       PortContainerForm->Show();
}
//---------------------------------------------------------------------------


void __fastcall TMainForm::MinimumCostFlow1Click(TObject *Sender)
{

     MCFAlgorithmForm->Show();
}
//---------------------------------------------------------------------------


void __fastcall TMainForm::AGVsinThePort1Click(TObject *Sender)
{
    PortAGVForm->Show();
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::MinimumCostFlowAlgorithms1Click(TObject *Sender)
{
     MCFAlgorithmForm->Show();

}
//---------------------------------------------------------------------------


void __fastcall TMainForm::FormClose(TObject *Sender, TCloseAction &Action)
{
        LONG Result;
        EnumDisplaySettings(
            NULL, // LPCTSTR lpszDeviceName -> specifies the display device
            Mode,    // DWORD iModeNum -> specifies the graphics mode
            &DevMode // LPDEVMODE lpDevMode	-> points to structure to receive settings
            );

        // See Win32.hlp
        // ChangeDisplaySettings asks the graphic driver to change the display mode
/*        Result=ChangeDisplaySettings(
            &DevMode, // LPDEVMODE lpDevMode,
            CDS_UPDATEREGISTRY // DWORD dwflags (change the mode dynamically and update the registry)
        );
    */
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::OpenResearchReport1Click(TObject *Sender)
{
        Variant wordApp;
        wordApp= Variant::CreateObject("Word.Application");

        Variant wordDocuments = wordApp.OlePropertyGet("Documents");
        wordDocuments.OleProcedure("open","C:\\Documents and Settings\\hrashi\\My Documents\\My Research\\My Report\\Version 20\\Research Report 24.doc");
        wordApp.OlePropertySet("Visible", (Variant) true);
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::OpenMy1stPaper1Click(TObject *Sender)
{
        Variant wordApp;
        wordApp= Variant::CreateObject("Word.Application");

        Variant wordDocuments = wordApp.OlePropertyGet("Documents");
        wordDocuments.OleProcedure("open","C:\\Documents and Settings\\hrashi\\My Documents\\My Research\\My Papers\\First Paper.doc");
        wordApp.OlePropertySet("Visible", (Variant) true);
}
//---------------------------------------------------------------------------

void __fastcall TMainForm::OpenSlidesfor1stSUpervisoryBoard1Click(
      TObject *Sender)
{

         Variant PowerPointApp = CreateOleObject("PowerPoint.Application");

         PowerPointApp.OlePropertySet("Visible", (Variant) true);

         Variant PowerPointDocuments =  PowerPointApp.OlePropertyGet("Presentations");

         PowerPointDocuments.OleFunction("open", "C:\\Documents and Settings\\hrashi\\My Documents\\My research\\My Presentation\\1st supervisory board presentation.PPT");
}
//---------------------------------------------------------------------------


void __fastcall TMainForm::SpeedButton7Click(TObject *Sender)
{
    PortContainerForm->Show();
}
//---------------------------------------------------------------------------

