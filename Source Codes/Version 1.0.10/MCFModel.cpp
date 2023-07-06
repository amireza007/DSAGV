//------------------------------------------------------------------------------
#include<iostream>
#include<iomanip>

#include <vcl.h>
#pragma hdrstop

#include "MCFModel.h"
#include "OpenPort.h"
#include "PortLayout.h"
#include "PortContainer.h"
#include "PortAGV.h"
#include "stdio.h"
#include "string.h"
#include "Unit1.h"
#include "mcfdefs.h"

#define Maximum_Container_Jobs     4000
#define Maximum_Number_Vehicles    300
#define Maximum_Number_Cranes      100

//------------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

TMCFAlgorithmForm *MCFAlgorithmForm;

struct {
   AnsiString IDStr;
   AnsiString SourcePointStr;
   AnsiString DestPointStr  ;
   long int   ReadyTime     ;
   long int   QCraneTime    ;
}  Container_Buff[Maximum_Container_Jobs];

struct {
   int  Last_Completed_Temp;
   int  Number_Of_Jobs;
   int  Next_Generation_Time;
   int  Last_Completed_Time;
   int  Number_Of_Done_Jobs;
}  Crane_Buff[Maximum_Number_Cranes];

struct {
   int  Last_Completed_Temp;
   int  Number_Of_Jobs;
   int  Last_Completed_Time;
   int  Number_Of_Done_Jobs;
}  Vehicle_Buff[Maximum_Number_Vehicles];


extern "C"  int  MCF_Solve( int i,char *InFile, char *OutFile);

static long int TimeValueInSecond = 0;

long int GlobalInitialTimeQuayCrane;
bool StartOfDynamicSchedule;

//------------------------------------------------------------------------------
__fastcall TMCFAlgorithmForm::TMCFAlgorithmForm(TComponent* Owner)
        : TForm(Owner)
{
	ArmSec = Digitalformat->Width / 5;
	MinArm=(4 * ArmSec) / 5;
	HourArm=(2* ArmSec) / 5;
	ClockCenter.x= Digitalformat->Width  /2 ;
	ClockCenter.y= Digitalformat->Width  /2 ;
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
        Edit15->Text = IntToStr(MCF_MAX_ART_COST);
}
//------------------------------------------------------------------------------
void __fastcall TMCFAlgorithmForm::BitBtn1Click(TObject *Sender)
{
       Close();
}
//------------------------------------------------------------------------------
int   __fastcall Get_Time_Between_Two_Points(AnsiString PortName,
                                            AnsiString SourcePoint,
                                            AnsiString DestPoint)
{
        TLocateOptions Opts;
        AnsiString DistanceStr;
        Opts.Clear();
        Variant locvalues[3];
        locvalues[0] = PortName;
        locvalues[1] = SourcePoint;
        locvalues[2] = DestPoint;

        if(SourcePoint == DestPoint)  return(0);

        bool b = PortLayoutForm->Table1->Locate("Portname;Sourcepoint;DestPoint", VarArrayOf(locvalues, 2), Opts);
        if (b)
        {
           DistanceStr = PortLayoutForm->Table1->FieldByName("Distance")->AsString;
           return(StrToInt(DistanceStr));
        }
        return(0);
}
//------------------------------------------------------------------------------
AnsiString  __fastcall Get_StartLocation_And_ReadyTime_AGV(AnsiString PortName,
                                            int AGVNumber, int *ReadyTimeAGV)
{
        TLocateOptions Opts;
        AnsiString StartLocationStr="";

        *ReadyTimeAGV = 0;
        Opts.Clear();
        Variant locvalues[3];
        locvalues[0] = PortName;
        AnsiString AGVNameStrS = "AGV-";
        AGVNameStrS = AGVNameStrS + AGVNumber;
        locvalues[1] = AGVNameStrS;

        bool b = PortAGVForm->Table1->Locate("Portname;AGVName", VarArrayOf(locvalues, 1), Opts);
        if (b)
        {
           StartLocationStr = PortAGVForm->Table1->FieldByName("StartLocation")->AsString;
           *ReadyTimeAGV    = StrToInt(PortAGVForm->Table1->FieldByName("ReadyTime")->AsString);
        }
        return(StartLocationStr);
}
//------------------------------------------------------------------------------
void __fastcall Set_Table4_For_New_Schedule(AnsiString PortNameStr)
{
      TLocateOptions Opts;
      Set  <TLocateOption,0,1> flags;
      flags << loCaseInsensitive << loPartialKey;

      Variant locvalues[2];

      int Number_Of_AGVs = PortAGVForm->Table2->RecordCount;
      PortAGVForm->Table2->First() ;
      for(int m=1;m<=Number_Of_AGVs;m++)
      {
           int NumberOfJobs = 0;
           if( MCFAlgorithmForm->Table4->RecordCount > 0)
           {
                MCFAlgorithmForm->Table4->First();
                AnsiString ContainerID = MCFAlgorithmForm->Table4->FieldByName("ContainerID")->AsString;
                AnsiString PortName    = MCFAlgorithmForm->Table4->FieldValues["Portname"];
                AnsiString AGVNameStrS = MCFAlgorithmForm->Table4->FieldValues["AGVName"];
                int        ReadyTime   = MCFAlgorithmForm->Table4->FieldValues["ContainerReadyTime"];
                int        VehicleTime = MCFAlgorithmForm->Table4->FieldValues["VehicleTime"];
                int        ActualTime  = MCFAlgorithmForm->Table4->FieldValues["ActualTime"];
                int        QCraneTime = MCFAlgorithmForm->Table4->FieldValues["QuayCraneTime"];
                AnsiString SourcePoint = MCFAlgorithmForm->Table4->FieldByName("SourcePoint")->AsString;
                AnsiString DestPoint   = MCFAlgorithmForm->Table4->FieldByName("DestPoint")->AsString;

                AnsiString AGVLocation  = PortAGVForm->Table2->FieldValues["StartLocation"];
                AnsiString PreAGVLocation = PortAGVForm->Table2->FieldValues["PreviousLocation"];

                MCFAlgorithmForm->Table4->Active = false;
                MCFAlgorithmForm->Table4->EmptyTable();
                MCFAlgorithmForm->Table4->Active = true;

                if(DestPoint.SubString(1,3) == "W/P" &&
                   SourcePoint == PreAGVLocation &&  AGVLocation == DestPoint)
                {
                   MCFAlgorithmForm->Table4->AutoCalcFields = false;
                   MCFAlgorithmForm->Table4->Append ();
                   MCFAlgorithmForm->Table4->FieldByName("ContainerID")->AsString = ContainerID;
                   MCFAlgorithmForm->Table4->FieldValues["Portname"]              = PortName   ;
                   MCFAlgorithmForm->Table4->FieldValues["AGVName"]               = AGVNameStrS;
                   MCFAlgorithmForm->Table4->FieldValues["ContainerReadyTime"]    = ReadyTime  ;
                   MCFAlgorithmForm->Table4->FieldValues["VehicleTime"]           = VehicleTime;
                   MCFAlgorithmForm->Table4->FieldValues["ActualTime"]            = ActualTime ;
                   MCFAlgorithmForm->Table4->FieldValues["SourcePoint"]           = SourcePoint;
                   MCFAlgorithmForm->Table4->FieldValues["DestPoint"]             = DestPoint ;
                   MCFAlgorithmForm->Table4->FieldValues["QuayCraneTime"]         = QCraneTime;
                   MCFAlgorithmForm->Table4->Post ();
                   MCFAlgorithmForm->Table4->AutoCalcFields = true;
                   NumberOfJobs = 1;

                   locvalues[0] = PortName;
                   locvalues[1] = ContainerID;
                   Opts << loCaseInsensitive ;

                   bool b = PortContainerForm->Table1->Locate("Portname;ContainerID", VarArrayOf(locvalues, 1) , Opts);
                   if (b) PortContainerForm->Table1->Delete();

                }

           }
           MCFAlgorithmForm->Table4->Refresh();
           MCFAlgorithmForm->DBChart1->Refresh() ;
           MCFAlgorithmForm->DBGrid3->Refresh ();

           PortAGVForm->Table2->Edit() ;
           PortAGVForm->Table2->FieldValues["NumberOfContainers"] =  NumberOfJobs;
           PortAGVForm->Table2->Post() ;

           PortAGVForm->Table2->Next() ;
      }

      PortAGVForm->Table2->Refresh();
      MCFAlgorithmForm->DBGrid4->Refresh ();
}
//------------------------------------------------------------------------------
void __fastcall Set_Empty_Table4_For_Specific_Port(AnsiString PortNameStr)
{
       TLocateOptions Opts;
       Set  <TLocateOption,0,1> flags;
       flags << loCaseInsensitive << loPartialKey;

       Variant locvalues[2];

       locvalues[0] = PortNameStr;
       bool b;
       // Deleting AGVs from AGV Files
       flags << loCaseInsensitive << loPartialKey;

       b = MCFAlgorithmForm->Table5->Locate("Portname",PortNameStr, flags);

       while(b)
       {
         MCFAlgorithmForm->Table5->Delete();
         bool b = MCFAlgorithmForm->Table5->Locate("Portname", PortNameStr, flags);
         if(MCFAlgorithmForm->Table5->FieldByName("Portname")->AsString != PortNameStr)
            break ;
         if (MCFAlgorithmForm->Table5->Eof ) break;
       }
       MCFAlgorithmForm->Table4->Refresh() ;
       MCFAlgorithmForm->DBChart1->Refresh() ;
       MCFAlgorithmForm->DBGrid3->Refresh ();

       Opts.Clear();
       for(int i=1;i<=PortAGVForm->Table2->RecordCount;i++)
       {
            locvalues[0] = PortNameStr;
            AnsiString AGVNameStrS = "AGV-"; AGVNameStrS = AGVNameStrS + StrToInt(i);
            locvalues[1] = AGVNameStrS;

            bool b = PortAGVForm->Table1->Locate("Portname;AGVName", VarArrayOf(locvalues, 1), Opts);
            if (b )
            {
                PortAGVForm->Table1->Edit();
                PortAGVForm->Table1->FieldValues["NumberOfContainers"] = 0;
                PortAGVForm->Table1->Post();
            }
        }
        PortAGVForm->Table2->Refresh();
        MCFAlgorithmForm->DBGrid4->Refresh ();

}
//------------------------------------------------------------------------------
void __fastcall  Reset_Number_Of_Jobs_Before_Solution(int Number_Of_Crane,int Number_of_Vehicles)
{
      for(int c=1;c<=Number_Of_Crane;c++)
      {
           Crane_Buff[c-1].Number_Of_Jobs = 0;
           Crane_Buff[c-1].Last_Completed_Temp = Crane_Buff[c-1].Last_Completed_Time ;
      }

      for(int v=1;v<=Number_of_Vehicles;v++)
      {
           Vehicle_Buff[v-1].Number_Of_Jobs = 0;
           Vehicle_Buff[v-1].Last_Completed_Temp = Vehicle_Buff[v-1].Last_Completed_Time ;
      }

}

