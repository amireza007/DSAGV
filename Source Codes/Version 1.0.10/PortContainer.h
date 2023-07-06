//---------------------------------------------------------------------------

#ifndef PortContainerH
#define PortContainerH
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
#include <Graphics.hpp>
#include <Db.hpp>
//---------------------------------------------------------------------------
class TPortContainerForm : public TForm
{
__published:	// IDE-managed Components
        TBitBtn *OK;
        TGroupBox *GroupBox3;
        TLabel *Label4;
        TLabel *Label5;
        TDBGrid *DBGrid1;
        TDBNavigator *DBNavigator1;
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
        TDataSource *DataSource1;
        TTable *Table1;
        TTable *Table2;
        TStringField *Table1PortName;
        TStringField *Table1ContainerID;
        TStringField *Table1SourcePoint;
        TStringField *Table1DestPoint;
        TStringField *Table2PortName;
        TStringField *Table2ContainerID;
        TStringField *Table2SourcePoint;
        TStringField *Table2DestPoint;
        TFloatField *Table2ReadyTime;
        TFloatField *Table1ReadyTime;
        TBitBtn *BitBtn1;
        TGroupBox *GroupBox4;
        TImage *Image4;
        TEdit *Edit3;
        TGroupBox *GroupBox1;
        TImage *Image2;
        TEdit *Edit1;
        TGroupBox *GroupBox2;
        TImage *Image1;
        TEdit *Edit2;
        TFloatField *Table2QuayCraneTime;
        TFloatField *Table1QuayCraneTime;
        TBlobField *Table2Late;
        TBlobField *Table2Done;
        TBlobField *Table1Late;
        TBlobField *Table1Done;
        void __fastcall BitBtn1Click(TObject *Sender);
        void __fastcall ListBox1Click(TObject *Sender);
        void __fastcall FormActivate(TObject *Sender);
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
private:	// User declarations
public:		// User declarations
        __fastcall TPortContainerForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TPortContainerForm *PortContainerForm;
//---------------------------------------------------------------------------
#endif
