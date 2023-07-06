//---------------------------------------------------------------------------

#ifndef MCFModelH
#define MCFModelH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ComCtrls.hpp>
#include <DB.hpp>
#include <DBCtrls.hpp>
#include <DBGrids.hpp>
#include <DBTables.hpp>
#include <ExtCtrls.hpp>
#include <Graphics.hpp>
#include <Grids.hpp>
#include <jpeg.hpp>
#include <Menus.hpp>
#include <Chart.hpp>
#include <DbChart.hpp>
#include <TeEngine.hpp>
#include <TeeProcs.hpp>
#include <Series.hpp>
#include <QRTEE.hpp>
#include <QuickRpt.hpp>
//---------------------------------------------------------------------------
class TMCFAlgorithmForm : public TForm
{
__published:	// IDE-managed Components
        TTabControl *TabControl1;
        TGroupBox *GroupBox3;
        TLabel *Label4;
        TLabel *Label5;
        TDBGrid *DBGrid1;
        TListBox *ListBox1;
        TGroupBox *GroupBox5;
        TLabel *Label1;
        TLabel *Label2;
        TImage *Image3;
        TEdit *Edit4;
        TEdit *Edit5;
        TUpDown *UpDown2;
        TGroupBox *GroupBox6;
        TRadioButton *RadioButton1;
        TRadioButton *RadioButton2;
        TGroupBox *GroupBox4;
        TLabel *Label7;
        TImage *Image4;
        TLabel *Label8;
        TEdit *Edit3;
        TEdit *Edit6;
        TGroupBox *GroupBox7;
        TLabel *Label9;
        TLabel *Label10;
        TDBGrid *DBGrid2;
        TListBox *ListBox3;
        TGroupBox *GroupBox8;
        TLabel *Label12;
        TLabel *Label13;
        TImage *Image5;
        TEdit *Edit7;
        TEdit *Edit8;
        TUpDown *UpDown1;
        TGroupBox *GroupBox9;
        TRadioButton *RadioButton3;
        TRadioButton *RadioButton4;
        TGroupBox *GroupBox12;
        TLabel *Label15;
        TImage *Image8;
        TLabel *Label16;
        TEdit *Edit11;
        TEdit *Edit12;
        TGroupBox *GroupBox13;
        TGroupBox *GroupBox11;
        TLabel *Label18;
        TEdit *Edit9;
        TButton *Button1;
        TButton *Button2;
        TDataSource *DataSource3;
        TGroupBox *GroupBox14;
        TButton *Button6;
        TDBGrid *DBGrid4;
        TLabel *Label11;
        TDBGrid *DBGrid3;
        TGroupBox *GroupBox10;
        TLabel *Label23;
        TLabel *Label24;
        TListBox *ListBox2;
        TListBox *ListBox5;
        TButton *Button5;
        TButton *Button3;
        TButton *Button4;
        TButton *Button7;
        TButton *Button8;
        TTable *Table4;
        TStringField *StringField5;
        TStringField *StringField6;
        TStringField *StringField8;
        TFloatField *FloatField2;
        TGroupBox *GroupBox15;
        TLabel *Label6;
        TImage *Image6;
        TEdit *Edit13;
        TLabel *Label14;
        TEdit *Edit16;
        TLabel *Label17;
        TEdit *Edit17;
        TLabel *Label25;
        TGroupBox *GroupBox1;
        TLabel *Label3;
        TImage *Image1;
        TEdit *Edit1;
        TGroupBox *GroupBox2;
        TImage *Image2;
        TEdit *Edit2;
        TTimer *Timer1;
        TGroupBox *GroupBox16;
        TLabel *Label27;
        TLabel *Label22;
        TButton *Button9;
        TLabel *Label26;
        TTable *Table5;
        TStringField *StringField1;
        TStringField *StringField2;
        TStringField *StringField4;
        TFloatField *FloatField1;
        TIntegerField *IntegerField1;
        TIntegerField *Table4WaitingTime;
        TLabel *Label29;
        TEdit *Edit18;
        TLabel *Label32;
        TImage *Image9;
        TImage *Image10;
        TImage *Image11;
        TImage *Image12;
        TEdit *Digitalformat;
        TEdit *Hourbox;
        TEdit *Minutebox;
        TEdit *Secondbox;
        TPopupMenu *PopupMenu1;
        TMenuItem *DigitalAnalog1;
        TMenuItem *N1;
        TMenuItem *SetAlarm1;
        TLabel *Label30;
        TTimer *Timer3;
        TTimer *Timer2;
        TFloatField *Table4VehicleTime;
        TFloatField *Table4ActualTime;
        TFloatField *Table5VehicleTime;
        TFloatField *Table5ActualTime;
        TLabel *Label31;
        TEdit *Edit19;
        TLabel *Label33;
        TLabel *Label34;
        TEdit *Edit20;
        TStringField *Table4SourcePoint;
        TStringField *Table4DestPoint;
        TStringField *Table5SourcePoint;
        TStringField *Table5DestPoint;
        TButton *Button10;
        TLabel *Label28;
        TEdit *Edit21;
        TDataSource *DataSource1;
        TTable *Table1;
        TStringField *Table1AGVName;
        TStringField *Table1ContainerID;
        TFloatField *Table1VehicleTime;
        TFloatField *Table1ContainerReadyTime;
        TFloatField *Table1ActualTime;
        TStringField *Table1SourcePoint;
        TStringField *Table1DestPoint;
        TGroupBox *GroupBox18;
        TDBGrid *DBGrid5;
        TLabel *Label35;
        TStringField *Table1PortName;
        TLabel *Label36;
        TFloatField *Table5QuayCraneTime;
        TFloatField *Table4QuayCraneTime;
        TFloatField *Table1QuayCraneTime;
        TGroupBox *GroupBox20;
        TRadioButton *RadioButton8;
        TRadioButton *RadioButton9;
        TRadioButton *RadioButton10;
        TEdit *Edit23;
        TLabel *Label42;
        TEdit *Edit26;
        TLabel *Label39;
        TButton *Button11;
        TGroupBox *GroupBox19;
        TButton *Button12;
        TLabel *Label37;
        TLabel *Label38;
        TEdit *Edit22;
        TQRChart *QRChart1;
        TQRDBChart *QRDBChart1;
        TBarSeries *Series3;
        TBarSeries *Series4;
        TBarSeries *Series5;
        TBarSeries *Series6;
        TDBChart *DBChart1;
        TBarSeries *Series1;
        TBarSeries *Series2;
        TBarSeries *Series7;
        TGroupBox *GroupBox17;
        TImage *Image7;
        TLabel *Label19;
        TLabel *Label20;
        TEdit *Edit10;
        TEdit *Edit14;
        TGroupBox *GroupBox21;
        TRadioButton *RadioButton5;
        TRadioButton *RadioButton6;
        TGroupBox *GroupBox22;
        TRadioButton *RadioButton7;
        TRadioButton *RadioButton11;
        TFloatField *Table5AGVNumber;
        TFloatField *Table4AGVNumber;
        TFloatField *Table1AGVNumber;
        TLabel *Label21;
        TEdit *Edit15;
        TProgressBar *ProgressBar1;
        void __fastcall BitBtn1Click(TObject *Sender);
        void __fastcall ListBox1Click(TObject *Sender);
        void __fastcall TabControl1Change(TObject *Sender);
        void __fastcall ListBox3Click(TObject *Sender);
        void __fastcall Button3Click(TObject *Sender);
        void __fastcall Button4Click(TObject *Sender);
        void __fastcall Button8Click(TObject *Sender);
        void __fastcall Button7Click(TObject *Sender);
        void __fastcall RadioButton5Click(TObject *Sender);
        void __fastcall RadioButton6Click(TObject *Sender);
        void __fastcall Button2Click(TObject *Sender);
        void __fastcall Button1Click(TObject *Sender);
        void __fastcall Table4CalcFields(TDataSet *DataSet);
        void __fastcall FormActivate(TObject *Sender);
        void __fastcall Timer1Timer(TObject *Sender);
        void __fastcall Button9Click(TObject *Sender);
        void __fastcall Label22Click(TObject *Sender);
        void __fastcall Timer2Timer(TObject *Sender,int n);

