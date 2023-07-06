//---------------------------------------------------------------------------

#ifndef PortAgvsH
#define PortAgvsH
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
#include <Grids.hpp>
#include <jpeg.hpp>
//---------------------------------------------------------------------------
class TPortAGVForm : public TForm
{
__published:	// IDE-managed Components
        TGroupBox *GroupBox1;
        TImage *Image2;
        TEdit *Edit1;
        TGroupBox *GroupBox2;
        TImage *Image1;
        TEdit *Edit2;
        TBitBtn *OK;
        TGroupBox *GroupBox3;
        TLabel *Label4;
        TLabel *Label5;
        TDBGrid *DBGrid1;
        TDBNavigator *DBNavigator1;
        TGroupBox *GroupBox7;
        TLabel *Label3;
        TEdit *Edit6;
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
        TBitBtn *BitBtn1;
        TDataSource *DataSource1;
        TDatabase *PortDatabase;
        TTable *Table1;
        TTable *Table2;
        TStringField *Table2PortName;
        TFloatField *Table2ReadyTime;
        TStringField *Table2AGVName;
        TStringField *Table2StartLocation;
        TStringField *Table1PortName;
        TStringField *Table1AGVName;
        TStringField *Table1StartLocation;
        TFloatField *Table1ReadyTime;
private:	// User declarations
public:		// User declarations
        __fastcall TPortAGVForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TPortAGVForm *PortAGVForm;
//---------------------------------------------------------------------------
#endif
