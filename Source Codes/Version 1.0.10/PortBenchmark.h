//---------------------------------------------------------------------------

#ifndef PortBenchmarkH
#define PortBenchmarkH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
class TBenchOptionForm : public TForm
{
__published:	// IDE-managed Components
        TGroupBox *GroupBox1;
        TLabel *Label1;
        TLabel *Label2;
        TEdit *Edit1;
        TEdit *Edit2;
        TGroupBox *GroupBox2;
        TGroupBox *GroupBox3;
        TRadioButton *RadioButton1;
        TRadioButton *RadioButton2;
        TRadioButton *RadioButton3;
        TLabel *Label3;
        TEdit *Edit3;
        TButton *Button1;
        TGroupBox *GroupBox4;
        TLabel *Label5;
        TLabel *Label6;
        TEdit *Edit5;
        TEdit *Edit6;
        TCheckBox *CheckBox1;
        TButton *Button2;
        void __fastcall Button1Click(TObject *Sender);
        void __fastcall Button2Click(TObject *Sender);
        void __fastcall RadioButton1Click(TObject *Sender);
        void __fastcall RadioButton2Click(TObject *Sender);
        void __fastcall RadioButton3Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall TBenchOptionForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TBenchOptionForm *BenchOptionForm;
//---------------------------------------------------------------------------
#endif