//------------------------------------------------------------------------------
void __fastcall  Read_Solution(char *FileName   ,int Num_Arcs   , int M_AGVs,
                               int   N_Nodes    ,TObject *Sender, int staticflag,
                               int  *TimeAGVToC ,int *TimeCToC  , int Number_Of_Container_Jobs,
                               int  *CostAGVToC ,int *CostCToC)
{
     TLocateOptions Opts;
     Variant locvalues[3];

     int   Time_Window_Vehicle;
     FILE *fp2 = fopen(FileName,"r+b");
     AnsiString buff2, b3 , b4, PortName;
     int   ActualTime,AGVConRec;
     int   VehicleTime;
     char  buff[500];
     int  *Route_Table , *AGV_Con_Table;

     int   Row=0, Num_Iteration, CPU_Time;
   //  float Objective_Function;
     unsigned long  Objective_Function;

     int   Quay_Crane_Time_Window,  Crane_Number , Number_Of_Cranes;

     int Number_Of_Jobs ;
     int Number_Of_Remaining_Jobs;

     long Max_Cost;
     FILE *maxcost = fopen("Tempcost.max","r");
     fgets(buff,500,maxcost);
     while(buff[0] == ' ') strcpy(buff,&buff[1]);
     MCFAlgorithmForm->Edit15->Text = buff;
     fclose(maxcost);

     Route_Table   = (int *) malloc(sizeof(int)*(Num_Arcs+1));

     AGV_Con_Table = (int *) malloc(sizeof(int)*Number_Of_Container_Jobs);

     if(staticflag)
     {
         Number_Of_Jobs = StrToInt(MCFAlgorithmForm->Edit1->Text);
         Number_Of_Remaining_Jobs = Number_Of_Jobs;
         Quay_Crane_Time_Window = StrToInt(MCFAlgorithmForm->Edit26->Text);
         Number_Of_Cranes       = StrToInt(MCFAlgorithmForm->Edit6->Text);
     } else
     {
         Quay_Crane_Time_Window = StrToInt(MCFAlgorithmForm->Edit18->Text);
         Number_Of_Cranes       = StrToInt(MCFAlgorithmForm->Edit12->Text);
     }

     while(1)
     {
          if(feof(fp2)) break;
          fgets(buff,500,fp2); buff2 =  buff; Row++;
          if(Row >= 4)
          {
             switch(Row)
             {
                 case 4:b3= buff2.SubString(buff2.AnsiPos("need")+4,buff2.AnsiPos("iteration")-buff2.AnsiPos("need")-5);
                        Num_Iteration = StrToInt(b3);
                        b3= buff2.SubString(buff2.AnsiPos("in")+2,buff2.AnsiPos("second")-buff2.AnsiPos("in")-3);
                        CPU_Time = StrToInt(b3);
                        break;
                 case 5:// Objective_Function = StrToFloat(buff2.SubString(2,buff2.Length()-3));
                        Objective_Function = StrToInt(buff2.SubString(2,buff2.Length()-3));
                        break;
                 default:if(buff2[1] == 'c') break;
                        b3 = buff2.SubString(3,buff2.Length()-4);
                        int i1 = b3.Pos(" ");
                        b4 = b3.SubString(i1+1,b3.Length()-i1);
                        int i2 = b4.Pos(" ");

                        int i  = StrToInt(b3.SubString(1,i1-1));
                        int j  = StrToInt(b4.SubString(1,i2-1));
                        *(Route_Table+i) = j;
              }
           }
     }
     fclose(fp2);

     MCFAlgorithmForm->Edit13->Text = IntToStr(CPU_Time);
     MCFAlgorithmForm->Edit16->Text = IntToStr(Num_Iteration);
     MCFAlgorithmForm->Edit17->Text = IntToStr(Objective_Function);

     if(staticflag)
     {
       locvalues[0] = MCFAlgorithmForm->ListBox1->Items->Strings[MCFAlgorithmForm->ListBox1->ItemIndex];
       Time_Window_Vehicle = StrToInt(MCFAlgorithmForm->Edit5->Text);
     }
     else
     {
       locvalues[0] = MCFAlgorithmForm->ListBox3->Items->Strings[MCFAlgorithmForm->ListBox3->ItemIndex];
       Time_Window_Vehicle = StrToInt(MCFAlgorithmForm->Edit19->Text);
     }

     if(!Route_Table || !AGV_Con_Table )
     {
         Application->MessageBox("Not Enough Memory !! ", "Error", MB_OK);
         return;
     }

     PortName = locvalues[0];

     PortAGVForm->Table2->Filter =   AnsiString("Portname = '") + PortName + "'";
     PortAGVForm->Table2->Filtered = true;

     Set  <TLocateOption,0,1> flags;
     flags << loCaseInsensitive << loPartialKey;

     PortContainerForm->Table2->Filter =   AnsiString("Portname = '") + PortName + "'";
     PortContainerForm->Table2->Filtered = true;

     Opts.Clear();
     int AGVn = 1;
     AnsiString AGVNameStrS = "AGV-" + IntToStr(AGVn);;
     locvalues[1] = AGVNameStrS;

     bool b ;
     if(staticflag)
     {
        PortAGVForm->Table2->First() ;
        for(int i=1;i<=M_AGVs;i++)
        {
               MCFAlgorithmForm->Table4->Active = false;
               MCFAlgorithmForm->Table4->EmptyTable() ;
               MCFAlgorithmForm->Table4->Active = true;

               PortAGVForm->Table2->Next() ;
        }
     }

     PortAGVForm->Table2->First() ;
     for(int i=1;i<=M_AGVs;i++)
     {
        int next = *(Route_Table+i);

        AGVConRec = 0;
        int n=0;

        while(next != N_Nodes)
        {
          int c_node = (next - M_AGVs)/2 + 1;

          next = *(Route_Table+next);
          if( next == N_Nodes) break;
          if( (next - M_AGVs) % 2 == 0 )
              next = *(Route_Table+next);

              AnsiString AGVNameStrS = "AGV-"; AGVNameStrS = AGVNameStrS + StrToInt(i);

              locvalues[0] = PortName;
              locvalues[1] = Container_Buff[c_node-1].IDStr;
              locvalues[2] = AGVNameStrS;
              Opts << loCaseInsensitive ;
              b = MCFAlgorithmForm->Table5->Locate("Portname;AGVName;ContainerID", VarArrayOf(locvalues, 2) , Opts);

              MCFAlgorithmForm->Table5->Append();

              MCFAlgorithmForm->Table5->FieldValues["Portname"]= PortName;
              MCFAlgorithmForm->Table5->FieldValues["AGVName"]= AGVNameStrS;
              MCFAlgorithmForm->Table5->FieldValues["AGVNumber"]= i;
              MCFAlgorithmForm->Table5->FieldValues["ContainerID"]       = Container_Buff[c_node-1].IDStr;

              locvalues[0] = PortName;
              locvalues[1] = Container_Buff[c_node-1].IDStr;
              Opts << loCaseInsensitive ;
              b = PortContainerForm->Table1->Locate("Portname;ContainerID", VarArrayOf(locvalues, 1) , Opts);
              if (b)
              {
                 AnsiString SourcePointContainerStr = PortContainerForm->Table1->FieldByName("SourcePoint")->AsString;
                 AnsiString DestPointContainerStr   = PortContainerForm->Table1->FieldByName("DestPoint")->AsString;
                 if (SourcePointContainerStr.SubString(1,3)== "W/P")
                     Crane_Number = StrToInt(SourcePointContainerStr.SubString(5,SourcePointContainerStr.Length()-4 ));
                 if (DestPointContainerStr.SubString(1,3)== "W/P")
                     Crane_Number = StrToInt(DestPointContainerStr.SubString(5,DestPointContainerStr.Length()-4 ));
                 PortContainerForm->Table1->Edit();
                 PortContainerForm->Table1->FieldValues["ReadyTime"]= Container_Buff[c_node-1].ReadyTime;

                 PortContainerForm->Table1->Post();
                 PortContainerForm->Table2->Refresh() ;
              }

              MCFAlgorithmForm->Table5->FieldValues["ContainerReadyTime"]= Container_Buff[c_node-1].ReadyTime;
              MCFAlgorithmForm->Table5->FieldValues["SourcePoint"]       = Container_Buff[c_node-1].SourcePointStr ;;
              MCFAlgorithmForm->Table5->FieldValues["DestPoint"]         = Container_Buff[c_node-1].DestPointStr;
              //---------------------------------------------------
              VehicleTime = Time_Window_Vehicle * Vehicle_Buff[i-1].Number_Of_Jobs
                          + Vehicle_Buff[i-1].Last_Completed_Temp ;

              Vehicle_Buff[i-1].Number_Of_Jobs++;

              *(AGV_Con_Table + AGVConRec) = c_node;
               AGVConRec ++;

              if( AGVConRec <= 1)
              {
                 int First_Job_For_Vehicle = (*(Route_Table + i)-M_AGVs) /2 + 1;
                 VehicleTime  += *(TimeAGVToC + (i-1)*Number_Of_Container_Jobs  + First_Job_For_Vehicle - 1 ); // + c_node - 1) ;
              }else
              {
                int n1 = *(AGV_Con_Table + AGVConRec - 2);
                int n2 = *(AGV_Con_Table + AGVConRec - 1);
                VehicleTime += * (TimeCToC + (n1-1)*Number_Of_Container_Jobs + n2-1);
              }

              MCFAlgorithmForm->Table5->FieldValues["VehicleTime"]= VehicleTime;

              int QuayCraneTime, ActualTime;

              if(Crane_Buff[Crane_Number-1].Number_Of_Jobs > 0 )
                  QuayCraneTime = Quay_Crane_Time_Window
                                + Crane_Buff[Crane_Number-1].Last_Completed_Temp;
              else
                  QuayCraneTime = Crane_Buff[Crane_Number-1].Last_Completed_Temp;

              Crane_Buff[Crane_Number-1].Number_Of_Jobs ++;

              MCFAlgorithmForm->Table5->FieldValues["QuayCraneTime"]= QuayCraneTime;
              if(QuayCraneTime > VehicleTime)
                 ActualTime = QuayCraneTime;
              else
                 ActualTime = VehicleTime;

              Crane_Buff[Crane_Number-1].Last_Completed_Temp = ActualTime;
              Crane_Buff[Crane_Number-1].Next_Generation_Time= ActualTime;
              Vehicle_Buff[i-1].Last_Completed_Temp = ActualTime;

              MCFAlgorithmForm->Table5->FieldValues["ActualTime"]= ActualTime;

              MCFAlgorithmForm->Table5->Post();

              if(n++ > Num_Arcs ) break;
        }
        PortAGVForm->Table2->Edit();
        PortAGVForm->Table2->FieldValues["NumberOfContainers"] = AGVConRec;
        int Cost;
        if(AGVConRec > 0 )
        {
              Cost  = *(CostAGVToC + (i-1)*Number_Of_Container_Jobs + *(AGV_Con_Table + 0 )-1 ) ;
              for(int k=0;k<AGVConRec-1;k++)
                  Cost += * (CostCToC + (*(AGV_Con_Table + k ) - 1)*Number_Of_Container_Jobs + *(AGV_Con_Table + k + 1 )-1);
        } else
              Cost = 0;

        if(staticflag)
        {
             PortAGVForm->Table2->FieldValues["CostAGV"] = Cost;
             Number_Of_Remaining_Jobs -= AGVConRec;
             MCFAlgorithmForm->Edit21->Text = Number_Of_Remaining_Jobs;
        }
        PortAGVForm->Table2->Post();
        PortAGVForm->Table2->Next() ;

     }
     MCFAlgorithmForm->Table4->Refresh() ;
     MCFAlgorithmForm->DBChart1->Refresh() ;
     MCFAlgorithmForm->DBGrid3->Refresh ();

     PortAGVForm->Table2->Refresh() ;
     MCFAlgorithmForm->DBGrid4->Refresh() ;

     free(Route_Table);
     free(AGV_Con_Table);
}
//------------------------------------------------------------------------------
void __fastcall Append_Supply_Nodes(int Number_Of_AGVs,FILE *fp)
{

     AnsiString Str1;
     fprintf(fp,"c Create supply nodes  \n");
     MCFAlgorithmForm->Label25->Caption = "Create Supply Nodes";
     MCFAlgorithmForm->GroupBox15->Refresh() ;
     for (int i1=1;i1<=Number_Of_AGVs;i1++)
     {
         //                Node    Supply
         Str1.printf("n    %5d     1",i1);
         fprintf(fp,"%s  \n", Str1.c_str());
         MCFAlgorithmForm->ProgressBar1->Position = 100*i1/Number_Of_AGVs;
     }
}
//------------------------------------------------------------------------------
void __fastcall Append_Demand_Node(int Number_Of_AGVs, int Number_Of_Nodes,FILE *fp)
{
     AnsiString Str1 ;
     fprintf(fp, "c Create demand node \n");
     //                Sink    Demand
     Str1.printf("n    %5d     %5d   ",Number_Of_Nodes,-1*Number_Of_AGVs);
     fprintf(fp,"%s  \n", Str1.c_str());
}
//------------------------------------------------------------------------------
void __fastcall  Append_Arcs_From_AGV_Nodes_To_Sink_Node(int Number_Of_AGVs,
                     int *Number_Of_ARCs,
                     AnsiString SinkNodeStr,FILE *fp)
{
     AnsiString Str1;
     fprintf(fp, "c Create arcs from AGV Nodes to the Sink node \n");
     MCFAlgorithmForm->Label25->Caption = "Arcs from AGV to Sink";
     MCFAlgorithmForm->Refresh() ;
     for (int i1=1;i1<=Number_Of_AGVs;i1++)
     {
            // define     AGV m     Sink  L    U     C
         Str1.printf("a    %5d     %5d    0    1     0", i1,StrToInt(SinkNodeStr));
         MCFAlgorithmForm->ProgressBar1->Position  = 100*i1/Number_Of_AGVs;
         fprintf(fp,"%s  \n", Str1.c_str());
         (*Number_Of_ARCs)++;
     }
}
//------------------------------------------------------------------------------
int __fastcall Penalty1(AnsiString PortNameStr)
{
     TLocateOptions Opts;

     Opts.Clear();
     Variant locvalues[2];
     locvalues[0] = PortNameStr;

     bool b = OpenPortForm->Table2->Locate("Portname", VarArrayOf(locvalues, 0), Opts);
     if (b)
     {
        int Distance = StrToInt(OpenPortForm->Table2->FieldByName("MaxDistance")->AsString);
        return(Distance*2);
     }
}
//------------------------------------------------------------------------------
int __fastcall Penalty2(AnsiString PortNameStr)
{
     TLocateOptions Opts;

     Opts.Clear();
     Variant locvalues[2];
     locvalues[0] = PortNameStr;

     bool b = OpenPortForm->Table2->Locate("Portname", VarArrayOf(locvalues, 0), Opts);
     if (b)
     {
        int Distance = StrToInt(OpenPortForm->Table2->FieldByName("MaxDistance")->AsString);
        return(Distance*5);
     }
}
//------------------------------------------------------------------------------
void __fastcall Append_Arcs_From_AGVs_Nodes_To_Container_Nodes(int Number_Of_AGVs
                        ,int *Number_Of_ARCs      , AnsiString PortNameStr
                        ,int *TimeAGVToC
                        ,int  Number_Container_Job, int *CostAGVToC  , FILE *fp, int Static)
{
     int ReadyTimeAGV;
     int RecTable2, Distance ;
     int W1, W2;
     AnsiString Str1;

     float Penalty = Penalty1(PortNameStr);

     if(Static)
     {
        W1 = StrToFloat(MCFAlgorithmForm->Edit22->Text);
        W2 = StrToFloat(MCFAlgorithmForm->Edit2->Text);
     } else
     {
        W1 = StrToFloat(MCFAlgorithmForm->Edit14->Text);
        W2 = StrToFloat(MCFAlgorithmForm->Edit10->Text);
     }

     fprintf(fp, "c Create arcs from any AGV node to any container node, if it is compatible\n");
     MCFAlgorithmForm->Label25->Caption = "Arcs from AGV to Jobs";
     MCFAlgorithmForm->Refresh() ;
     for (int i1=1;i1<=Number_Of_AGVs;i1++)
     {
         MCFAlgorithmForm->ProgressBar1->Position = 100*i1/Number_Of_AGVs;
         int i2=Number_Of_AGVs+1;
         AnsiString  StartLocationAGVStr = Get_StartLocation_And_ReadyTime_AGV(PortNameStr,
                                            i1, &ReadyTimeAGV);
         RecTable2=1;
         int NoRecTable2 = Number_Container_Job; // PortContainerForm->Table2->RecordCount;
         for(int r=1;r<=NoRecTable2;r++)
         {
             AnsiString ContainerIDStr          = Container_Buff[r-1].IDStr ;
             AnsiString SourcePointContainerStr = Container_Buff[r-1].SourcePointStr ;
             AnsiString DestPointContainerStr   = Container_Buff[r-1].DestPointStr;
             int ReadyTimeContainer             = Container_Buff[r-1].ReadyTime ;
             AnsiString PortNameIDStr = PortNameStr;

             if(StartLocationAGVStr.SubString(1,3)== "W/P" &&
                SourcePointContainerStr.SubString(1,3) == "W/P")
                Distance = Get_Time_Between_Two_Points(PortNameIDStr,StartLocationAGVStr,SourcePointContainerStr);

             else if(StartLocationAGVStr.SubString(1,3)== "W/P" &&
                SourcePointContainerStr.SubString(1,3) == "Blo")
                Distance = Get_Time_Between_Two_Points(PortNameIDStr,StartLocationAGVStr,SourcePointContainerStr)
                         + Get_Time_Between_Two_Points(PortNameIDStr,SourcePointContainerStr,DestPointContainerStr);

             else if(StartLocationAGVStr.SubString(1,3)== "Blo" &&
                SourcePointContainerStr.SubString(1,3) == "W/P")
                Distance = Get_Time_Between_Two_Points(PortNameIDStr,StartLocationAGVStr,SourcePointContainerStr);

             else if(StartLocationAGVStr.SubString(1,3)== "Blo" &&
                SourcePointContainerStr.SubString(1,3) == "Blo")
                Distance = Get_Time_Between_Two_Points(PortNameIDStr,StartLocationAGVStr,SourcePointContainerStr)
                         + Get_Time_Between_Two_Points(PortNameIDStr,SourcePointContainerStr,DestPointContainerStr);

             *(TimeAGVToC  + (i1-1)*Number_Container_Job + r-1) = ReadyTimeAGV + Distance;
             int DT1, DT2;
             // make a higher priority for for late jobs
            //  DT1 = ReadyTimeContainer - (Distance +  ReadyTimeAGV + TimeValueInSecond)
            //      - Late *(TimeValueInSecond- ReadyTimeContainer)
            //  if (DT1 < 0) DT1 = 0;

             DT1 = ReadyTimeContainer - (Distance +  ReadyTimeAGV + TimeValueInSecond);
             if( DT1 >= 0 )
             {
                DT1 = W1 * (ReadyTimeContainer - (Distance +  ReadyTimeAGV + TimeValueInSecond))
                    + W2 *  Distance;
                //  - Late * (TimeValueInSecond - ReadyTimeContainer)
                //  if (DT1 < 0 ) DT1 = 0;
                // define       AGV m     c      L   U     C
                Str1.printf("a    % 5.0d     % 5.0d     0    1    %10.1f", i1,i2,(float ) DT1);
                (*Number_Of_ARCs)++;
                fprintf(fp,"%s  \n", Str1.c_str());
                *(CostAGVToC+(i1-1)*Number_Container_Job+ RecTable2-1) =  DT1;
             }
             else{
                DT2 = W1 * Penalty *(ReadyTimeAGV + Distance + TimeValueInSecond - ReadyTimeContainer)
                    + W2 * Penalty * Distance;
                //  - Late * (TimeValueInSecond - ReadyTimeContainer)
                //  if (DT2 < 0 ) DT2 = 0;
                 // define arc AGV m     c     L    U    C
                Str1.printf("a    % 5.0d     % 5.0d     0    1    %10.1f", i1,i2,(float ) DT2);
                (*Number_Of_ARCs)++;
                 fprintf(fp,"%s  \n", Str1.c_str());
                *(CostAGVToC+(i1-1)*Number_Container_Job+ RecTable2-1) = DT2;
             }
             RecTable2++;
             i2 = i2 + 2;
         }
     }
}
// -----------------------------------------------------------------------------
void __fastcall Append_Arcs_From_CNode_To_CPNodes(int Number_Of_AGVs,
                                                  int Number_Of_Nodes,
                                                  int *Number_Of_ARCs,FILE *fp)
{
     AnsiString Str1;
     fprintf(fp,"c Create arcs from any c node to c' container node\n");
     MCFAlgorithmForm->Label25->Caption = "Arcs from c to c'";
     MCFAlgorithmForm->Refresh() ;
     for (int i1=Number_Of_AGVs+1;i1<=Number_Of_Nodes-1;i1=i1+2)
     {
         // define arc from any c to c'   L    U   C
         Str1.printf("a    %5d     %5d    1    1   0", i1,i1+1);
         fprintf(fp,"%s  \n", Str1.c_str());
         (*Number_Of_ARCs)++;
         MCFAlgorithmForm->ProgressBar1->Position = 100*(i1-Number_Of_AGVs-1)/(Number_Of_Nodes-1 - Number_Of_AGVs -1);
     }
}
//------------------------------------------------------------------------------
void __fastcall Append_Arcs_From_CPNodes_To_Sink_Node(int Number_Of_AGVs
                                ,int Number_Of_Nodes, AnsiString SinkNodeStr
                                ,int *Number_Of_ARCs,FILE *fp)
{
     AnsiString Str1;

     fprintf(fp,"c Create arcs from c' node to sink node \n");
     MCFAlgorithmForm->Label25->Caption = "Arcs from c' to Sink";
     MCFAlgorithmForm->Refresh() ;
     for (int i1=Number_Of_AGVs+2;i1<=Number_Of_Nodes-1;i1=i1+2)
     {
         //                 C'     Sink   L   U   C
         Str1.printf("a    %5d     %5d    0   1   0", i1,StrToInt(SinkNodeStr));
         fprintf(fp,"%s  \n", Str1.c_str());
         (*Number_Of_ARCs)++;
         MCFAlgorithmForm->ProgressBar1->Position = 100*(i1-Number_Of_AGVs-2)/(Number_Of_Nodes-1 - Number_Of_AGVs -2);
     }
}
//------------------------------------------------------------------------------
void __fastcall Append_Arcs_From_CPNodes_To_Any_Other_CNodes(int Number_Of_AGVs
                        ,int Number_Of_Container_Jobs, int *Number_Of_ARCs
                        ,int *TimeCToC ,int *CostCToC,AnsiString PortNameIDStr,FILE *fp, int Static)
{
     int Tij ;
     int W1, W2;

     if (Number_Of_Container_Jobs <= 1) return;

     float Penalty = Penalty2(PortNameIDStr);

     if(Static)
     {
        W1 = StrToFloat(MCFAlgorithmForm->Edit22->Text);
        W2 = StrToFloat(MCFAlgorithmForm->Edit2->Text);
     } else
     {
        W1 = StrToFloat(MCFAlgorithmForm->Edit14->Text);
        W2 = StrToFloat(MCFAlgorithmForm->Edit10->Text);
     }

     AnsiString Str1;

     int RecTable2=1;


     fprintf(fp, "c Create arcs from any c' node to any other c node\n");

     MCFAlgorithmForm->GroupBox15->Refresh() ;
     int NoRecTable3 = Number_Of_Container_Jobs; // PortContainerForm->Table2->RecordCount;

     MCFAlgorithmForm->Label25->Caption = "Arcs from c' to other c";

     for (int i1=Number_Of_AGVs+2;i1<Number_Of_AGVs+2+ Number_Of_Container_Jobs*2; i1=i1+2)
     {                                            //  is deleted from last line
          MCFAlgorithmForm->ProgressBar1->Position = 100*RecTable2/NoRecTable3;
          MCFAlgorithmForm->Refresh() ;
          int i2=Number_Of_AGVs+1;

          AnsiString ContainerIDStr          = Container_Buff[RecTable2-1].IDStr ;

          for(int RecTable3=1;RecTable3<=NoRecTable3;RecTable3++)
          {

               AnsiString ContainerIDStr2          = Container_Buff[RecTable3-1].IDStr ;

               if(ContainerIDStr != ContainerIDStr2)
               {
                  AnsiString SourcePointContainerStr = Container_Buff[RecTable2-1].SourcePointStr ;
                  AnsiString DestPointContainerStr   = Container_Buff[RecTable2-1].DestPointStr;
                  int ReadyTimeContainer             = Container_Buff[RecTable2-1].ReadyTime ;
                  int QCTime                         = Container_Buff[RecTable2-1].QCraneTime ;

                  AnsiString SourcePointContainerStr2= Container_Buff[RecTable3-1].SourcePointStr ;
                  AnsiString DestPointContainerStr2  = Container_Buff[RecTable3-1].DestPointStr;
                  int ReadyTimeContainer2            = Container_Buff[RecTable3-1].ReadyTime ;
                  int QCTime2                        = Container_Buff[RecTable2-1].QCraneTime ;

                  if(SourcePointContainerStr.SubString(1,3)== "W/P"
                  && DestPointContainerStr2.SubString(1,3) == "W/P")
                     Tij = Get_Time_Between_Two_Points(PortNameIDStr,SourcePointContainerStr ,DestPointContainerStr)
                         + Get_Time_Between_Two_Points(PortNameIDStr,DestPointContainerStr   ,SourcePointContainerStr2)
                         + Get_Time_Between_Two_Points(PortNameIDStr,SourcePointContainerStr2,DestPointContainerStr2);

                  else if(SourcePointContainerStr.SubString(1,3)== "W/P"
                  && SourcePointContainerStr2.SubString(1,3) == "W/P")
                     Tij = Get_Time_Between_Two_Points(PortNameIDStr,SourcePointContainerStr ,DestPointContainerStr)
                         + Get_Time_Between_Two_Points(PortNameIDStr,DestPointContainerStr   ,SourcePointContainerStr2);

                  else if(DestPointContainerStr.SubString(1,3)== "W/P"
                  && DestPointContainerStr2.SubString(1,3) == "W/P")
                     Tij = Get_Time_Between_Two_Points(PortNameIDStr,DestPointContainerStr ,SourcePointContainerStr2)
                         + Get_Time_Between_Two_Points(PortNameIDStr,SourcePointContainerStr2,DestPointContainerStr2);

                  else if(DestPointContainerStr.SubString(1,3)== "W/P"
                  && SourcePointContainerStr2.SubString(1,3) == "W/P")
                     Tij = Get_Time_Between_Two_Points(PortNameIDStr,DestPointContainerStr ,SourcePointContainerStr2);

                   *(TimeCToC + (RecTable2-1)*Number_Of_Container_Jobs + RecTable3-1) = Tij;

                  int DT1, DT2;
                  // Make a higher priority for for the late jobs
                  // DT1 = ReadyTimeContainer - (Distance +  ReadyTimeAGV + TimeValueInSecond)
                  //      - Late *(TimeValueInSecond- ReadyTimeContainer)
                 //  If (DT1 < 0) DT1 = 0;

                //  DT1 = QCTime2 - (QCTime + Tij );
                  DT1 = ReadyTimeContainer2 - (ReadyTimeContainer + Tij );
                  if(DT1 >= 0 )
                  {
                     // DT2 = ReadyTimeContainer2 - (ReadyTimeContainer + Tij)
                     //     - Late *(TimeValueInSecond - ReadyTimeContainer2)
                     // if (DT2 < 0) DT2 = 0;
                     (*Number_Of_ARCs) = (*Number_Of_ARCs) + 1;
                     DT1 = W1 * (ReadyTimeContainer2 - (ReadyTimeContainer + Tij ))
                         + W2 * Tij;
                     //                 Agv m   C     L   U   C
                     Str1.printf("a    %5d     %5d    0   1   %10.1f", i1,i2,(float ) DT1 );
                     fprintf(fp,"%s  \n", Str1.c_str());
                     *(CostCToC + (RecTable2-1)*Number_Of_Container_Jobs + RecTable3-1) = DT1;
                  }else
                  {
                     *Number_Of_ARCs = *Number_Of_ARCs + 1;
                     DT2 = W1 * Penalty * (ReadyTimeContainer + Tij - ReadyTimeContainer2)
                         + W2 * Penalty * Tij;
                     //     - Late *(TimeValueInSecond - ReadyTimeContainer2)
                     // if (DT2 < 0) DT2 = 0;
                     //                 Agv m   C     L   U   C
                     Str1.printf("a    %5d     %5d    0   1   %10.1f", i1,i2,(float ) DT2 );
                     fprintf(fp,"%s  \n", Str1.c_str());
                     *(CostCToC + (RecTable2-1)*Number_Of_Container_Jobs + RecTable3-1) = DT2;
                  }
                }
                i2 = i2 + 2;
          }
          RecTable2++;
     }
}
//------------------------------------------------------------------------------
int  Read_Container_Jobs(int Dynamic_Flag)
{
     int  i;
     int  CTime = TimeValueInSecond;
     int  Time2 = 0 ; // StrToInt(MCFAlgorithmForm->Edit14->Text)*60;

     int Number_Of_Container_Jobs = PortContainerForm->Table2->RecordCount;

     PortContainerForm->Table2->First();
     for(i=1;i<=Number_Of_Container_Jobs;i++)
     {
          if(i > Maximum_Container_Jobs ) break;

          AnsiString ContainerIDStr          = PortContainerForm->Table2->FieldByName("ContainerID")->AsString;
          AnsiString SourcePointContainerStr = PortContainerForm->Table2->FieldByName("SOurcePoint")->AsString;
          AnsiString DestPointContainerStr   = PortContainerForm->Table2->FieldByName("DestPoint")->AsString;
          int ReadyTimeContainer             = StrToInt(PortContainerForm->Table2->FieldByName("ReadyTime")->AsString);
          int QuayCraneTime                  = StrToInt(PortContainerForm->Table2->FieldByName("QuayCraneTime")->AsString);

          Container_Buff[i-1].IDStr =  ContainerIDStr;
          Container_Buff[i-1].SourcePointStr = SourcePointContainerStr;
          Container_Buff[i-1].DestPointStr   = DestPointContainerStr;
          Container_Buff[i-1].ReadyTime      = ReadyTimeContainer;
          Container_Buff[i-1].QCraneTime     = QuayCraneTime;

        //  if( Dynamic_Flag )
        //  {
        //     if( ReadyTimeContainer >  TimeValueInSecond + Time2 ) break;
        //  }

          PortContainerForm->Table2->Next();
     }
     return(i-1);
}
//------------------------------------------------------------------------------
void __fastcall Set_Cranes_And_Vehicles_For_Next(int Number_of_Cranes)
{

  for(int c=1;c<=Maximum_Number_Cranes;c++)
  {
      Crane_Buff[c-1].Number_Of_Done_Jobs = 0;
      Crane_Buff[c-1].Next_Generation_Time= StrToInt(MCFAlgorithmForm->Edit23->Text);
      Crane_Buff[c-1].Last_Completed_Time = StrToInt(MCFAlgorithmForm->Edit23->Text);
  }
  int Number_Of_AGVs = PortAGVForm->Table2->RecordCount;
  for(int m=1;m<=Number_Of_AGVs;m++)
  {
        Vehicle_Buff[m-1].Number_Of_Jobs = 0;
        Vehicle_Buff[m-1].Last_Completed_Time = TimeValueInSecond;
  }

/*   for (int c=1;c<=Number_of_Cranes;c++)
   {
        Crane_Buff[c-1].Number_Of_Jobs = 0;
        Crane_Buff[c-1].Last_Completed_Time = TimeValueInSecond;
        Crane_Buff[c-1].Next_Generation_Time = TimeValueInSecond;
   }
   int Number_Of_AGVs = PortAGVForm->Table2->RecordCount;
   int Crane_Number;
   for(int m=1;m<=Number_Of_AGVs;m++)
   {
        Vehicle_Buff[m-1].Number_Of_Jobs = 0;
        Vehicle_Buff[m-1].Last_Completed_Time = TimeValueInSecond;
   }

   PortAGVForm->Table2->First() ;
   for(int m=1;m<=Number_Of_AGVs;m++)
   {
        if(MCFAlgorithmForm->Table4->RecordCount > 0)
        {
            MCFAlgorithmForm->Table4->First() ;
            AnsiString SourcePointStr = MCFAlgorithmForm->Table4->FieldByName("SourcePoint")->AsString;
            AnsiString DestPointStr   = MCFAlgorithmForm->Table4->FieldByName("DestPoint")->AsString;
            int        ActualTime  = MCFAlgorithmForm->Table4->FieldValues["ActualTime"];
            if (SourcePointStr.SubString(1,3)== "W/P")
                Crane_Number = StrToInt(SourcePointStr.SubString(5,SourcePointStr.Length()-4 ));
            if (DestPointStr.SubString(1,3)== "W/P")
                Crane_Number = StrToInt(DestPointStr.SubString(5,DestPointStr.Length()-4 ));

            Crane_Buff[Crane_Number-1].Last_Completed_Time  = ActualTime;

            Vehicle_Buff[m-1].Last_Completed_Time  = ActualTime;
        }
        PortAGVForm->Table2->Next() ;
   }
  */
}
//------------------------------------------------------------------------------
void __fastcall TMCFAlgorithmForm::ListBox1Click(TObject *Sender)
{
     TLocateOptions Opts;

     Variant locvalues[2];

     Opts.Clear();

     locvalues[0] = ListBox1->Items->Strings[ListBox1->ItemIndex];

     bool b = OpenPortForm->Table2->Locate("Portname", VarArrayOf(locvalues, 0), Opts);
     if (b)
     {
         Edit3->Text  = OpenPortForm->Table2->FieldByName("NumberOfBlockYard")->AsString;
         Edit6->Text  = OpenPortForm->Table2->FieldByName("NumberOfWorkingPosition")->AsString;
         Edit4->Text  = OpenPortForm->Table2->FieldByName("NumberOfAGVs")->AsString;
     } else
         return;

     PortLayoutForm->Table2->Filter   =   AnsiString("Portname = '") + ListBox1->Items->Strings[ListBox1->ItemIndex] + "'";

     MCFAlgorithmForm->Table1->Filter = AnsiString("Portname = '") + ListBox1->Items->Strings[ListBox1->ItemIndex] + "'";

     Label35->Caption = ListBox1->Items->Strings[ListBox1->ItemIndex];
     MCFAlgorithmForm->Table1->Filtered = true;

     PortContainerForm->Table2->Filter= AnsiString("Portname = '") + ListBox1->Items->Strings[ListBox1->ItemIndex] + "'";

     PortAGVForm->Table2->Filter = AnsiString("Portname = '") + ListBox1->Items->Strings[ListBox1->ItemIndex] + "'";

     PortLayoutForm->DataSource1->Enabled = true;
     PortContainerForm->DataSource1->Enabled = true;
     PortAGVForm->DataSource1->Enabled = true;
     DataSource3->Enabled = true;
     DataSource1->Enabled = true;
     Button11->Enabled = true;
     Set_Cranes_And_Vehicles_For_Next(StrToInt(Edit6->Text)); // Static fashion
}
//------------------------------------------------------------------------------
void __fastcall TMCFAlgorithmForm::TabControl1Change(TObject *Sender)
{
    switch(TabControl1->TabIndex)
    {
       case 0 :  // Static
                GroupBox7->Visible = false;
                GroupBox10->Visible = false;
                GroupBox18->Visible = false;
                GroupBox19->Visible = false;
                Button4->Enabled = true;
                GroupBox3->Visible = true;

                break;
       case 1 : // Model
                GroupBox3->Visible = false;
                GroupBox7->Visible = false;
                GroupBox18->Visible = false;
                GroupBox19->Visible = false;
                Button4->Enabled = false;

                GroupBox10->Visible = true;

                break;
       case 2 : // Dynamic
                GroupBox3->Visible = false;
                GroupBox10->Visible = false;
                GroupBox18->Visible = false;
                GroupBox19->Visible = false;
                Button4->Enabled = false;
                ListBox3->ItemIndex = ListBox1->ItemIndex;
                ListBox3Click(Sender);
                GroupBox7->Visible = true;

                break;
       case 3 : // Result
                GroupBox3->Visible = false;
                GroupBox10->Visible = false;
                GroupBox7->Visible = false;
                GroupBox19->Visible = false;
                Button4->Enabled = false;
                GroupBox18->Visible = true;

                break;
       case 4 : // Graph
                GroupBox3->Visible = false;
                GroupBox10->Visible = false;
                GroupBox7->Visible = false;
                GroupBox18->Visible = false;
                GroupBox19->Visible = true;
                Button4->Enabled = false;
                PortAGVForm->Table2->First() ;
                for(int m=1;m<=PortAGVForm->Table2->RecordCount;m++)
                {
                   QRChart1->Chart->Series[m-1]->AddXY(m,PortAGVForm->Table2->FieldValues["CostAGV"],"Cost",0);
                   QRChart1->Chart->Series[m-1]->AddXY(m,PortAGVForm->Table2->FieldValues["WaitedTime"],"Wait",0);
               //    QRChart1->Chart->Series[m-1]->XValues->Value[0] = m;
                //   QRChart1->Chart->Series[m-1]->YValues->Value[0] = PortAGVForm->Table2->FieldValues["CostAGV"];
                 //  QRChart1->Chart->Series[m-1]->XValues->Value[1] = m;
                  // QRChart1->Chart->Series[m-1]->YValues->Value[1] = PortAGVForm->Table2->FieldValues["WaitedTime"];
                   PortAGVForm->Table2->Next() ;
                }
                break;
    }
}
//------------------------------------------------------------------------------

