//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include  "Main1_0_5.h"
#include  "PortLayout.h"
#include  "OpenPort.h"
#include  "Global_ext.h"
#include  "Dijkstra.h"
#include  "math.h"
#include  "PortBenchmark.h"
#include  <iostream.h>
#include  <fstream.h>
#include  <stdio.h>
#include  <math.h>
#include  <iomanip.h>


//#define DIJKSTRA_DEBUG

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TPortLayoutForm *PortLayoutForm;

//---------------------------------------------------------------------------
__fastcall TPortLayoutForm::TPortLayoutForm(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall TPortLayoutForm::OKClick(TObject *Sender)
{
    DataSource1->Enabled = false;
    DataSource2->Enabled = false;
    PortLayoutForm->Hide();
}
//---------------------------------------------------------------------------

bool __fastcall Read_Port(String Port)
{
     TLocateOptions Opts;

     Opts.Clear();
     Variant locvalues[2];
     locvalues[0] = Port;

     Port_Buff.Port = Port;
     bool b = OpenPortForm->Table2->Locate("Portname", VarArrayOf(locvalues, 0), Opts);
     if (b)
     {
       Port_Buff.NumberOfBlockYard        = OpenPortForm->Table2->FieldByName("NumberOfBlockYard")->Value;
       Port_Buff.NumberOfWorkingPosition  = OpenPortForm->Table2->FieldByName("NumberOfWorkingPosition")->Value;
       Port_Buff.NumberOfAGVs             = OpenPortForm->Table2->FieldByName("NumberOfAGVs")->Value;
       Port_Buff.NumberOfJunctions        = OpenPortForm->Table2->FieldByName("NumberOfJunctions")->Value;
       Port_Buff.NumberOfLanes            = OpenPortForm->Table2->FieldByName("NumberOfLanes")->Value;

       Port_Buff.TotalEarlyTimes = 0;
       Port_Buff.TotalLateTimes  = 0;
       Port_Buff.TotalQCWTimes   = 0;
       Port_Buff.TotalVWTimes    = 0;
       Port_Buff.TotalVTTimes    = 0;
       Port_Buff.TotalEarlyJobs  = 0;
       Port_Buff.TotalLateJobs   = 0;
       Port_Buff.TotalDfApAc     = 0;
       Port_Buff.TotalDoneJobs   = 0;
       return(true);
     }
     return(false);


}
//---------------------------------------------------------------------------

void __fastcall TPortLayoutForm::ListBox1Click(TObject *Sender)
{
     TLocateOptions Opts;

     DataSource1->Enabled = true;
     DataSource2->Enabled = true;
     Opts.Clear();
     Variant locvalues[2];
     locvalues[0] = ListBox1->Items->Strings[ListBox1->ItemIndex];

     bool b = OpenPortForm->Table2->Locate("Portname", VarArrayOf(locvalues, 0), Opts);
     if (b)
     {
       Edit1->Text  = OpenPortForm->Table2->FieldByName("NumberOfBlockYard")->AsString;
       Edit2->Text  = OpenPortForm->Table2->FieldByName("NumberOfWorkingPosition")->AsString;
       Edit3->Text  = OpenPortForm->Table2->FieldByName("NumberOfJunctions")->AsString;
       Edit4->Text  = OpenPortForm->Table2->FieldByName("NumberOfAGVs")->AsString;
       Edit7->Text  = OpenPortForm->Table2->FieldByName("NumberOfLanes")->AsString;
     }

     Table2->Filtered = false;
     Table4->Filtered = false;

     Opts.Clear();
     locvalues[0] = ListBox1->Items->Strings[ListBox1->ItemIndex];
     b = Table1->Locate("Portname", VarArrayOf(locvalues, 0), Opts);

     if(b == true)
        Edit6->Text  = OpenPortForm->Table2->FieldByName("MaxDistance")->AsString;
     else
     {
       locvalues[0] = ListBox1->Items->Strings[ListBox1->ItemIndex];
       OpenPortForm->Table2->Locate("Portname", VarArrayOf(locvalues, 0), Opts);
       OpenPortForm->Table2->Edit();
       OpenPortForm->Table2->FieldValues["MaxDistance"]= StrToInt(Edit6->Text);
       OpenPortForm->Table2->Post();

       for(int i1=1;i1<=Edit1->Text;i1++)
       {
         for(int i2=1;i2<=Edit1->Text;i2++)
         {
            AnsiString BlockStrS = "Block "; BlockStrS = BlockStrS + i1;
            AnsiString BlockStrD = "Block "; BlockStrD = BlockStrD + i2;
            if(i1 != i2)
            {
                Table1->Append();
                Table1->FieldValues["Portname"]= ListBox1->Items->Strings[ListBox1->ItemIndex];
                Table1->FieldValues["SourcePoint"]= BlockStrS;
                Table1->FieldValues["DestPoint"]= BlockStrD;
                Table1->FieldValues["Distance"] = 1+ random(StrToInt(Edit6->Text));
                Table1->Post();
            }
         }
         for(int i2=1;i2<=Edit2->Text;i2++)
         {
            AnsiString BlockStrS = "Block "; BlockStrS = BlockStrS + i1;
            AnsiString BlockStrD = "W/P "; BlockStrD = BlockStrD + i2;
            Table1->Append();
            Table1->FieldValues["Portname"]= ListBox1->Items->Strings[ListBox1->ItemIndex];
            Table1->FieldValues["SourcePoint"]= BlockStrS;
            Table1->FieldValues["DestPoint"]= BlockStrD;
            Table1->FieldValues["Distance"] = 1+ random(StrToInt(Edit6->Text));
            Table1->Post();
         }
       }
       for(int i1=1;i1<=Edit2->Text;i1++)
       {
         for(int i2=1;i2<=Edit2->Text;i2++)
         {
            AnsiString BlockStrS = "W/P "; BlockStrS = BlockStrS + i1;
            AnsiString BlockStrD = "W/P "; BlockStrD = BlockStrD + i2;
            if(i1 != i2)
            {
                Table1->Append();
                Table1->FieldValues["Portname"]= ListBox1->Items->Strings[ListBox1->ItemIndex];
                Table1->FieldValues["SourcePoint"]= BlockStrS;
                Table1->FieldValues["DestPoint"]= BlockStrD;
                Table1->FieldValues["Distance"] = 1+ random(StrToInt(Edit6->Text));
                Table1->Post();
            }
          }
         for(int i2=1;i2<=Edit1->Text;i2++)
         {
            AnsiString BlockStrS = "W/P "; BlockStrS = BlockStrS + i1;
            AnsiString BlockStrD = "Block "; BlockStrD = BlockStrD + i2;
            Table1->Append();
            Table1->FieldValues["Portname"]= ListBox1->Items->Strings[ListBox1->ItemIndex];
            Table1->FieldValues["SourcePoint"]= BlockStrS;
            Table1->FieldValues["DestPoint"]= BlockStrD;
            Table1->FieldValues["Distance"] = 1+ random(StrToInt(Edit6->Text));
            Table1->Post();
         }
       }
     }
     Opts.Clear();
     locvalues[0] = ListBox1->Items->Strings[ListBox1->ItemIndex];
     b = Table3->Locate("Portname", VarArrayOf(locvalues, 0), Opts);

     int NB = StrToInt(Edit1->Text);
     int NC = StrToInt(Edit2->Text);
     AnsiString BlockStrS;
     if(b == false)
       for(int i1=1;i1<=Edit3->Text;i1++)
       {
         Table3->Append();
         Table3->FieldValues["Portname"]= ListBox1->Items->Strings[ListBox1->ItemIndex];
         if(i1 <= NB)
         {
            BlockStrS = "Block "; BlockStrS = BlockStrS + i1;
         }
         else if (i1 <= NC + NB)
         {
            int i2 = i1 - NB ; BlockStrS = "W/P "; BlockStrS = BlockStrS + i2;
         } else
         {
            int i2 = i1 - NB - NC;BlockStrS = "M"; BlockStrS = BlockStrS + i2;
         }

         Table3->FieldValues["Location"]= BlockStrS;
         Table3->FieldValues["Junction"] = i1;
         Table3->Post();
       }
     Table2->Filter =   AnsiString("Portname = '") + ListBox1->Items->Strings[ListBox1->ItemIndex] + "'";
     Table2->Filtered = true;

     Table4->Filter =   AnsiString("Portname = '") + ListBox1->Items->Strings[ListBox1->ItemIndex] + "'";
     Table4->Filtered = true;

}
//---------------------------------------------------------------------------

void __fastcall TPortLayoutForm::FormActivate(TObject *Sender)
{
         PortLayoutForm->ListBox1->Clear();
         OpenPortForm->Table2->First();
         while (!OpenPortForm->Table2->Eof)
         {
            PortLayoutForm->ListBox1->Items->Add(OpenPortForm->Table2->FieldByName("Portname")->AsString);
            OpenPortForm->Table2->Next();
         }
}
//---------------------------------------------------------------------------

void __fastcall TPortLayoutForm::FormClose(TObject *Sender,
      TCloseAction &Action)
{
    DataSource1->Enabled = false;
    PortLayoutForm->Hide();
}
//---------------------------------------------------------------------------
void __fastcall Read_Route(String PortName)
{
     PortLayoutForm->Table4->Filter =   AnsiString("Portname = '") + PortName + "'";
     PortLayoutForm->Table4->Filtered = true;
     PortLayoutForm->Table4->First();
     if (! PortLayoutForm->Table4->Eof)
     {
         for(int z=1;z <= Port_Buff.NumberOfJunctions; z++)
         {
            Route_Buff [z-1].Junction     = PortLayoutForm->Table4->FieldByName("Junction")->Value;
            Route_Buff [z-1].Location     = PortLayoutForm->Table4->FieldByName("Location")->AsString;

            Route_Buff [z-1].NextJunc1    = PortLayoutForm->Table4->FieldByName("NextJunc1")->AsInteger;
            Route_Buff [z-1].NextLane1    = PortLayoutForm->Table4->FieldByName("NextLane1")->AsInteger;
            Route_Buff [z-1].DurationLane1= PortLayoutForm->Table4->FieldByName("DurationLane1")->AsInteger;

            Route_Buff2[Route_Buff[z-1].NextLane1-1].Duration = Route_Buff [z-1].DurationLane1;

            Route_Buff [z-1].NextJunc2    = PortLayoutForm->Table4->FieldByName("NextJunc2")->AsInteger;
            Route_Buff [z-1].NextLane2    = PortLayoutForm->Table4->FieldByName("NextLane2")->AsInteger;
            Route_Buff [z-1].DurationLane2= PortLayoutForm->Table4->FieldByName("DurationLane2")->AsInteger;

            Route_Buff2[Route_Buff[z-1].NextLane2-1].Duration = Route_Buff [z-1].DurationLane2;

            Route_Buff [z-1].NextJunc3    = PortLayoutForm->Table4->FieldByName("NextJunc3")->AsInteger;
            Route_Buff [z-1].NextLane3    = PortLayoutForm->Table4->FieldByName("NextLane3")->AsInteger;
            Route_Buff [z-1].DurationLane3= PortLayoutForm->Table4->FieldByName("DurationLane3")->AsInteger;

            Route_Buff2[Route_Buff[z-1].NextLane3-1].Duration = Route_Buff [z-1].DurationLane3;

            PortLayoutForm->Table4->Next();
            if(PortLayoutForm->Table4->Eof) break;
         }
     }
}
//---------------------------------------------------------------------------
int   __fastcall Update_Time_Between_Two_Points(AnsiString PortName,
                                            AnsiString SourcePoint,
                                            AnsiString DestPoint,
                                            int NewDistance)
{
        TLocateOptions Opts;
        AnsiString DistanceStr;
        Opts.Clear();
        Variant locvalues[3];
        locvalues[0] = PortName;
        locvalues[1] = SourcePoint;
        locvalues[2] = DestPoint;

        bool b = PortLayoutForm->Table1->Locate("Portname;Sourcepoint;DestPoint", VarArrayOf(locvalues, 2), Opts);
        if (b)
        {
           PortLayoutForm->Table1->Edit();
           PortLayoutForm->Table1->FieldByName("Distance")->Value = NewDistance;
           PortLayoutForm->Table1->Post();
           return(1);
        }
        return(0);
}
//---------------------------------------------------------------------------
int   __fastcall Get_Junction(AnsiString PortName,AnsiString StrPoint)
{
        TLocateOptions Opts;
        Opts.Clear();
        Variant locvalues[3];
        locvalues[0] = PortName;
        locvalues[1] = StrPoint;

        bool b = PortLayoutForm->Table4->Locate("Portname;Location", VarArrayOf(locvalues, 1), Opts);
        if (b)
        {
           int Junction = PortLayoutForm->Table4->FieldByName("Junction")->Value;
           return(Junction);
        }
        return(0);
}
//---------------------------------------------------------------------------
bool   __fastcall Detect_Middle(AnsiString PortName,int Junction)
{
        TLocateOptions Opts;
        Opts.Clear();
        Variant locvalues[3];
        locvalues[0] = PortName;
        locvalues[1] = Junction;

        bool b = PortLayoutForm->Table4->Locate("Portname;Junction", VarArrayOf(locvalues, 1), Opts);
        if (b)
        {
           AnsiString JunctionStr = PortLayoutForm->Table4->FieldByName("Location")->Value;
           if(JunctionStr.SubString(1,1) == "M")
              return(true);
        }
        return(false);
}
//------------------------------------------------------------------------------

int __fastcall Get_Lane_Route_Buff(int S2,int D2)
{

      if(Route_Buff [S2-1].NextJunc1 == D2)
         return(Route_Buff [S2-1].NextLane1 );

      if(Route_Buff [S2-1].NextJunc2 == D2)
         return(Route_Buff [S2-1].NextLane2 );

      if(Route_Buff [S2-1].NextJunc3 == D2)
         return(Route_Buff [S2-1].NextLane3 );

      return(0);
}

//------------------------------------------------------------------------------
void __fastcall Set_Path_For_Vehicle(int VN,int Junc_S,int Junc_D, int DMap,int NewAGV)
{
     int    D2;
     static int S2 ;

     if(NewAGV) S2 = Junc_S;

     if (DMap > 0)
     {
         Set_Path_For_Vehicle(VN,Junc_S,Junc_D,DIJ_Gpi [DMap], 0 );
         if (DMap >  DIJ_NumberOfNodes - Junc_S )
             D2 = DMap - (DIJ_NumberOfNodes - Junc_S );
         else
             D2 = DMap + Junc_S;
         Vehicle_Buff[VN-1].Lane [Vehicle_Buff[VN-1].Number_of_Lanes] = Get_Lane_Route_Buff(S2,D2);
         Vehicle_Buff[VN-1].Number_of_Lanes++;
         S2 = D2;
     }
}

//---------------------------------------------------------------------------
int __fastcall Shortest_Path_Dijkstra(AnsiString PortName,int Source,int Destination,int VN)
{
      int   src, dst, w ;
      int   u;
      GEntryPtr vptr;

      DIJ_NumberOfNodes = Port_Buff.NumberOfJunctions;
      DIJ_NumberOfEdges = Port_Buff.NumberOfLanes ;

      DIJ_Initialize();

      // Add Edges
      for (int n=1 ; n<=Port_Buff.NumberOfLanes; n++)
      {
          if(Route_Buff [n-1].NextJunc1 != 0 && Route_Buff [n-1].NextJunc1 != Source)
          {
            src = n;   dst = Route_Buff [n-1].NextJunc1;  w  = Route_Buff [n-1].DurationLane1;
            if( dst == Destination || src == Source ||
                ( Detect_Middle(PortName,src) && Detect_Middle(PortName,dst)) )
            {
                src = src % Source + (DIJ_NumberOfNodes - Source )*(src < Source);
                dst = dst % Source + (DIJ_NumberOfNodes - Source )*(dst < Source);
                DIJ_InsertEdge(src, dst, w);
            }
          }
          if(Route_Buff [n-1].NextJunc2 != 0 && Route_Buff [n-1].NextJunc1 != Source)
          {
            src = n;   dst = Route_Buff [n-1].NextJunc2;  w  = Route_Buff [n-1].DurationLane2;
            if( dst == Destination || src == Source ||
                ( Detect_Middle(PortName,src) && Detect_Middle(PortName,dst)) )
            {
                src = src % Source + (DIJ_NumberOfNodes - Source )*(src < Source);
                dst = dst % Source + (DIJ_NumberOfNodes - Source )*(dst < Source);
                DIJ_InsertEdge(src, dst, w);
            }
          }
          if(Route_Buff [n-1].NextJunc3 != 0 && Route_Buff [n-1].NextJunc1 != Source)
          {
            src = n;   dst = Route_Buff [n-1].NextJunc3;  w  = Route_Buff [n-1].DurationLane3;
            if( dst == Destination || src == Source ||
                ( Detect_Middle(PortName,src) && Detect_Middle(PortName,dst)) )
            {
                src = src % Source + (DIJ_NumberOfNodes - Source )*(src < Source);
                dst = dst % Source + (DIJ_NumberOfNodes - Source )*(dst < Source);
                DIJ_InsertEdge(src, dst, w);
            }
          }
      }

      DIJ_InitializeSingleSource();
      DIJ_InitializeQ();

      while (DIJ_Last > 0)
      {
          u = DIJ_DeleteMinQ();

          vptr = DIJ_Adj[u];
          while (vptr)
          {
               DIJ_Relax(u,vptr->node,vptr->weight);
               vptr=vptr->next;
          }
     }
     int DMap = Destination % Source + (DIJ_NumberOfNodes - Source )*(Destination < Source);
     int Distance = DIJ_Distance [DMap];

     if(VN)
     {
        Vehicle_Buff[VN-1].Number_of_Lanes = 0;
        Vehicle_Buff[VN-1].Lane_No         = 0;
        if(Distance < Infinity )
           Set_Path_For_Vehicle(VN,Source ,Destination , DMap, 1);
        else
           int No_Path = 1;
     }

     DIJ_Free();
     return (Distance);
}
//---------------------------------------------------------------------------

void __fastcall TPortLayoutForm::BitBtn1Click(TObject *Sender)
{
   // Calculate Shortest Distance by Dijkstra Algorithm
   if(ListBox1->ItemIndex < 0) return;
   AnsiString PortName = ListBox1->Items->Strings[ListBox1->ItemIndex];

   if(!Read_Port(PortName)) return;

   Read_Route(PortName);

   int s,d, NewDistance ;

   for(int i1=1;i1<=Edit1->Text;i1++)
   {
       AnsiString BlockStrS = "Block "; BlockStrS = BlockStrS + i1;
       s = Get_Junction(PortName,BlockStrS);
       for(int i2=1;i2<=Edit1->Text;i2++)
       {
           AnsiString BlockStrD = "Block "; BlockStrD = BlockStrD + i2;
           d = Get_Junction(PortName,BlockStrD);
           if(i1 != i2)
           {
             NewDistance = Shortest_Path_Dijkstra(PortName,s,d, 0 );
             Update_Time_Between_Two_Points(PortName,BlockStrS,BlockStrD,NewDistance);
           }
       }
       for(int i2=1;i2<=Edit2->Text;i2++)
       {
           AnsiString BlockStrD = "W/P "; BlockStrD = BlockStrD + i2;
           d = Get_Junction(PortName,BlockStrD);
           NewDistance = Shortest_Path_Dijkstra(PortName,s,d, 0);
           Update_Time_Between_Two_Points(PortName,BlockStrS,BlockStrD,NewDistance);
       }
   }
   for(int i1=1;i1<=Edit2->Text;i1++)
   {
       AnsiString BlockStrS = "W/P "; BlockStrS = BlockStrS + i1;
       s = Get_Junction(PortName,BlockStrS);
       for(int i2=1;i2<=Edit2->Text;i2++)
       {
          AnsiString BlockStrD = "W/P "; BlockStrD = BlockStrD + i2;
          if(i1 != i2)
          {
             d = Get_Junction(PortName,BlockStrD);
             NewDistance = Shortest_Path_Dijkstra(PortName,s,d, 0);
             Update_Time_Between_Two_Points(PortName,BlockStrS,BlockStrD,NewDistance);
          }
       }
       for(int i2=1;i2<=Edit1->Text;i2++)
       {
          AnsiString BlockStrD = "Block "; BlockStrD = BlockStrD + i2;
          d = Get_Junction(PortName,BlockStrD);
          NewDistance = Shortest_Path_Dijkstra(PortName,s,d, 0);
          Update_Time_Between_Two_Points(PortName,BlockStrS,BlockStrD,NewDistance);
       }
   }
   PortLayoutForm->Table2->Refresh();
}
//---------------------------------------------------------------------------


void __fastcall TPortLayoutForm::BitBtn2Click(TObject *Sender)
{
     char Fname1[100], Fname2[100], Buff[100];
     sprintf(&Fname1[0],"%s",BenchOptionForm->Edit5->Text);
     sprintf(&Fname2[0],"%s",BenchOptionForm->Edit6->Text);
     ofstream Fout1(Fname1);
     ofstream Fout2(Fname2);

     if(ListBox1->ItemIndex < 0) return;

     AnsiString PortName = ListBox1->Items->Strings[ListBox1->ItemIndex];
     sprintf(Buff,"%s",PortName);

     if(!Read_Port(PortName)) return;

     Read_Route(PortName);
     Fout1<<"\nRoute Information for "<<Buff;
     Fout1<<"\nJunc";
     Fout1<<"\tLoc "<<"\t";
     Fout1<<"\tNJ-1 ";
     Fout1<<"\tNL-1 ";
     Fout1<<"\tDL-1 ";
     Fout1<<"\tNJ-2 ";
     Fout1<<"\tNL-2 ";
     Fout1<<"\tDL-2 ";
     Fout1<<"\tNJ-3 ";
     Fout1<<"\tNL-3 ";
     Fout1<<"\tDL-3 ";
     for(int z=1;z <= Port_Buff.NumberOfJunctions; z++)
     {
        Fout1<<"\n"<<Route_Buff [z-1].Junction     ;
        sprintf(Buff,"%s",Route_Buff [z-1].Location);
        Fout1<<"\t"<<Buff<<"\t" ;
        Fout1<<"\t"<<Route_Buff [z-1].NextJunc1    ;
        Fout1<<"\t"<<Route_Buff [z-1].NextLane1    ;
        Fout1<<"\t"<<Route_Buff [z-1].DurationLane1;
        Fout1<<"\t"<<Route_Buff [z-1].NextJunc2    ;
        Fout1<<"\t"<<Route_Buff [z-1].NextLane2    ;
        Fout1<<"\t"<<Route_Buff [z-1].DurationLane2;
        Fout1<<"\t"<<Route_Buff [z-1].NextJunc3    ;
        Fout1<<"\t"<<Route_Buff [z-1].NextLane3    ;
        Fout1<<"\t"<<Route_Buff [z-1].DurationLane3;
    }
    Fout1.close ();

    TLocateOptions Opts;
    AnsiString DistanceStr;
    Opts.Clear();
    Variant locvalues[3];
    locvalues[0] = PortName;

    bool b;
    b = PortLayoutForm->Table1->Locate("Portname", VarArrayOf(locvalues, 0), Opts);
    sprintf(Buff,"%s",PortName);
    Fout2<<"\nDistance Information for "<<Buff;
    Fout2<<"\nSource";
    Fout2<<"\t\tDest";
    Fout2<<"\t\tTime";
    while(true)
    {
         if(Table1->Eof ) break;
         String Port2     = Table1->FieldByName("Portname")->AsString;
         String BlockStrS = Table1->FieldByName("SourcePoint")->AsString;
         String BlockStrD = Table1->FieldByName("DestPoint")->AsString  ;
         int    Distance  = Table1->FieldByName("Distance")->Value;
         if (Port2 != PortName)
             break;
         sprintf(Buff,"%s",BlockStrS); Fout2<<"\n"<<Buff;
         sprintf(Buff,"%s",BlockStrD); Fout2<<"\t\t"<<Buff;
                                       Fout2<<"\t\t"<<Distance ;

         PortLayoutForm->Table1->Next();
    }
    Fout2.close();
}
//---------------------------------------------------------------------------


