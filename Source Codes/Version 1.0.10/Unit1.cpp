//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit1.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
        : TForm(Owner)
{
		ArmSec=Form1->Width / 5;
		MinArm=(4 * ArmSec) / 5;
		HourArm=(2* ArmSec) / 5;
		ClockCenter.x=(Form1->Width/2);
		ClockCenter.y=(Form1->Height/2);
		Digitalformat->Top = ClockCenter.y -(Digitalformat->Height/2);
		Digitalformat->Left = ClockCenter.x - (Digitalformat->Width/2);
		CoordSec.x=ClockCenter.x;
		CoordSec.y=ClockCenter.y;
		CoordMin.x=ClockCenter.x;
		CoordMin.y=ClockCenter.y;
		CoordHour.x=ClockCenter.x;
		CoordHour.y=ClockCenter.y;
		DegToRad = (3.1416/180);
		ClockBoarder=ArmSec + 3;
		AnalogMode = true;
		AlarmSet = false;
		FullClock = true;
		Digitalformat->Visible = false;
		DrawHand = SecondHand;

}
//---------------------------------------------------------------------------

void __fastcall TForm1::Painta()
{
		Word NHour, NMin, NSec, NMSec, CHour, CMin, CSec, CMSec, AHour, AMin, ASec, AMSec;

		dtNow = Now();
		DecodeTime(dtNow, NHour, NMin, NSec, NMSec);
		DecodeTime(dtCurrent, CHour, CMin, CSec, CMSec);
		Secondbox->Text = IntToStr(NSec);
		if (AlarmSet)
               	{
			DecodeTime (dtAlarm,AHour,AMin,ASec,AMSec);
			if (NHour == AHour  && NMin == AMin && NSec == 0)
			DisplayAlarm();
		}
		if (AnalogMode)
		{
			Canvas->Pen->Color = clBlack;
			Canvas->Pen->Mode = pmNotXor;
			if (DrawHand > SecondHand)
			{
				Canvas->MoveTo(ClockCenter.x,ClockCenter.y);
				Canvas->LineTo(CoordSec.x,CoordSec.y);
			}
			else
			{
				if (FullClock)
				{
				    DrawClock(12,30,5);
				}
				else
				{
				    DrawClock(4,90,5);
				}
				DrawClock(60,6,2);
				DrawHand = MinuteHand;
			}
			AngSec = ((float (NSec)) + (float (NMSec)/1000)) * 6;
			CoordSec.x = ClockCenter.x + (ArmSec * sin(AngSec * DegToRad));
			CoordSec.y = ClockCenter.y - (ArmSec * cos(AngSec * DegToRad));
			Canvas->MoveTo(ClockCenter.x,ClockCenter.y);
			Canvas->LineTo(CoordSec.x,CoordSec.y);

			if (NMin!=CMin)
			{
				dtCurrent = EncodeTime((Word)CHour,(Word)NMin,(Word)NSec,(Word)0);
				Minutebox->Text = IntToStr(NMin);
				Hourbox->Text = IntToStr(NHour);
				MinAngle = NMin * 6;
				Canvas->Pen->Width = 2;
				Canvas->Pen->Color = clWindow;
				if (DrawHand > MinuteHand)
				{
					Canvas->MoveTo(ClockCenter.x,ClockCenter.y);
					Canvas->LineTo(CoordMin.x,CoordMin.y);
				}
				else
					DrawHand = HourHand;
				CoordMin.x = ClockCenter.x + (MinArm * sin(MinAngle * DegToRad));
				CoordMin.y = ClockCenter.y - (MinArm * cos(MinAngle * DegToRad));
				Canvas->MoveTo(ClockCenter.x,ClockCenter.y);
				Canvas->LineTo(CoordMin.x,CoordMin.y);
				if(NHour!=CHour)
				{
					HourAngle = NHour * 30;
					Canvas->Pen->Width = 2;
					if (DrawHand > HourHand)
					{
						Canvas->MoveTo(ClockCenter.x,ClockCenter.y);
						Canvas->LineTo(CoordHour.x,CoordHour.y);
					}
					else
						DrawHand=All;
					CoordHour.x = ClockCenter.x + (HourArm * sin(HourAngle * DegToRad));
					CoordHour.y = ClockCenter.y - (HourArm * cos(HourAngle * DegToRad));
					Canvas->MoveTo(ClockCenter.x,ClockCenter.y);
					Canvas->LineTo(CoordHour.x,CoordHour.y);
				}
				Canvas->Pen->Width = 2;
			}
	}
	else
	{
		Digitalformat->Text = TimeToStr(dtNow);
	}
}
//---------------------------------------------------------------------------
void __fastcall TForm1::DrawClock(int Segments,int SegmentSize,int IncrementLength)
{
		TPoint IncrementA,IncrementB;
		int Angle;
		Canvas->Pen->Width = 2;
		Canvas->Pen->Mode = pmBlack;
		Canvas->Brush->Style = bsClear;
		Canvas->Ellipse(ClockCenter.x-ClockBoarder,ClockCenter.y-ClockBoarder,ClockCenter.x+ClockBoarder,ClockCenter.y+ClockBoarder);
		Canvas->Pen->Width = 2;
		for(int Cycle=0; Cycle < Segments; Cycle++)
		{
			Angle = (Cycle * SegmentSize) + SegmentSize;
			IncrementA.x = ClockCenter.x + ((ClockBoarder-5) * sin(Angle * DegToRad));
			IncrementA.y = ClockCenter.y - ((ClockBoarder-5) * cos(Angle * DegToRad));
			IncrementB.x = IncrementA.x + (IncrementLength * sin(Angle * DegToRad));
			IncrementB.y = IncrementA.y - (IncrementLength * cos(Angle * DegToRad));
			Canvas->MoveTo(IncrementA.x,IncrementA.y);
			Canvas->LineTo(IncrementB.x,IncrementB.y);
			if (Segments==12  || Segments == 4)
			{
				IncrementB.x = (IncrementA.x + (4*IncrementLength * sin(Angle * DegToRad))-4);
				IncrementB.y = (IncrementA.y - (4*IncrementLength * cos(Angle * DegToRad))-4);
				Canvas->TextOut(IncrementB.x,IncrementB.y,IntToStr(Angle/30));
			}
		}
		Canvas->Pen->Mode = pmNotXor;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Timer1Timer(TObject *Sender)
{
		Painta();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::DisplaybuttonClick(TObject *Sender)
{
		SwitchMode();
}
//---------------------------------------------------------------------------


void __fastcall TForm1::AlarmbuttonClick(TObject *Sender)
{
		SetAlarm(StrToInt(Hourbox->Text),StrToInt(Minutebox->Text),StrToInt(Secondbox->Text),0);
		Hourbox->Visible =!Hourbox->Visible;
		Minutebox->Visible =!Minutebox->Visible;
		Image1->Visible =!Image1->Visible;
		Image3->Visible =!Image3->Visible;
		Image4->Visible =!Image4->Visible;
}
//---------------------------------------------------------------------------
void __fastcall  TForm1::SetAlarm(int AHour,int AMin,int ASec,int AMSec)
{
		  try
		  {
			  dtAlarm = EncodeTime((Word)AHour,(Word)AMin,(Word)ASec,(Word)0);
			  DecodeTime(dtAlarm, AHour, AMin, ASec, AMSec);
			  Label3->Caption = "Alarm Set For " + IntToStr(AHour) +":" + IntToStr(AMin);
			  AlarmSet = true;
		  }
		  catch(...)
		  {
			  Application->MessageBox("Invalid time entered.", "Invalid Time", MB_OK);
		  }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::GetAlarm()
{
        if (AlarmSet)
        {
                Word AHour, AMin, ASec, AMSec;
                DecodeTime(dtAlarm, AHour, AMin, ASec, AMSec);
        }
}
//---------------------------------------------------------------------------
void __fastcall TForm1::DisplayAlarm()
{
		if (AlarmSet)
        {
	        AlarmSet = false;
	        Label3->Caption = "";
	        Application->MessageBox("ALARM!!! Alarm was set to go off at this time", "Alarm", MB_OK);
        }
}//---------------------------------------------------------------------------
void __fastcall TForm1::SwitchMode()
{
		int Extra (10);
		Canvas->Pen->Mode = pmCopy;
		//Canvas->Pen->Color = clBtnFace;
		Canvas->Pen->Color = clNone;
		Canvas->Brush->Style = bsSolid;
		Canvas->Brush->Color = clBtnFace;
		Canvas->Brush->Color = clNone;
		//Canvas->Ellipse(ClockCenter.x-ClockBoarder-Extra,ClockCenter.y-ClockBoarder-Extra,ClockCenter.x+ClockBoarder+Extra,ClockCenter.y+ClockBoarder+Extra);
		Digitalformat->Visible = !Digitalformat->Visible;
		DrawHand = SecondHand;
		dtCurrent = EncodeTime((Word)0, (Word)0, (Word)0, (Word)0);
		AnalogMode = !AnalogMode;
}
//---------------------------------------------------------------------------



void __fastcall TForm1::SetAlarm1Click(TObject *Sender)
{
		Hourbox->Visible =!Hourbox->Visible;
		Minutebox->Visible =!Minutebox->Visible;
		Image1->Visible =!Image1->Visible;
		Image3->Visible =!Image3->Visible;
		Image4->Visible =!Image4->Visible;
}
//---------------------------------------------------------------------------


void __fastcall TForm1::Image2Click(TObject *Sender)
{
		Application->Terminate();
}
//---------------------------------------------------------------------------