void __fastcall TMCFAlgorithmForm::ListBox3Click(TObject *Sender)
{
     TLocateOptions Opts;

     int  Number_Of_Container_Jobs, Number_Of_Nodes;
     int  ReadyTimeAGV;
     int  Tij, Number_ARCs;

     int  *TAGVToC, *TCToC;

     int  *CostAGVToC,*CostCToC;

     if(ListBox3->ItemIndex < 0) return;

     AnsiString PortNameStr, Str1 , Str2, SinkNodeStr;
     AnsiString DistanceStr;
     Variant locvalues[2];

     Opts.Clear();

     PortNameStr = ListBox3->Items->Strings[ListBox3->ItemIndex];
     locvalues[0] = PortNameStr;

     bool b = OpenPortForm->Table2->Locate("Portname", VarArrayOf(locvalues, 0), Opts);
     if (b)
     {
         Edit11->Text  = OpenPortForm->Table2->FieldByName("NumberOfBlockYard")->AsString;
         Edit12->Text  = OpenPortForm->Table2->FieldByName("NumberOfWorkingPosition")->AsString;
         Edit7->Text   = OpenPortForm->Table2->FieldByName("NumberOfAGVs")->AsString;
     } else
         return;


     MCFAlgorithmForm->Table1->Filter  = AnsiString("Portname = '") + ListBox3->Items->Strings[ListBox3->ItemIndex] + "'";
     MCFAlgorithmForm->Table1->Filtered= true;

     Label35->Caption = ListBox3->Items->Strings[ListBox3->ItemIndex];
     MCFAlgorithmForm->Table1->Filtered = true;

     PortContainerForm->Table2->Filter  = AnsiString("Portname = '") + ListBox3->Items->Strings[ListBox3->ItemIndex] + "'";
     PortContainerForm->Table2->Filtered= true;

     PortLayoutForm->DataSource1->Enabled = true;
     PortContainerForm->DataSource1->Enabled = true;
     PortAGVForm->DataSource1->Enabled = true;
     DataSource3->Enabled = true;
     DataSource1->Enabled = true;

     PortLayoutForm->Table2->Filter    = AnsiString("Portname = '") + ListBox3->Items->Strings[ListBox3->ItemIndex] + "'";
     PortLayoutForm->Table2->Filtered  = true;


     PortAGVForm->Table2->Filter =   AnsiString("Portname = '") + ListBox3->Items->Strings[ListBox3->ItemIndex] + "'";
     PortAGVForm->Table2->Filtered = true;

     Set_Cranes_And_Vehicles_For_Next(StrToInt(Edit12->Text));// Start of Dynamic fashio

}
//------------------------------------------------------------------------------

