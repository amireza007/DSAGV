//---------------------------------------------------------------------------

#ifndef Unit1H
#define Unit1H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <math.h>
#include <jpeg.hpp>
#include <Menus.hpp>
//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// IDE-managed Components
        TLabel *Label3;
        TEdit *Digitalformat;
        TEdit *Hourbox;
        TEdit *Minutebox;
        TEdit *Secondbox;
        TTimer *Timer1;
        TPopupMenu *PopupMenu1;
        TMenuItem *DigitalAnalog1;
        TMenuItem *N1;
        TMenuItem *SetAlarm1;
        TImage *Image1;
        TImage *Image2;
        TImage *Image3;
        TImage *Image4;
        void __fastcall Timer1Timer(TObject *Sender);
        void __fastcall DisplaybuttonClick(TObject *Sender);
        void __fastcall AlarmbuttonClick(TObject *Sender);
        void __fastcall SetAlarm1Click(TObject *Sender);
        void __fastcall Image2Click(TObject *Sender);
private:	// User declarations
        //typedef enum {SecHand, MinHand, HourHand, All} FirstDraw;
        typedef enum {HourHand, MinuteHand, SecondHand, All} FirstDraw;
        int MinAngle, HourAngle, MinArm, HourArm, ClockBoarder;
        bool AlarmSet, AnalogMode, FullClock;
        float DegToRad, AngSec, ArmSec;
        //int AngMin, ArmMin, ArmHour, AngHour;
        FirstDraw DrawHand;
        TPoint ClockCenter, CoordSec, CoordMin, CoordHour;
        TDateTime dtNow, dtCurrent, dtAlarm;
        TTimer* FTimer;
        void __fastcall TForm1::Painta();
        void __fastcall TForm1::DrawClock(int,int,int);
public:		// User declarations
        __fastcall TForm1(TComponent* Owner);
        void __fastcall TForm1::SetAlarm(int,int,int,int);
        void __fastcall TForm1::GetAlarm();
        void __fastcall TForm1::DisplayAlarm();
        void __fastcall TForm1::SwitchMode();
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
 