        void __fastcall DisplaybuttonClick(TObject *Sender);
        void __fastcall AlarmbuttonClick(TObject *Sender);
        void __fastcall SetAlarm1Click(TObject *Sender);
        void __fastcall Timer3Timer(TObject *Sender);
        void __fastcall DBGrid4CellClick(TColumn *Column);
        void __fastcall Button10Click(TObject *Sender);
        void __fastcall DBGrid5TitleClick(TColumn *Column);
        void __fastcall Button11Click(TObject *Sender);
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
        void __fastcall DBChart1CanResize(TObject *Sender, int &NewWidth,
          int &NewHeight, bool &Resize);
private:	// User declarations
        typedef enum {HourHand, MinuteHand, SecondHand, All} FirstDraw;
        int MinAngle, HourAngle, MinArm, HourArm, ClockBoarder;
        bool AlarmSet, AnalogMode, FullClock;
        float DegToRad, AngSec, ArmSec;
        FirstDraw DrawHand;
        TPoint ClockCenter, CoordSec, CoordMin, CoordHour;
        TDateTime dtNow, dtCurrent, dtAlarm;
        TTimer* FTimer;

        void __fastcall TMCFAlgorithmForm::Painta();
        void __fastcall TMCFAlgorithmForm::DrawClock(int,int,int);
        void __fastcall TMCFAlgorithmForm::Insert_Containers_To_Table(int Number_Of_Container_Per_Minute,int i2);
public:		// User declarations
        __fastcall TMCFAlgorithmForm(TComponent* Owner);
        void __fastcall TMCFAlgorithmForm::SetAlarm(int,int,int,int);
        void __fastcall TMCFAlgorithmForm::GetAlarm();
        void __fastcall TMCFAlgorithmForm::DisplayAlarm();
        void __fastcall TMCFAlgorithmForm::SwitchMode();
};
//---------------------------------------------------------------------------
extern PACKAGE TMCFAlgorithmForm *MCFAlgorithmForm;
//---------------------------------------------------------------------------
#endif