void __fastcall TMCFAlgorithmForm::Button3Click(TObject *Sender)
{
     DataSource3->Enabled = false;
     PortAGVForm->DataSource1->Enabled = false;
     DataSource1->Enabled = false;


     PortLayoutForm->DataSource1->Enabled = false;
     PortContainerForm->DataSource1->Enabled = false;
     Close();
}
//------------------------------------------------------------------------------
void __fastcall Set_Staus_of_Vehilce_to_Stopping()
{
       int Number_Of_AGVs = PortAGVForm->Table2->RecordCount;

       int Current_AGV = PortAGVForm->Table2->RecNo  ;

       PortAGVForm->Table2->First() ;
       for(int m=1;m<=Number_Of_AGVs;m++)
       {
           PortAGVForm->Table2->Edit() ;
           PortAGVForm->Table2->FieldValues["Status"] = "Stopping" ;
           PortAGVForm->Table2->Post() ;
           PortAGVForm->Table2->Next() ;
       }
       PortAGVForm->Table2->Refresh();
       MCFAlgorithmForm->DBGrid4->Refresh ();

       PortAGVForm->Table2->RecNo = Current_AGV;
}
//------------------------------------------------------------------------------
void __fastcall TMCFAlgorithmForm::Button4Click(TObject *Sender)
{
       if( ListBox1->ItemIndex < 0) return;
       AnsiString PortNameStr = ListBox1->Items->Strings[ListBox1->ItemIndex];
       Set_Empty_Table4_For_Specific_Port(PortNameStr);
       Set_Staus_of_Vehilce_to_Stopping();
}
//------------------------------------------------------------------------------
void __fastcall TMCFAlgorithmForm::Button8Click(TObject *Sender)
{    // Stop
     Set_Staus_of_Vehilce_to_Stopping();

     ListBox3->Enabled = true;

     Screen->Cursor =  crArrow;
     Timer3->Enabled = false;
     Timer2->Enabled = false;

     Edit11->Enabled = true;
  //   Edit14->Enabled = true;
//     Edit15->Enabled = true;
     Edit9->Enabled  = true;
     Edit10->Enabled = true;
     Edit19->Enabled = true;
     Edit18->Enabled = true;

     Button7->Enabled = true;

     Button8->Enabled = false;
     Button7->Enabled = true;  // Run
     Button9->Enabled = true;  // Reset

     Button1->Enabled = false;
     Button3->Enabled = true; //  Close
     Button2->Enabled = false;

     Timer1->Enabled = false;

     TimeValueInSecond = 0;
}
//------------------------------------------------------------------------------

void __fastcall TMCFAlgorithmForm::Button7Click(TObject *Sender)
{                // RUN
     if(MCFAlgorithmForm->ListBox3->ItemIndex < 0 )
     {
        Application->MessageBox(" No port !! ", "Error", MB_OK);
        return;
     }
     ListBox3->Enabled = false;
     StartOfDynamicSchedule = true;

     Screen->Cursor =  crHourGlass;    // Show hourglass cursor

     int Number_Of_AGVs = PortAGVForm->Table2->RecordCount;

     int Current_AGV = PortAGVForm->Table2->RecNo  ;

     PortAGVForm->Table2->First() ;
     for(int m=1;m<=Number_Of_AGVs;m++)
     {
           PortAGVForm->Table2->Edit() ;
           PortAGVForm->Table2->FieldValues["CostAGV"] = 0;
           PortAGVForm->Table2->FieldValues["WaitedTime"] = 0;
           PortAGVForm->Table2->Post() ;
           PortAGVForm->Table2->Next() ;
     }
     PortAGVForm->Table2->Refresh();

     MCFAlgorithmForm->DBGrid4->Columns->Items[6]->Title->Caption  = "Travelled Time"  ;
     MCFAlgorithmForm->DBGrid4->Refresh ();

     PortAGVForm->Table2->RecNo = Current_AGV;

 //    SwitchMode();
 //    SwitchMode();
     Timer3->Enabled = true;
 //    Timer2->Enabled = true;

     Button1->Enabled = false; // Resume
     Button2->Enabled = true; //  Suspend
     Button3->Enabled = false; //  Close
     Button7->Enabled = false;
     Button8->Enabled = true; //  Stop
     Button9->Enabled = false; //  Reset

     Edit11->Enabled = false;
//     Edit14->Enabled = false;
//     Edit15->Enabled = false;
     Edit18->Enabled = false;
     Edit9->Enabled  = false;
     Edit10->Enabled = false;
     Edit19->Enabled = false;

  //   if(Edit14->Text.c_str() == "")
   //     Edit14->Text = "5";

  //   Timer1->Interval = StrToInt(Edit14->Text)*1000*60 ;

     TDateTime DateTime = Time();  // store the current date and time
     AnsiString str = TimeToStr(DateTime); // convert the time to a string
     Label27->Caption = "  Start  Time: " + str;  // display the time on the form's caption

  //   GlobalInitialTimeQuayCrane = StrToInt(MCFAlgorithmForm->Edit15->Text);

  //   Timer1Timer(Sender);

  //   Timer1->Enabled = true;
}
//------------------------------------------------------------------------------

void __fastcall TMCFAlgorithmForm::RadioButton5Click(TObject *Sender)
{
 //    Edit15->Enabled = false;
//     Edit14->Enabled = true;
}
//------------------------------------------------------------------------------

void __fastcall TMCFAlgorithmForm::RadioButton6Click(TObject *Sender)
{
//    Edit14->Enabled = false;
//    Edit15->Enabled = true;
}
//------------------------------------------------------------------------------

void __fastcall TMCFAlgorithmForm::Button2Click(TObject *Sender)
{    // Suspend
     Set_Staus_of_Vehilce_to_Stopping();
     Button2->Enabled = false;
     Button1->Enabled = true;

     Timer1->Enabled = false;
     Timer2->Enabled = false;
     Timer3->Enabled = false;
}
//------------------------------------------------------------------------------

void __fastcall TMCFAlgorithmForm::Button1Click(TObject *Sender)
{    // Resume
     Button2->Enabled = true;
     Button1->Enabled = false;
   //  Timer1->Enabled = true;
   //  Timer2->Enabled = true;
     Timer3->Enabled = true;
}
//------------------------------------------------------------------------------

