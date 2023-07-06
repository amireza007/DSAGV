//---------------------------------------------------------------------------
#ifndef MainH
#define MainH
//---------------------------------------------------------------------------
#include <sysutils.hpp>
#include <windows.hpp>
#include <messages.hpp>
#include <sysutils.hpp>
#include <classes.hpp>
#include <graphics.hpp>
#include <controls.hpp>
#include <forms.hpp>
#include <dialogs.hpp>
#include <stdctrls.hpp>
#include <buttons.hpp>
#include <extctrls.hpp>
#include <menus.hpp>
#include <Classes.hpp>
#include <ComCtrls.hpp>
#include <Controls.hpp>
#include <Dialogs.hpp>
#include <Menus.hpp>
#include <ExtCtrls.hpp>
#include <jpeg.hpp>
#include <StdCtrls.hpp>
#include <ImgList.hpp>
#include <ToolWin.hpp>
#include <Graphics.hpp>
#include <Buttons.hpp>
#include "Word_2K_SRVR.h"
#include <OleServer.hpp>
#include "PowerPoint_2K_SRVR.h"
#include <OleCtnrs.hpp>
//---------------------------------------------------------------------------
class TMainForm : public TForm
{
__published:   
	TMainMenu *MainMenu;
	TMenuItem *FileOpenItem;
	TMenuItem *FileExitItem;
	TMenuItem *WindowTileItem;
	TMenuItem *WindowCascadeItem;
	TMenuItem *WindowArrangeItem;
	TMenuItem *HelpContentsItem;
	TMenuItem *HelpSearchItem;
	TMenuItem *HelpHowToUseItem;
	TMenuItem *HelpAboutItem;
	TStatusBar *StatusLine;
	TOpenDialog *OpenDialog;
	TSaveDialog *SaveDialog;
	TPrintDialog *PrintDialog;
	TPrinterSetupDialog *PrintSetupDialog;
        TMenuItem *Options1;
        TMenuItem *ContainersJobMenu;
        TImageList *ImageList1;
        TMenuItem *N9;
        TMenuItem *LayoutandEquipmentinPort1;
        TMenuItem *N7;
        TMenuItem *CloseTablesofPorts1;
        TMenuItem *EditPorts1;
        TMenuItem *N8;
        TMenuItem *N10;
        TImage *Image1;
        TLabel *Label2;
        TLabel *Label3;
        TPanel *Panel1;
        TSpeedButton *SpeedButton1;
        TSpeedButton *SpeedButton2;
        TSpeedButton *SpeedButton4;
        TMenuItem *AGVsinThePort1;
        TMenuItem *Process1;
        TMenuItem *MinimumCostFlowAlgorithms1;
        TMenuItem *N4;
        TMenuItem *LINDOAPIAlgorithms1;
        TSpeedButton *SpeedButton6;
        TSpeedButton *SpeedButton8;
        TSpeedButton *SpeedButton9;
        TSpeedButton *SpeedButton3;
        TSpeedButton *SpeedButton5;
        TLabel *Label1;
        TLabel *Label4;
        TLabel *Label5;
        TImage *Image2;
        TImage *Image3;
        TMenuItem *Document1;
        TMenuItem *OpenResearchReport1;
        TMenuItem *N2;
        TMenuItem *OpenSlidesfor1stSUpervisoryBoard1;
        TMenuItem *N5;
        TMenuItem *OpenMy1stPaper1;
        TImage *Image4;
        TSpeedButton *SpeedButton7;
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall ShowHint(TObject *Sender);
	void __fastcall FileOpen(TObject *Sender);
	void __fastcall FilePrint(TObject *Sender);
	void __fastcall FilePrintSetup(TObject *Sender);
	void __fastcall FileExit(TObject *Sender);
	void __fastcall EditUndo(TObject *Sender);
	void __fastcall EditCut(TObject *Sender);
	void __fastcall EditCopy(TObject *Sender);
	void __fastcall EditPaste(TObject *Sender);
	void __fastcall WindowTile(TObject *Sender);
	void __fastcall WindowCascade(TObject *Sender);
	void __fastcall WindowArrange(TObject *Sender);
	void __fastcall HelpContents(TObject *Sender);
	void __fastcall HelpSearch(TObject *Sender);
	void __fastcall HelpHowToUse(TObject *Sender);
	void __fastcall HelpAbout(TObject *Sender);
        void __fastcall LayoutandEquipmentinPort1Click(TObject *Sender);
        void __fastcall CloseTablesofPorts1Click(TObject *Sender);
        void __fastcall EditPorts1Click(TObject *Sender);
        void __fastcall ContainersJobMenuClick(TObject *Sender);
        void __fastcall MinimumCostFlow1Click(TObject *Sender);
        void __fastcall AGVsinThePort1Click(TObject *Sender);
        void __fastcall MinimumCostFlowAlgorithms1Click(TObject *Sender);
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
        void __fastcall OpenResearchReport1Click(TObject *Sender);
        void __fastcall OpenMy1stPaper1Click(TObject *Sender);
        void __fastcall OpenSlidesfor1stSUpervisoryBoard1Click(
          TObject *Sender);
        void __fastcall SpeedButton7Click(TObject *Sender);
private:        // private user declarations
public:         // public user declarations
	virtual __fastcall TMainForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern TMainForm *MainForm;
//---------------------------------------------------------------------------
#endif
