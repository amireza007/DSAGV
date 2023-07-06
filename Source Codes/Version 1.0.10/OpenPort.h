//---------------------------------------------------------------------------

#ifndef OpenPortH
#define OpenPortH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <DB.hpp>
#include <DBCtrls.hpp>
#include <DBGrids.hpp>
#include <DBTables.hpp>
#include <ExtCtrls.hpp>
#include <Grids.hpp>
#include <Db.hpp>
//---------------------------------------------------------------------------
class TOpenPortForm : public TForm
{
__published:	// IDE-managed Components
        TDataSource *DataSource1;
        TTable *Table2;
        TDBGrid *DBGrid1;
        TDBNavigator *DBNavigator1;
        TDatabase *PortDatabase;
        TButton *Button1;
        TStringField *Table2Portname;
        TFloatField *Table2NumberOfBlockYard;
        TFloatField *Table2NumberOfWorkingPosition;
        TStringField *Table2Description;
        TFloatField *Table2NumberOfAGVs;
        TFloatField *Table2MaxDistance;
        TFloatField *Table2W1;
        TFloatField *Table2W2;
        TFloatField *Table2NumberOfZones;
        TFloatField *Table2NumberOfLanes;
        TFloatField *Table2NumberOfJunctions;
        void __fastcall Button1Click(TObject *Sender);
        void __fastcall Table2BeforeDelete(TDataSet *DataSet);
private:	// User declarations
public:		// User declarations
        __fastcall TOpenPortForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TOpenPortForm *OpenPortForm;
//---------------------------------------------------------------------------
#endif