void __fastcall TMCFAlgorithmForm::Table4CalcFields(TDataSet *DataSet)
{
       Table4WaitingTime->Value = Table4->FieldByName("VehicleTime")->Value
                                - Table4->FieldByName("QuayCraneTime")->Value;
}
//------------------------------------------------------------------------------
void __fastcall TMCFAlgorithmForm::FormActivate(TObject *Sender)
{
     ListBox1->Clear();
     ListBox3->Clear();

     OpenPortForm->Table2->First();
     while (!OpenPortForm->Table2->Eof)
     {
         ListBox1->Items->Add(OpenPortForm->Table2->FieldByName("Portname")->AsString);
         ListBox3->Items->Add(OpenPortForm->Table2->FieldByName("Portname")->AsString);
         OpenPortForm->Table2->Next();
     }

     MCFAlgorithmForm->Refresh() ;
}
//------------------------------------------------------------------------------
void  __fastcall Set_Next_Location_Vehicle_Q_Source_Q(int ActualTime, int PassTime,  bool StartOfNewSchedule,AnsiString PortNameStr,
                         int a,int b,AnsiString SourcePointContainerStr,AnsiString DestPointContainerStr)
{     // case 1: V.S.D=Q.Q.Y
      if ( a == 0) // Vehicle location = SourceContainer
      {
          if( ActualTime > PassTime)
          {
              PortAGVForm->Table2->FieldValues["ReadyTime"]    = 0 ;
              PortAGVForm->Table2->FieldValues["Status"] = "Waiting" ;

              int WaitedTime = PortAGVForm->Table2->FieldValues["WaitedTime"];
              WaitedTime++;
              PortAGVForm->Table2->FieldValues["WaitedTime"] = WaitedTime ;
          }else
          {
              PortAGVForm->Table2->FieldValues["PreviousLocation"]= SourcePointContainerStr;
              PortAGVForm->Table2->FieldValues["StartLocation"]   = DestPointContainerStr;
              PortAGVForm->Table2->FieldValues["ReadyTime"]       = b  + StrToInt(MCFAlgorithmForm->Edit19->Text ) ;
              PortAGVForm->Table2->FieldValues["Status"] = "Going" ;
              PortAGVForm->Table2->FieldValues["Load"]   = true;
           }
      } else // a > 0 , Source <> Vehicle's Location
      {
           PortAGVForm->Table2->FieldValues["PreviousLocation"]= PortAGVForm->Table2->FieldValues["StartLocation"];
           PortAGVForm->Table2->FieldValues["StartLocation"]= SourcePointContainerStr;
           PortAGVForm->Table2->FieldValues["ReadyTime"]    = a + StrToInt(MCFAlgorithmForm->Edit19->Text ) ;
           PortAGVForm->Table2->FieldValues["Status"] = "Going" ;
           PortAGVForm->Table2->FieldValues["Load"] = false;
      }
}
//---------------------------------------------------------------------------
void  __fastcall Set_Next_Location_Vehicle_Q_Dest_Q(int ActualTime, int PassTime,  bool StartOfNewSchedule,AnsiString PortNameStr,
                         int a,int b,AnsiString SourcePointContainerStr,AnsiString DestPointContainerStr)
{     // case 2: V.S.D=Q.Y.Q
      if(PortAGVForm->Table2->FieldValues["Load"])
      {
         if( ActualTime > PassTime)
         {
            PortAGVForm->Table2->FieldValues["ReadyTime"]    = 0 ;
            PortAGVForm->Table2->FieldValues["Status"] = "Waiting" ;

            int WaitedTime = PortAGVForm->Table2->FieldValues["WaitedTime"];
            WaitedTime++;
            PortAGVForm->Table2->FieldValues["WaitedTime"] = WaitedTime ;
         }else
         {
           MCFAlgorithmForm->Table4->Next() ;
           if(!MCFAlgorithmForm->Table4->Eof  )
           {
                AnsiString SourcePointContainerStr2 = MCFAlgorithmForm->Table4->FieldByName("SourcePoint")->AsString;
                AnsiString DestPointContainerStr2   = MCFAlgorithmForm->Table4->FieldByName("DestPoint")->AsString;
                AnsiString AGVLocation = PortAGVForm->Table2->FieldValues["StartLocation"];
                PortAGVForm->Table2->FieldValues["PreviousLocation"]= DestPointContainerStr;
                PortAGVForm->Table2->FieldValues["StartLocation"]= SourcePointContainerStr2;
                int d = Get_Time_Between_Two_Points(PortNameStr,AGVLocation , SourcePointContainerStr2);
                PortAGVForm->Table2->FieldValues["ReadyTime"]    = d + StrToInt(MCFAlgorithmForm->Edit19->Text ) ;
                PortAGVForm->Table2->FieldValues["Status"] = "Going" ;
                PortAGVForm->Table2->FieldValues["Load"] = false;
           }
         }
      }  else
      {
           PortAGVForm->Table2->FieldValues["PreviousLocation"]= PortAGVForm->Table2->FieldValues["StartLocation"];
           PortAGVForm->Table2->FieldValues["StartLocation"]= SourcePointContainerStr;
           PortAGVForm->Table2->FieldValues["ReadyTime"]    = a + StrToInt(MCFAlgorithmForm->Edit19->Text );
           PortAGVForm->Table2->FieldValues["Status"] = "Going" ;
           PortAGVForm->Table2->FieldValues["Load"] = false;
      }
}
//------------------------------------------------------------------------------
void  __fastcall Set_Next_Location_Vehicle_Y_Source_Q(int ActualTime, int PassTime, bool StartOfNewSchedule,AnsiString PortNameStr,
                         int a,int b,AnsiString SourcePointContainerStr,AnsiString DestPointContainerStr)
{  // case 3: V.S.D=Y.Q.Y
           PortAGVForm->Table2->FieldValues["PreviousLocation"]= PortAGVForm->Table2->FieldValues["StartLocation"];
           PortAGVForm->Table2->FieldValues["StartLocation"]= SourcePointContainerStr;
           PortAGVForm->Table2->FieldValues["ReadyTime"]    = a;
           PortAGVForm->Table2->FieldValues["Status"] = "Going" ;
           PortAGVForm->Table2->FieldValues["Load"] = false;
}
//------------------------------------------------------------------------------
void  __fastcall Set_Next_Location_Vehicle_Y_Dest_Q(int ActualTime, int PassTime, bool StartOfNewSchedule,AnsiString PortNameStr,
                         int a,int b,AnsiString SourcePointContainerStr,AnsiString DestPointContainerStr)
{   // case 4: V.S.D=Y.Y.Q
    if ( a == 0) // Vehicle location = Source
    {
           PortAGVForm->Table2->FieldValues["Load"] = true;
           PortAGVForm->Table2->FieldValues["PreviousLocation"]= PortAGVForm->Table2->FieldValues["StartLocation"];
           PortAGVForm->Table2->FieldValues["StartLocation"]= DestPointContainerStr;
           PortAGVForm->Table2->FieldValues["ReadyTime"]    = b;
           PortAGVForm->Table2->FieldValues["Status"] = "Going" ;
    }else
    {
           PortAGVForm->Table2->FieldValues["PreviousLocation"]= PortAGVForm->Table2->FieldValues["StartLocation"];
           PortAGVForm->Table2->FieldValues["StartLocation"]= SourcePointContainerStr;
           PortAGVForm->Table2->FieldValues["ReadyTime"]    = a;
           PortAGVForm->Table2->FieldValues["Status"] = "Going" ;
           PortAGVForm->Table2->FieldValues["Load"] = false;
    }
}
//------------------------------------------------------------------------------
void   __fastcall  Set_Location_And_ReadyTime_AGV(AnsiString PortNameStr,int m,
                                                  int PassTime,bool StartOfNewSchedule)
{
        TLocateOptions Opts;
        Opts.Clear();
        Variant locvalues[3];

        AnsiString SourcePointContainerStr;
        AnsiString DestPointContainerStr  ;

        AnsiString AGVLocation;
        int ReadyTimeAGV;

        int a ;
        int b ;

        AGVLocation  = PortAGVForm->Table2->FieldValues["StartLocation"];
        ReadyTimeAGV = PortAGVForm->Table2->FieldValues["ReadyTime"];

        if(--ReadyTimeAGV > 0)
        {
            locvalues[0]= AGVLocation;
            locvalues[1]= ReadyTimeAGV;
            bool b = PortAGVForm->Table1->Locate("StartLocation;Readytime", VarArrayOf(locvalues, 1), Opts);

            AnsiString AGVName  = PortAGVForm->Table1->FieldValues["AGVName"];
            AnsiString PortName = PortAGVForm->Table1->FieldValues["PortName"];
            AnsiString AGVNameCurr = "AGV-";  AGVNameCurr = AGVNameCurr + m;

            PortAGVForm->Table2->Edit() ;
            if (b &&  AGVNameCurr != AGVName && PortName == PortNameStr)
            {
               PortAGVForm->Table2->FieldValues["ReadyTime"] = ReadyTimeAGV + 1;
               PortAGVForm->Table2->FieldValues["Status"] = "Collision" ;
               int WaitedTime = PortAGVForm->Table2->FieldValues["WaitedTime"];
               WaitedTime++;
               PortAGVForm->Table2->FieldValues["WaitedTime"] = WaitedTime ;

                // Update Vehicle time and Actual time: Increase Vehicle Time one second
               if(MCFAlgorithmForm->Table4->RecordCount > 0)
               {
                  MCFAlgorithmForm->Table4->First();
                  int ActualTime  = MCFAlgorithmForm->Table4->FieldValues["ActualTime"] ;
                  int VehicleTime = MCFAlgorithmForm->Table4->FieldValues["VehicleTime"];
                  if( ActualTime > VehicleTime)
                  {
                    MCFAlgorithmForm->Table4->Edit();
                    MCFAlgorithmForm->Table4->FieldValues["VehicleTime"]= VehicleTime + 1;
                    MCFAlgorithmForm->Table4->FieldValues["ActualTime"] = ActualTime + 1;
                    MCFAlgorithmForm->Table4->Post();
                  }
                }  
            } else
            {
               PortAGVForm->Table2->FieldValues["ReadyTime"] = ReadyTimeAGV;
               PortAGVForm->Table2->FieldValues["Status"] = "Going" ;

               int TravelledTime = PortAGVForm->Table2->FieldValues["CostAGV"];
               TravelledTime++;
               PortAGVForm->Table2->FieldValues["CostAGV"] = TravelledTime;
            }
            PortAGVForm->Table2->Post() ;
            return;
        }

        PortAGVForm->Table2->Edit() ;

        // ReadyTimeAGV = 0;
        bool Load = PortAGVForm->Table2->FieldValues["Load"];
        if(AGVLocation.SubString(1,3)  == "W/P")
        {
              if( !MCFAlgorithmForm->Table4->Eof )
              {
                MCFAlgorithmForm->Table4->First();
                SourcePointContainerStr = MCFAlgorithmForm->Table4->FieldByName("SourcePoint")->AsString;
                DestPointContainerStr   = MCFAlgorithmForm->Table4->FieldByName("DestPoint")->AsString;
                int ActualTime          = MCFAlgorithmForm->Table4->FieldValues["ActualTime"];
                a = Get_Time_Between_Two_Points(PortNameStr,AGVLocation             ,SourcePointContainerStr);
                b = Get_Time_Between_Two_Points(PortNameStr,SourcePointContainerStr ,DestPointContainerStr);
                if(SourcePointContainerStr.SubString(1,3) == "W/P")
                   Set_Next_Location_Vehicle_Q_Source_Q(ActualTime, PassTime,StartOfNewSchedule,PortNameStr,
                                             a,b,SourcePointContainerStr,DestPointContainerStr);
                else
                   Set_Next_Location_Vehicle_Q_Dest_Q(ActualTime, PassTime,StartOfNewSchedule,PortNameStr,
                                             a,b,SourcePointContainerStr,DestPointContainerStr);
              }else
              {
                 PortAGVForm->Table2->FieldValues["Status"] = "Idle" ;
                 PortAGVForm->Table2->FieldValues["Load"]   = false ;
                 PortAGVForm->Table2->FieldValues["ReadyTime"]    = 0 ;
              }
        }else // AGVLocation = Yard
        {
              if( !MCFAlgorithmForm->Table4->Eof )
              {
                MCFAlgorithmForm->Table4->First();
                SourcePointContainerStr = MCFAlgorithmForm->Table4->FieldByName("SourcePoint")->AsString;
                DestPointContainerStr   = MCFAlgorithmForm->Table4->FieldByName("DestPoint")->AsString;
                int ActualTime          = MCFAlgorithmForm->Table4->FieldValues["ActualTime"];
                a = Get_Time_Between_Two_Points(PortNameStr,AGVLocation             ,SourcePointContainerStr);
                b = Get_Time_Between_Two_Points(PortNameStr,SourcePointContainerStr ,DestPointContainerStr);
                if(SourcePointContainerStr.SubString(1,3) == "W/P")
                   Set_Next_Location_Vehicle_Y_Source_Q(ActualTime, PassTime,StartOfNewSchedule, PortNameStr,
                                             a,b,SourcePointContainerStr,DestPointContainerStr);
                else
                   Set_Next_Location_Vehicle_Y_Dest_Q(ActualTime, PassTime, StartOfNewSchedule, PortNameStr,
                                             a,b,SourcePointContainerStr,DestPointContainerStr);
              }else
              {
                 PortAGVForm->Table2->FieldValues["Status"] = "Idle" ;
                 PortAGVForm->Table2->FieldValues["Load"]   = false;
                 PortAGVForm->Table2->FieldValues["ReadyTime"]    = 0 ;
              }
        }
        PortAGVForm->Table2->Post() ;
}
//------------------------------------------------------------------------------
void  Delete_Processed_Container_Jobs(long int Passed_Time, bool StartOfNewSchedule)
{
       TLocateOptions Opts;
       Set  <TLocateOption,0,1> flags;
    //   flags << loCaseInsensitive << loPartialKey;
       flags << loPartialKey;

       long int ReadyTimeContainer;
       long int DueTimeContainer;

       if(MCFAlgorithmForm->ListBox3->ItemIndex < 0 ) return;

       Variant locvalues[3];
       AnsiString PortNameStr = MCFAlgorithmForm->ListBox3->Items->Strings[MCFAlgorithmForm->ListBox3->ItemIndex];
       locvalues[0] = PortNameStr;

       flags << loCaseInsensitive << loPartialKey;
       Opts.Clear();

       int Number_Of_AGVs = PortAGVForm->Table2->RecordCount;

       PortAGVForm->Table2->First() ;
       for(int m=1;m<=Number_Of_AGVs;m++)
       {
             while(MCFAlgorithmForm->Table4->RecordCount > 0)
             {
                MCFAlgorithmForm->Table4->First() ;
                int ActualTime  = MCFAlgorithmForm->Table4->FieldValues["ActualTime"];
                if( ActualTime >  Passed_Time )
                     break;
                else
                {
                   AnsiString ContainerID = MCFAlgorithmForm->Table4->FieldByName("ContainerID")->AsString;
                   AnsiString PortName    = MCFAlgorithmForm->Table4->FieldValues["Portname"];

                   AnsiString AGVNameStrS = MCFAlgorithmForm->Table4->FieldValues["AGVName"];
                   int        AGVNumber   = MCFAlgorithmForm->Table4->FieldValues["AGVNumber"];
                   int        ReadyTime   = MCFAlgorithmForm->Table4->FieldValues["ContainerReadyTime"];
                   int        VehicleTime = MCFAlgorithmForm->Table4->FieldValues["VehicleTime"];
                   int        ActualTime  = MCFAlgorithmForm->Table4->FieldValues["ActualTime"];
                   AnsiString SourcePoint = MCFAlgorithmForm->Table4->FieldByName("SourcePoint")->AsString;
                   AnsiString DestPoint   = MCFAlgorithmForm->Table4->FieldByName("DestPoint")->AsString;

                   MCFAlgorithmForm->Table4->Delete() ;

                   locvalues[0] = PortName;
                   locvalues[1] = ContainerID;
                   Opts << loCaseInsensitive ;

                   bool b = PortContainerForm->Table1->Locate("Portname;ContainerID", VarArrayOf(locvalues, 1) , Opts);
                   if (b)
                   {
                     int Crane_Number;
                     if (SourcePoint.SubString(1,3)== "W/P")
                         Crane_Number = StrToInt(SourcePoint.SubString(5,SourcePoint.Length()-4 ));
                     if (DestPoint.SubString(1,3)== "W/P")
                         Crane_Number = StrToInt(DestPoint.SubString(5,DestPoint.Length()-4 ));

                     Crane_Buff[Crane_Number-1].Last_Completed_Time = ActualTime;
                     Crane_Buff[Crane_Number-1].Number_Of_Jobs --;
                     Crane_Buff[Crane_Number-1].Number_Of_Done_Jobs++;

                     Vehicle_Buff[AGVNumber-1].Last_Completed_Time = ActualTime;
                     Vehicle_Buff[AGVNumber-1].Number_Of_Jobs --;
                     Vehicle_Buff[AGVNumber-1].Number_Of_Done_Jobs++;


                     MCFAlgorithmForm->Table1->Append ();
                     MCFAlgorithmForm->Table1->FieldByName("ContainerID")->AsString = ContainerID;
                     MCFAlgorithmForm->Table1->FieldValues["Portname"]              = PortName   ;
                     MCFAlgorithmForm->Table1->FieldValues["AGVName"]               = AGVNameStrS;
                     MCFAlgorithmForm->Table1->FieldValues["AGVNumber"]             = AGVNumber;
                     MCFAlgorithmForm->Table1->FieldValues["ContainerReadyTime"]    = ReadyTime  ;
                     MCFAlgorithmForm->Table1->FieldValues["VehicleTime"]           = VehicleTime;
                     MCFAlgorithmForm->Table1->FieldValues["ActualTime"]            = ActualTime ;
                     MCFAlgorithmForm->Table1->FieldValues["SourcePoint"]           = SourcePoint;
                     MCFAlgorithmForm->Table1->FieldValues["DestPoint"]             = DestPoint  ;
                     MCFAlgorithmForm->Table1->Post ();

                     PortContainerForm->Table1->Delete();
                   }
                   PortAGVForm->Table2->Edit();
                   int NumContainers = PortAGVForm->Table2->FieldValues["NumberOfContainers"] ;
                   if ( NumContainers > 0 )
                       PortAGVForm->Table2->FieldValues["NumberOfContainers"] = NumContainers - 1;
                   PortAGVForm->Table2->Post();

                 }
             }
             PortAGVForm->Table2->Next() ;
       }

       PortContainerForm->Table2->Refresh();
       PortAGVForm->Table2->Refresh();
       MCFAlgorithmForm->DBGrid4->Refresh ();

       MCFAlgorithmForm->Table4->Refresh() ;
       MCFAlgorithmForm->DBChart1->Refresh() ;
       MCFAlgorithmForm->DBGrid3->Refresh ();
}
//------------------------------------------------------------------------------
void   __fastcall TMCFAlgorithmForm::Insert_Containers_To_Table(int Number_Of_Container_Per_Minute,int i2)
{
       int i,i1,r;
       AnsiString BlockStrS, BlockStrD;

       static Next_Quay_Crane = 1;
       if (StartOfDynamicSchedule) Next_Quay_Crane = 1;

       long int BaseTime = TimeValueInSecond + GlobalInitialTimeQuayCrane;

       int Quay_Crane_Time_Window = StrToInt(MCFAlgorithmForm->Edit18->Text);
       int Number_Of_Cranes       = StrToInt(MCFAlgorithmForm->Edit12->Text);

       StartOfDynamicSchedule = true;

       TDateTime DateTime = Time();
       AnsiString TimeStr = TimeToStr(DateTime);
       TimeStr = TimeStr.SubString(1,6);

       MCFAlgorithmForm->Label22->Color = clYellow;
       MCFAlgorithmForm->Label22->Caption = "Incoming Containers...";
       MCFAlgorithmForm->GroupBox16->Refresh();


       for(i=1; i<=Number_Of_Container_Per_Minute;i++)
       {
          r = 3 + random(2); // 4 if we like four cases
           // 3 because we like to have only Yard to berth and vice versa
          switch(r)
          {
             case 3: // Yard to Berth
                i1 = 1+ random(StrToInt(MCFAlgorithmForm->Edit11->Text));
                BlockStrS = "Block "; BlockStrS = BlockStrS + i1;
                BlockStrD = "W/P "; BlockStrD = BlockStrD + i2;
                break;
             case 4: // Berth to Yard
                i1 = 1+ random(StrToInt(MCFAlgorithmForm->Edit11->Text));
                BlockStrS = "W/P "; BlockStrS = BlockStrS + i2;
                BlockStrD = "Block "; BlockStrD = BlockStrD + i1;
                break;
          }
          PortContainerForm->Table1->Append();

          AnsiString ContainerIDStrS = "C-";
          ContainerIDStrS = ContainerIDStrS + IntToStr(TimeValueInSecond)
                          + IntToStr(i1) + IntToStr(i2) + IntToStr(i);

          PortContainerForm->Table1->FieldValues["ContainerID"]= ContainerIDStrS;
          PortContainerForm->Table1->FieldValues["Portname"]   = MCFAlgorithmForm->ListBox3->Items->Strings[MCFAlgorithmForm->ListBox3->ItemIndex];

          PortContainerForm->Table1->FieldValues["SourcePoint"]= BlockStrS;
          PortContainerForm->Table1->FieldValues["DestPoint"]  = BlockStrD;

          int ReadyTime = Quay_Crane_Time_Window + Crane_Buff[i2-1].Next_Generation_Time;
          Crane_Buff[i2-1].Next_Generation_Time = ReadyTime;

          PortContainerForm->Table1->FieldValues["ReadyTime"]     = ReadyTime;
          PortContainerForm->Table1->FieldValues["QuayCraneTime"] = ReadyTime;

          PortContainerForm->Table1->Post();

       }
       MCFAlgorithmForm->Label22->Color = clYellow;
       MCFAlgorithmForm->Label22->Caption = "Containers came ";
       MCFAlgorithmForm->GroupBox16->Refresh();
       PortContainerForm->Table2->Refresh() ;
}
//------------------------------------------------------------------------------
void __fastcall  Write_Model_To_ListBox2()
{    // Static Fashion
     MCFAlgorithmForm->ListBox2->Items->Clear();
     FILE *fp = fopen("temp.inp","r+b"); // outstream  string
     char buff[100];
     while(1)
     {
          if(feof(fp)) break;
          if(!fgets(buff,100,fp)) break;
          MCFAlgorithmForm->ListBox2->Items->Add(buff);
     }
     fclose(fp);
}
//------------------------------------------------------------------------------
void __fastcall Write_Solution_To_ListBox5()
{  //  Static fashion
         char buff[100];
     MCFAlgorithmForm->ListBox5->Items->Clear();
     FILE *fp2 = fopen("temp.out","r+b");
     while(1)
     {
         if(feof(fp2)) break;
         if(!fgets(buff,100,fp2)) break;
         MCFAlgorithmForm->ListBox5->Items->Add(buff);
     }
     fclose(fp2);
}
//------------------------------------------------------------------------------
void __fastcall TMCFAlgorithmForm::Timer1Timer(TObject *Sender)
{
     TLocateOptions Opts;

     int   Number_Of_Container_Per_Minute =  StrToInt(Edit9->Text);
     if(Number_Of_Container_Per_Minute == 0) return;

     int   Number_Of_AGVs, Number_Of_Container_Jobs, Number_Of_Nodes, Number_Of_ARCs=0;

     int   ReadyTimeAGV;
     int   Tij, Number_ARCs;
     int   *TimeAGVToC,*TimeCToC;

     int   *CostAGVToC,*CostCToC;

     if(ListBox3->ItemIndex < 0) return;

     AnsiString PortNameStr, Str1 , Str2, SinkNodeStr;
     AnsiString DistanceStr;
     Variant locvalues[2];

     PortNameStr = ListBox3->Items->Strings[ListBox3->ItemIndex];
     locvalues[0] = PortNameStr;

     Opts.Clear();

     bool b = OpenPortForm->Table2->Locate("Portname", VarArrayOf(locvalues, 0), Opts);
     if (b)
     {
         Edit3->Text  = OpenPortForm->Table2->FieldByName("NumberOfBlockYard")->AsString;
         Edit6->Text  = OpenPortForm->Table2->FieldByName("NumberOfWorkingPosition")->AsString;
         Edit4->Text  = OpenPortForm->Table2->FieldByName("NumberOfAGVs")->AsString;
         Number_Of_AGVs = PortAGVForm->Table2->RecordCount;
     } else
         return;

     PortLayoutForm->Table2->Filter   =   AnsiString("Portname = '") + ListBox3->Items->Strings[ListBox3->ItemIndex] + "'";

     PortContainerForm->Table2->Filter=   AnsiString("Portname = '") + ListBox3->Items->Strings[ListBox3->ItemIndex] + "'";

     PortAGVForm->Table2->Filter=   AnsiString("Portname = '") + ListBox3->Items->Strings[ListBox3->ItemIndex] + "'";


     // Edit1->Text  = PortContainerForm->Table2->RecordCount;

     //  Number_Of_Container_Jobs = PortContainerForm->Table2->RecordCount;
                                                   // Dynamic Flag
     Number_Of_Container_Jobs = Read_Container_Jobs(1);

     // Just for test
     Number_ARCs  =  ( Number_Of_AGVs * Number_Of_Container_Jobs)
                  +  ( Number_Of_Container_Jobs * 2)
                  +    Number_Of_AGVs
                  +  Number_Of_Container_Jobs * (Number_Of_Container_Jobs-1);

     ListBox2->Items->Clear();
     ListBox5->Items->Clear();

     if(Number_Of_Container_Jobs*Number_Of_AGVs == 0 ) return;


  //   Timer2->Enabled = false;
  //   Timer3->Enabled = false;

     Str1 = "c  Port Name : " ; Str1 = Str1 + ListBox3->Items->Strings[ListBox3->ItemIndex];
     ListBox2->Items->Add(Str1);

     Str1 = "c  Number of Container Jobs : " ; Str1 = Str1 + IntToStr(Number_Of_Container_Jobs);
     ListBox2->Items->Add(Str1);

     Str1 = "c  Number of AGVs           : " ; Str1 = Str1 + IntToStr(Number_Of_AGVs);
     ListBox2->Items->Add(Str1);

     Number_Of_Nodes = Number_Of_AGVs + Number_Of_Container_Jobs*2 + 1;

     Str1 = "p min " ;  // define Min, number of nodes and arcs
     Str1 = Str1 + IntToStr(Number_Of_Nodes)+ "    " + IntToStr(Number_ARCs);
     if( MCFAlgorithmForm->RadioButton7->Checked)
         ListBox2->Items->Add(Str1);
     SinkNodeStr = IntToStr(Number_Of_Nodes);

     TimeAGVToC = (int *) malloc(sizeof(int)*Number_Of_AGVs*Number_Of_Container_Jobs);
     TimeCToC   = (int *) malloc(sizeof(int)*Number_Of_Container_Jobs*Number_Of_Container_Jobs);

     CostAGVToC = (int *) malloc(sizeof(int)*Number_Of_AGVs*Number_Of_Container_Jobs);
     CostCToC   = (int *) malloc(sizeof(int)*Number_Of_Container_Jobs*Number_Of_Container_Jobs);

     if(!CostCToC || !CostAGVToC || !TimeAGVToC || !TimeCToC )
     {
         Application->MessageBox("Not Enough Memory !! ", "Error", MB_OK);
       //  Timer2->Enabled = true;
       //  Timer3->Enabled = true;
         return;
     }

     TCursor Save_Cursor = Screen->Cursor;
     Screen->Cursor = crHourGlass;    // Show hourglass cursor

     Label22->Color = clBlue ;
     Label22->Caption = "Making the Model..."; // Danamic fashion
     GroupBox16->Refresh();


     FILE *fp = fopen("temp.inp","w+b"); // outstream  string

     Str1 = "p min " ;  // define Min, number of nodes and arcs
     Str1 = Str1 + IntToStr(Number_Of_Nodes)+ "   " + IntToStr(Number_ARCs) ;
     if( MCFAlgorithmForm->RadioButton7->Checked)
         ListBox2->Items->Add(Str1);

     fprintf(fp,"%s \n", Str1);
     SinkNodeStr = IntToStr(Number_Of_Nodes);

     Append_Supply_Nodes(Number_Of_AGVs,fp);

     Append_Demand_Node (Number_Of_AGVs,Number_Of_Nodes,fp);

     Append_Arcs_From_AGVs_Nodes_To_Container_Nodes(Number_Of_AGVs,&Number_Of_ARCs,              //SF
                                    PortNameStr,TimeAGVToC,Number_Of_Container_Jobs,CostAGVToC,fp,0);

     Append_Arcs_From_AGV_Nodes_To_Sink_Node(Number_Of_AGVs,&Number_Of_ARCs,SinkNodeStr,fp);

     Append_Arcs_From_CNode_To_CPNodes(Number_Of_AGVs,Number_Of_Nodes,&Number_Of_ARCs,fp);

     Append_Arcs_From_CPNodes_To_Sink_Node(Number_Of_AGVs,Number_Of_Nodes,SinkNodeStr,&Number_Of_ARCs,fp);

     Append_Arcs_From_CPNodes_To_Any_Other_CNodes(Number_Of_AGVs,Number_Of_Container_Jobs,
                                &Number_Of_ARCs,TimeCToC,CostCToC,PortNameStr,fp,0);
                                                                                //SF

     fclose(fp);

     if( MCFAlgorithmForm->RadioButton7->Checked)
         Write_Model_To_ListBox2();


     Label22->Color = clMaroon	;
     Label22->Caption = "Solving the Model...";
     GroupBox16->Refresh();

     int stat = MCF_Solve( 2,"temp.inp","temp.out"); // Dynamic Fashion
     switch(stat)
     {
        case -1 :
           Label22->Caption = "Not Enough Memory !! ";
           Label22->Color = clRed;
           break;
        case  1 :
           Label22->Caption = " Inconsistent Model!! ";
           Label22->Color = clRed;
           break;
        case 0 :
           Label22->Color = clGreen ;
           Label22->Caption = "Solution is Ready";

           if( MCFAlgorithmForm->RadioButton5->Checked)
              Write_Solution_To_ListBox5();

           Reset_Number_Of_Jobs_Before_Solution(StrToInt(Edit6->Text),StrToInt(Edit7->Text) );
           Read_Solution("temp.out",Number_ARCs, Number_Of_AGVs, Number_Of_Nodes,Sender,
                       0,TimeAGVToC,TimeCToC,Number_Of_Container_Jobs,CostAGVToC,CostCToC);
               //  StaticFlag
     }
     Screen->Cursor = Save_Cursor; // always restore the cursor
     GroupBox16->Refresh();
     free(TimeAGVToC);
     free(TimeCToC);
     free(CostAGVToC);
     free(CostCToC);

  //   Timer2->Enabled = true;
  //   Timer3->Enabled = true;
}
//------------------------------------------------------------------------------

