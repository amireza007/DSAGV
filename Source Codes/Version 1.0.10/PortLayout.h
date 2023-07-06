//---------------------------------------------------------------------------

#ifndef PortLayoutH
#define PortLayoutH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include <jpeg.hpp>
#include <Grids.hpp>
#include <DB.hpp>
#include <DBCtrls.hpp>
#include <DBGrids.hpp>
#include <DBTables.hpp>
#include <Graphics.hpp>
#include <Db.hpp>
//---------------------------------------------------------------------------
class TPortLayoutForm : public TForm
{
__published:	// IDE-managed Components
        TDataSource *DataSource1;
        TTable *Table1;
        TGroupBox *GroupBox3;
        TDBGrid *DBGrid1;
        TDBNavigator *DBNavigator1;
        TGroupBox *GroupBox7;
        TLabel *Label3;
        TEdit *Edit6;
        TTable *Table2;
        TStringField *Table2PortName;
        TStringField *Table2SourcePoint;
        TStringField *Table2DestPoint;
        TFloatField *Table2Distance;
        TListBox *ListBox1;
        TLabel *Label4;
        TLabel *Label5;
        TBitBtn *OK;
        TStringField *Table1PortName;
        TStringField *Table1SourcePoint;
        TStringField *Table1DestPoint;
        TFloatField *Table1Distance;
        TGroupBox *GroupBox1;
        TImage *Image2;
        TEdit *Edit1;
        TGroupBox *GroupBox2;
        TImage *Image1;
        TEdit *Edit2;
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
        TDBGrid *DBGrid2;
        TDBNavigator *DBNavigator2;
        TDataSource *DataSource2;
        TTable *Table3;
        TTable *Table4;
        TStringField *Table3PortName;
        TStringField *Table4PortName;
        TLabel *Label6;
        TLabel *Label7;
        TGroupBox *GroupBox4;
        TImage *Image4;
        TLabel *Label8;
        TEdit *Edit3;
        TLabel *Label9;
        TFloatField *Table3Junction;
        TStringField *Table3Location;
        TFloatField *Table3NextJunc1;
        TFloatField *Table3NextLane1;
        TFloatField *Table3DurationLane1;
        TFloatField *Table3NextJunc2;
        TFloatField *Table3NextLane2;
        TFloatField *Table3Durationlane2;
        TFloatField *Table3NextJunc3;
        TFloatField *Table3NextLane3;
        TFloatField *Table3DurationLane3;
        TFloatField *Table4Junction;
        TStringField *Table4Location;
        TFloatField *Table4NextJunc1;
        TFloatField *Table4NextLane1;
        TFloatField *Table4DurationLane1;
        TFloatField *Table4NextJunc2;
        TFloatField *Table4NextLane2;
        TFloatField *Table4Durationlane2;
        TFloatField *Table4NextJunc3;
        TFloatField *Table4NextLane3;
        TFloatField *Table4DurationLane3;
        TLabel *Label10;
        TEdit *Edit7;
        TBitBtn *BitBtn1;
        TBitBtn *BitBtn2;
        void __fastcall OKClick(TObject *Sender);
        void __fastcall ListBox1Click(TObject *Sender);
        void __fastcall FormActivate(TObject *Sender);
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
        void __fastcall BitBtn1Click(TObject *Sender);
        void __fastcall BitBtn2Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall TPortLayoutForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TPortLayoutForm *PortLayoutForm;
//---------------------------------------------------------------------------
#endif