void __fastcall TMCFAlgorithmForm::Button9Click(TObject *Sender)
{       // Reset
       if(MCFAlgorithmForm->ListBox3->ItemIndex < 0 )
       {
          Application->MessageBox(" No port !! ", "Error", MB_OK);
          return;
       }
         // Deleting container Jobs from Container Job Files
       TLocateOptions Opts;
       Set  <TLocateOption,0,1> flags;
       flags << loCaseInsensitive << loPartialKey;

       for(int c=1;c<=Maximum_Number_Cranes;c++)
       {
           Crane_Buff[c-1].Number_Of_Jobs  = 0;
           Crane_Buff[c-1].Number_Of_Done_Jobs = 0;
           Crane_Buff[c-1].Next_Generation_Time= StrToInt(Edit23->Text);
           Crane_Buff[c-1].Last_Completed_Time = StrToInt(Edit23->Text);
       }

       PortAGVForm->Table2->First() ;
       for(int v=1;v<=PortAGVForm->Table2->RecordCount;v++)
       {
           Vehicle_Buff[v-1].Last_Completed_Time = 0;
           Vehicle_Buff[v-1].Number_Of_Jobs      = 0;

           PortAGVForm->Table2->Edit() ;
           PortAGVForm->Table2->FieldValues["CostAGV"] = 0;
           PortAGVForm->Table2->FieldValues["Load"] = false;
           PortAGVForm->Table2->FieldValues["Status"] = "Idle" ;
           PortAGVForm->Table2->FieldValues["ReadyTime"] = 0;
           PortAGVForm->Table2->FieldValues["WaitedTime"] = 0;
           PortAGVForm->Table2->FieldValues["NumberOfContainers"] = 0;
           PortAGVForm->Table2->Post() ;

           MCFAlgorithmForm->Table4->Active = false;
           MCFAlgorithmForm->Table4->EmptyTable();
           MCFAlgorithmForm->Table4->Active = true;

           PortAGVForm->Table2->Next() ;
       }
       PortAGVForm->Table2->Refresh () ;

      PortContainerForm->Table2->Active = false;
      PortContainerForm->Table2->EmptyTable();
      PortContainerForm->Table2->Active = true;

      MCFAlgorithmForm->Table1->Active = false;
      MCFAlgorithmForm->Table1->EmptyTable();
      MCFAlgorithmForm->Table1->Active = true;

}
//------------------------------------------------------------------------------


void __fastcall TMCFAlgorithmForm::Label22Click(TObject *Sender)
{
   //  int   Number_Of_Container_Per_Minute =  StrToInt(Edit9->Text);
   //  if(Number_Of_Container_Per_Minute == 0) return;
   //  Insert_Containers_To_Table(Number_Of_Container_Per_Minute);
     //   Delete_Processed_Container_Jobs(100);
   //   Painta();
   // Timer1Timer(Sender);
}
//------------------------------------------------------------------------------

void __fastcall TMCFAlgorithmForm::Timer2Timer(TObject *Sender,int Crane_Number)
{
     int   Number_Of_Container_Per_Minute =  StrToInt(Edit9->Text);
     if(Number_Of_Container_Per_Minute == 0) return;
     Insert_Containers_To_Table(Number_Of_Container_Per_Minute,Crane_Number);
}
//------------------------------------------------------------------------------

void __fastcall TMCFAlgorithmForm::Painta()
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
     	Image10->Canvas->Pen->Color = clBlack;
     	Image10->Canvas->Pen->Mode = pmNotXor;
     	if (DrawHand > SecondHand)
     	{
     		Image10->Canvas->MoveTo(ClockCenter.x,ClockCenter.y);
     		Image10->Canvas->LineTo(CoordSec.x,CoordSec.y);
     	}
     	else
     	{
     		if (FullClock)
     		    DrawClock(12,30,5);
     		else
     		    DrawClock(4,90,5);
     		DrawClock(60,6,2);
     		DrawHand = MinuteHand;
     	}
     	AngSec = ((float (NSec)) + (float (NMSec)/1000)) * 6;
     	CoordSec.x = ClockCenter.x + (ArmSec * sin(AngSec * DegToRad));
     	CoordSec.y = ClockCenter.y - (ArmSec * cos(AngSec * DegToRad));
     	Image10->Canvas->MoveTo(ClockCenter.x,ClockCenter.y);
     	Image10->Canvas->LineTo(CoordSec.x,CoordSec.y);

     	if (NMin!=CMin)
     	{
     		dtCurrent = EncodeTime((Word)CHour,(Word)NMin,(Word)NSec,(Word)0);
     		Minutebox->Text = IntToStr(NMin);
     		Hourbox->Text = IntToStr(NHour);
     		MinAngle = NMin * 6;
     		Image10->Canvas->Pen->Width = 2;
     		Image10->Canvas->Pen->Color = clRed; // clWindow;
     		if (DrawHand > MinuteHand)
     		{
     			Image10->Canvas->MoveTo(ClockCenter.x,ClockCenter.y);
     			Image10->Canvas->LineTo(CoordMin.x,CoordMin.y);
     		}
     		else
     			DrawHand = HourHand;
     		CoordMin.x = ClockCenter.x + (MinArm * sin(MinAngle * DegToRad));
     		CoordMin.y = ClockCenter.y - (MinArm * cos(MinAngle * DegToRad));
     		Image10->Canvas->MoveTo(ClockCenter.x,ClockCenter.y);
     		Image10->Canvas->LineTo(CoordMin.x,CoordMin.y);
     		if(NHour!=CHour)
     		{
     			HourAngle = NHour * 30;
     			Image10->Canvas->Pen->Width = 2;
     			if (DrawHand > HourHand)
     			{
     				Image10->Canvas->MoveTo(ClockCenter.x,ClockCenter.y);
     				Image10->Canvas->LineTo(CoordHour.x,CoordHour.y);
     			}
     			else
     				DrawHand=All;
     			CoordHour.x = ClockCenter.x + (HourArm * sin(HourAngle * DegToRad));
     			CoordHour.y = ClockCenter.y - (HourArm * cos(HourAngle * DegToRad));
     			Image10->Canvas->MoveTo(ClockCenter.x,ClockCenter.y);
     			Image10->Canvas->LineTo(CoordHour.x,CoordHour.y);
     		}
     		Image10->Canvas->Pen->Width = 2;
     	}
     }
     else
     {
		Digitalformat->Text = TimeToStr(dtNow);
     }
}
//------------------------------------------------------------------------------
void __fastcall TMCFAlgorithmForm::DrawClock(int Segments,int SegmentSize,int IncrementLength)
{
     TPoint IncrementA,IncrementB;
     int Angle;

     Image10->Canvas->Pen->Width = 2;
     Image10->Canvas->Pen->Mode =  pmBlack;
     Image10->Canvas->Brush->Style = bsClear;
     Image10->Canvas->Ellipse(ClockCenter.x-ClockBoarder,ClockCenter.y-ClockBoarder,ClockCenter.x+ClockBoarder,ClockCenter.y+ClockBoarder);
     Image10->Canvas->Pen->Width = 2;
     for(int Cycle=0; Cycle < Segments; Cycle++)
     {
     	Angle = (Cycle * SegmentSize) + SegmentSize;
     	IncrementA.x = ClockCenter.x + ((ClockBoarder-5) * sin(Angle * DegToRad));
     	IncrementA.y = ClockCenter.y - ((ClockBoarder-5) * cos(Angle * DegToRad));
     	IncrementB.x = IncrementA.x + (IncrementLength * sin(Angle * DegToRad));
     	IncrementB.y = IncrementA.y - (IncrementLength * cos(Angle * DegToRad));
     	Image10->Canvas->MoveTo(IncrementA.x,IncrementA.y);
     	Image10->Canvas->LineTo(IncrementB.x,IncrementB.y);
     	if (Segments==12  || Segments == 4)
     	{
     		IncrementB.x = (IncrementA.x + (4*IncrementLength * sin(Angle * DegToRad))-4);
     		IncrementB.y = (IncrementA.y - (4*IncrementLength * cos(Angle * DegToRad))-4);
     		Image10->Canvas->TextOut(IncrementB.x,IncrementB.y,IntToStr(Angle/30));
     	}
     }
     Image10->Canvas->Pen->Mode = pmNotXor;
}
//------------------------------------------------------------------------------

void __fastcall TMCFAlgorithmForm::DisplaybuttonClick(TObject *Sender)
{
     SwitchMode();
}
//------------------------------------------------------------------------------


void __fastcall TMCFAlgorithmForm::AlarmbuttonClick(TObject *Sender)
{
     SetAlarm(StrToInt(Hourbox->Text),StrToInt(Minutebox->Text),StrToInt(Secondbox->Text),0);
     Hourbox->Visible =!Hourbox->Visible;
     Minutebox->Visible =!Minutebox->Visible;
     Image9->Visible =!Image9->Visible;
     Image11->Visible =!Image11->Visible;
     Image12->Visible =!Image12->Visible;
}
//------------------------------------------------------------------------------
void __fastcall  TMCFAlgorithmForm::SetAlarm(int AHour,int AMin,int ASec,int AMSec)
{
		  try
		  {
			  dtAlarm = EncodeTime((Word)AHour,(Word)AMin,(Word)ASec,(Word)0);
			  DecodeTime(dtAlarm, AHour, AMin, ASec, AMSec);
			  Label30->Caption = "Alarm Set For " + IntToStr(AHour) +":" + IntToStr(AMin);
			  AlarmSet = true;
		  }
		  catch(...)
		  {
			  Application->MessageBox("Invalid time entered.", "Invalid Time", MB_OK);
		  }
}
//------------------------------------------------------------------------------
void __fastcall TMCFAlgorithmForm::GetAlarm()
{
        if (AlarmSet)
        {
                Word AHour, AMin, ASec, AMSec;
                DecodeTime(dtAlarm, AHour, AMin, ASec, AMSec);
        }
}
//------------------------------------------------------------------------------
void __fastcall TMCFAlgorithmForm::DisplayAlarm()
{
        if (AlarmSet)
        {
                AlarmSet = false;
	        Label30->Caption = "";
	        Application->MessageBox("ALARM!!! Alarm was set to go off at this time", "Alarm", MB_OK);
        }
}//-----------------------------------------------------------------------------
void __fastcall TMCFAlgorithmForm::SwitchMode()
{
	// int Extra (10);
	Image10->Canvas->Pen->Mode = pmCopy;
	//Canvas->Pen->Color = clBtnFace;
	Image10->Canvas->Pen->Color = clNone;
	Image10->Canvas->Brush->Style = bsSolid;
	Image10->Canvas->Brush->Color = clBtnFace;
	Image10->Canvas->Brush->Color = clNone;
	Digitalformat->Visible = !Digitalformat->Visible;
	DrawHand = SecondHand;
	dtCurrent = EncodeTime((Word)0, (Word)0, (Word)0, (Word)0);
	AnalogMode = !AnalogMode;
}
//------------------------------------------------------------------------------
void __fastcall TMCFAlgorithmForm::SetAlarm1Click(TObject *Sender)
{
	Hourbox->Visible =!Hourbox->Visible;
	Minutebox->Visible =!Minutebox->Visible;
	Image9->Visible =!Image9->Visible;
	Image11->Visible =!Image11->Visible;
	Image12->Visible =!Image12->Visible;
}
//------------------------------------------------------------------------------
int  __fastcall  FindIdleCrane()
{
       int Number_Of_Cranes       = StrToInt(MCFAlgorithmForm->Edit12->Text);
       int i2;
       for(i2=1;i2<=Number_Of_Cranes;i2++)
           if (Crane_Buff[i2-1].Number_Of_Jobs == 0) return(i2);
       if(i2 > Number_Of_Cranes ) return(0);
}
//------------------------------------------------------------------------------
void __fastcall TMCFAlgorithmForm::Timer3Timer(TObject *Sender)
{
      static TDateTime PreviousDateTime;
      TDateTime DateTime = Time();  // store the current date and time
      AnsiString TimeStr = TimeToStr(DateTime); // convert the time to a string
      Label26->Caption = "Current Time: " + TimeStr;  // display the time on the form's caption

      TimeValueInSecond += 1;

      MCFAlgorithmForm->Label32->Caption = "Simulation Time (Second): " + IntToStr(TimeValueInSecond);

      AnsiString PortNameStr = MCFAlgorithmForm->ListBox3->Items->Strings[MCFAlgorithmForm->ListBox3->ItemIndex];

      int Current_AGV = PortAGVForm->Table2->RecNo  ;

      int Number_Of_AGVs = PortAGVForm->Table2->RecordCount;
      PortAGVForm->Table2->First() ;
      for(int m=1;m<=Number_Of_AGVs;m++)
      {
           MCFAlgorithmForm->Table4->First() ;
           Set_Location_And_ReadyTime_AGV(PortNameStr,m,TimeValueInSecond,StartOfDynamicSchedule);
           PortAGVForm->Table2->Next() ;
      }

      Delete_Processed_Container_Jobs(TimeValueInSecond,StartOfDynamicSchedule);

      int Crane_Number = FindIdleCrane();
      if(Crane_Number)
      {
         Timer2Timer(Sender,Crane_Number); // Generate Container
         Set_Table4_For_New_Schedule(PortNameStr);
         Timer1Timer(Sender); // Making the model, solving it and Generating new schedule:
      }

      PortAGVForm->Table2->RecNo = Current_AGV;
      if(PreviousDateTime != DateTime)
      {
         Painta();
         PreviousDateTime = DateTime;
      }
}
//------------------------------------------------------------------------------


void __fastcall TMCFAlgorithmForm::DBGrid4CellClick(TColumn *Column)
{
   if(PortAGVForm->Table2->RecNo > 0 )
   {
     AnsiString AGVName =  PortAGVForm->Table2->FieldValues["AGVName"];
     Label31->Caption = "Container Jobs for " + AGVName;
     Label31->Refresh() ;
   }

}
//------------------------------------------------------------------------------


void __fastcall TMCFAlgorithmForm::Button10Click(TObject *Sender)
{
    static int SwapPanel = 0;
    int w1, w2, l1, l2;
    SwapPanel = SwapPanel ^ 1;

    switch(TabControl1->TabIndex)
    {
       case 0 :  // Static
                w1 = GroupBox3->Width ;
                l1 = GroupBox3->Left ;
                break;
       case 1 : // Model
                w1 = GroupBox10->Width ;
                l1 = GroupBox10->Left ;
                break;
       case 2 : // Dynamic
                w1 = GroupBox7->Width ;
                l1 = GroupBox7->Left ;
                break;
       case 3 : // Dynamic
                w1 = GroupBox18->Width ;
                l1 = GroupBox18->Left ;
                break;
       case 4 : // Result
                w1 = GroupBox19->Width ;
                l1 = GroupBox19->Left ;
                break;
    }


    w2 = GroupBox14->Width ;
    l2 = GroupBox14->Left ;

    GroupBox3->Width = w2;
    GroupBox3->Left = l2;
    GroupBox10->Width = w2;
    GroupBox10->Left = l2;
    GroupBox7->Width = w2;
    GroupBox7->Left = l2;
    GroupBox18->Width = w2;
    GroupBox18->Left = l2;
    GroupBox19->Width = w2;
    GroupBox19->Left = l2;


    GroupBox14->Width = w1;
    GroupBox14->Left = l1;
    switch(SwapPanel)
    {
       case 0 :    DBGrid4->Width = 305;
                   DBGrid3->Width = 305;
                   ListBox2->Width = 551;
                   ListBox5->Width = 551;
                   break;
       case 1 :    DBGrid4->Width = 605;
                   DBGrid3->Width = 605;
                   ListBox2->Width = 305;
                   ListBox5->Width = 305;
                   break;
    }
}
//------------------------------------------------------------------------------


void __fastcall TMCFAlgorithmForm::DBGrid5TitleClick(TColumn *Column)
{
      int        FieldNo  = DBGrid5->SelectedIndex ;
      switch(FieldNo)
      {
         case 0:// ContainerId
                Table1->IndexFieldNames = "PortName;AGVNumber;ContainerId";
                break;
         case 4://  AgvName
                Table1->IndexFieldNames = "PortName;AGVNumber";
                break;
         case 6:// ActualTime
                Table1->IndexFieldNames = "PortName;AGVNumber;ActualTime";
                break;
      }
}

//------------------------------------------------------------------------------
void __fastcall Update_Num_Of_Arcs_To_File(int Number_Of_Nodes,int Number_Of_ARCs)
{
     FILE *fp = fopen("temp.inp","r+"); // outstream  string

     AnsiString Str1 = "p min " ;  // define Min, number of nodes and arcs
     Str1 = Str1 + IntToStr(Number_Of_Nodes)+ "     "  + IntToStr(Number_Of_ARCs) ;
     fprintf(fp,"%s     ", Str1.c_str());
     fclose(fp);
}
//------------------------------------------------------------------------------

void __fastcall TMCFAlgorithmForm::Button11Click(TObject *Sender)
{   // Generate Jobs and Schedule (Static fashion )
       int i,i1,i2,r;
       AnsiString BlockStrS, BlockStrD;

       TLocateOptions Opts;
       AnsiString DistanceStr;
       Opts.Clear();
       Variant locvalues[3];


       if(ListBox1->ItemIndex < 0) return;

       static Next_Quay_Crane = 1;
       if (StartOfDynamicSchedule) Next_Quay_Crane = 1;

       long int BaseTime = TimeValueInSecond + GlobalInitialTimeQuayCrane;

       int Quay_Crane_Time_Window = StrToInt(MCFAlgorithmForm->Edit26->Text);
       int Number_Of_Cranes       = StrToInt(MCFAlgorithmForm->Edit6->Text);
       int Number_Of_Vehicles     = StrToInt(MCFAlgorithmForm->Edit4->Text);

       for(int c=1;c<=Number_Of_Cranes;c++)
       {
           Crane_Buff[c-1].Number_Of_Done_Jobs = 0;
           Crane_Buff[c-1].Next_Generation_Time= StrToInt(Edit23->Text);
           Crane_Buff[c-1].Last_Completed_Time = StrToInt(Edit23->Text);
       }

       for(int v=1;v<=Number_Of_Vehicles;v++)
       {
           Vehicle_Buff[v-1].Number_Of_Jobs      = 0;
           Vehicle_Buff[v-1].Number_Of_Done_Jobs = 0;

           Vehicle_Buff[v-1].Last_Completed_Time = 0;
       }

       locvalues[0] = ListBox1->Items->Strings[ListBox1->ItemIndex];

       bool b = PortContainerForm->Table1->Locate("Portname", VarArrayOf(locvalues, 0), Opts);

       if( b == false)
         for(i=1; i<=StrToInt(Edit1->Text);i++)
         {
          ProgressBar1->Position = 100*i/StrToInt(Edit1->Text);
          r = 3 + random(2); // 4 if we like four cases
               // 3 because we like to have only Yard to berth and vice versa
          switch(r)
          {
             case 3: // Yard to Berth
                i1 = 1+ random(StrToInt(MCFAlgorithmForm->Edit3->Text));
                if (MCFAlgorithmForm->RadioButton8->Checked == true )
                    i2 = 1;
                else if (MCFAlgorithmForm->RadioButton9->Checked == true )
                        i2 = 1+ random(StrToInt(MCFAlgorithmForm->Edit12->Text));
                     else
                     {
                        i2 = Next_Quay_Crane;
                        if(++Next_Quay_Crane > Number_Of_Cranes ) Next_Quay_Crane = 1;
                     }
                BlockStrS = "Block "; BlockStrS = BlockStrS + i1;
                BlockStrD = "W/P "; BlockStrD = BlockStrD + i2;
                break;
             case 4: // Berth to Yard
                if (MCFAlgorithmForm->RadioButton8->Checked == true )
                    i2 = 1;
                else if (MCFAlgorithmForm->RadioButton9->Checked == true )
                        i2 = 1+ random(StrToInt(MCFAlgorithmForm->Edit12->Text));
                     else
                     {
                        i2 = Next_Quay_Crane;
                        if(++Next_Quay_Crane > Number_Of_Cranes ) Next_Quay_Crane = 1;
                     }
                i1 = 1+ random(StrToInt(MCFAlgorithmForm->Edit3->Text));
                BlockStrS = "W/P "; BlockStrS = BlockStrS + i2;
                BlockStrD = "Block "; BlockStrD = BlockStrD + i1;
                break;
          }
          PortContainerForm->Table1->Append();

          AnsiString ContainerIDStrS = "C-";
          ContainerIDStrS = ContainerIDStrS + IntToStr(i);

          PortContainerForm->Table1->FieldValues["ContainerID"]= ContainerIDStrS;
          PortContainerForm->Table1->FieldValues["Portname"]   = ListBox1->Items->Strings[ListBox1->ItemIndex];

          PortContainerForm->Table1->FieldValues["SourcePoint"]= BlockStrS;
          PortContainerForm->Table1->FieldValues["DestPoint"]  = BlockStrD;

          int ReadyTime = Quay_Crane_Time_Window * Crane_Buff[i2-1].Number_Of_Jobs
                        + Crane_Buff[i2-1].Last_Completed_Time;
          Crane_Buff[i2-1].Number_Of_Jobs ++;

          PortContainerForm->Table1->FieldValues["ReadyTime"]     = ReadyTime;
          PortContainerForm->Table1->FieldValues["QuayCraneTime"] = ReadyTime;

          PortContainerForm->Table1->Post();
        }

     // Generate schedule
     PortContainerForm->Table2->Refresh() ;

     int   Number_Of_AGVs, Number_Of_Container_Jobs, Number_Of_Nodes, Number_Of_ARCs=0;

     int   ReadyTimeAGV;
     int   Tij, Number_ARCs;

     int   *TimeAGVToC,*TimeCToC;
     int   *CostAGVToC,*CostCToC;

     AnsiString PortNameStr, Str1 , Str2, SinkNodeStr;

     Opts.Clear();

     PortNameStr = ListBox1->Items->Strings[ListBox1->ItemIndex];

     Number_Of_AGVs = StrToInt(Edit4->Text) ;

                                                 // Dynamic Flag
     Number_Of_Container_Jobs =  Read_Container_Jobs(0 );
     if (Number_Of_Container_Jobs > 0)
         Edit1->Text  = IntToStr(Number_Of_Container_Jobs);

     // Just for test
     Number_ARCs  =  ( Number_Of_AGVs * Number_Of_Container_Jobs)
                  +  ( Number_Of_Container_Jobs * 2)
                  +    Number_Of_AGVs
                  +  Number_Of_Container_Jobs * (Number_Of_Container_Jobs-1);

     if(Number_Of_Container_Jobs*Number_Of_AGVs == 0 ) return;

     Str1 = "c  Port Name : " ; Str1 = Str1 + ListBox1->Items->Strings[ListBox1->ItemIndex];
     ListBox2->Items->Add(Str1);

     Str1 = "c  Number of Container Jobs : " ; Str1 = Str1 + IntToStr(Number_Of_Container_Jobs);
     ListBox2->Items->Add(Str1);

     Str1 = "c  Number of AGVs           : " ; Str1 = Str1 + IntToStr(Number_Of_AGVs);
     ListBox2->Items->Add(Str1);

     Number_Of_Nodes = Number_Of_AGVs + Number_Of_Container_Jobs*2 + 1;

     TimeAGVToC    = (int *) malloc(sizeof(int)*Number_Of_AGVs*Number_Of_Container_Jobs);
     TimeCToC      = (int *) malloc(sizeof(int)*Number_Of_Container_Jobs*Number_Of_Container_Jobs);

     CostAGVToC = (int *) malloc(sizeof(int)*Number_Of_AGVs*Number_Of_Container_Jobs);
     CostCToC   = (int *) malloc(sizeof(int)*Number_Of_Container_Jobs*Number_Of_Container_Jobs);

     if(!CostCToC || !CostAGVToC || !TimeAGVToC || !TimeCToC )
     {
         Application->MessageBox("Not Enough Memory !! ", "Error", MB_OK);
         return;
     }

     TCursor Save_Cursor = Screen->Cursor;
     Screen->Cursor = crHourGlass;    // Show hourglass cursor

     Label25->Color = clBlue;
     Label25->Caption = "Making the Model...";// Static Fashion
     GroupBox15->Refresh();


     FILE *fp = fopen("temp.inp","w+b"); // outstream  string

     Str1 = "p min " ;  // define Min, number of nodes and arcs
     Str1 = Str1 + IntToStr(Number_Of_Nodes)+ "     "  + IntToStr(Number_ARCs) ;
     fprintf(fp,"%s                       \n", Str1.c_str());
     SinkNodeStr = IntToStr(Number_Of_Nodes);

     Append_Supply_Nodes(Number_Of_AGVs,fp); //  Number_Of_ARCs += Number_Of_AGVs;

     Append_Demand_Node (Number_Of_AGVs,Number_Of_Nodes,fp); // Number_Of_ARCs++;

     Append_Arcs_From_AGVs_Nodes_To_Container_Nodes(Number_Of_AGVs,&Number_Of_ARCs,              //SF
                                    PortNameStr,TimeAGVToC,Number_Of_Container_Jobs,CostAGVToC,fp,1);

     Append_Arcs_From_AGV_Nodes_To_Sink_Node(Number_Of_AGVs,&Number_Of_ARCs,SinkNodeStr,fp);

     Append_Arcs_From_CNode_To_CPNodes(Number_Of_AGVs,Number_Of_Nodes,&Number_Of_ARCs,fp);

     Append_Arcs_From_CPNodes_To_Sink_Node(Number_Of_AGVs,Number_Of_Nodes,SinkNodeStr,&Number_Of_ARCs,fp);

     Append_Arcs_From_CPNodes_To_Any_Other_CNodes(Number_Of_AGVs,Number_Of_Container_Jobs,
                                                 &Number_Of_ARCs,TimeCToC,CostCToC,PortNameStr,fp,1);
                                                                                             // SF

     fclose(fp);

     Update_Num_Of_Arcs_To_File(Number_Of_Nodes,Number_Of_ARCs);

     if( MCFAlgorithmForm->RadioButton7->Checked)
         Write_Model_To_ListBox2();

     Label25->Color = clMaroon ;
     Label25->Caption = "Solving the Model...";
     GroupBox15->Refresh();

     int stat = MCF_Solve( 2,"temp.inp","temp.out"); // Static Fashion
     switch(stat)
     {
        case -1 :
           Application->MessageBox("This model can not be solved by MCF Algorithms", "Error", MB_OK);
           Label25->Caption = "Not Enough Memory !! ";
           Label25->Color = clRed;
           break;
        case  1 :
           Application->MessageBox("This model can not be solved by MCF Algorithms", "Error", MB_OK);
           Label25->Caption = " Inconsistent Model!! ";
           Label25->Color = clRed;
           break;
        case 0 :
           Label25->Color = clGreen ;
           Label25->Caption = "Solution is Ready";

           if( MCFAlgorithmForm->RadioButton5->Checked)
              Write_Solution_To_ListBox5();

           Reset_Number_Of_Jobs_Before_Solution(Number_Of_Cranes,Number_Of_AGVs); // Static fashion
           Read_Solution("temp.out",Number_ARCs, Number_Of_AGVs, Number_Of_Nodes,
                   Sender,1,TimeAGVToC,TimeCToC,Number_Of_Container_Jobs,CostAGVToC,CostCToC);
     }
     Screen->Cursor = Save_Cursor; // always restore the cursor

     free(TimeAGVToC);
     free(TimeCToC);
     free(CostAGVToC);
     free(CostCToC);
}
//------------------------------------------------------------------------------

void __fastcall TMCFAlgorithmForm::FormClose(TObject *Sender,
      TCloseAction &Action)
{
     DataSource3->Enabled = false;
     PortAGVForm->DataSource1->Enabled = false;
     DataSource1->Enabled = false;


     PortLayoutForm->DataSource1->Enabled = false;
     PortContainerForm->DataSource1->Enabled = false;
}
//------------------------------------------------------------------------------


void __fastcall TMCFAlgorithmForm::DBChart1CanResize(TObject *Sender,
      int &NewWidth, int &NewHeight, bool &Resize)
{
     Refresh();
}
//------------------------------------------------------------------------------


