//-----------------------------------------------------------------------------
#include<iostream>
#include<iomanip>

#pragma hdrstop

#include "Main1_0_5.h"
#include "MCFModel1_3.h"
#include "OpenPort.h"
#include "PortLayout.h"
#include "PortContainer.h"
#include "PortAGV.h"
#include "stdio.h"
#include "string.h"
#include "Unit1.h"
#include "mcf.h"

#include "Dijkstra.h"
#include "PortBenchmark.h"
#include <iostream.h>
#include <fstream.h>
#include <math.h>
#include <mem.h>

#include <iomanip.h>
#include "HCDVRP.cpp"

// #define TSP-PORT
#define  MCF_DEBUG_4

bool __fastcall Read_Port(String Port);
void __fastcall Read_Route(String PortName);
int  __fastcall Get_Junction(AnsiString PortName,AnsiString StrPoint);
int  __fastcall Shortest_Path_Dijkstra(AnsiString PortName,int s,int d,int m);
long MCF_write_intermediate2(MCF_network_p net);

void Main_Dijkstra();

//------------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

TMCFAlgorithmForm *MCFAlgorithmForm;


extern  int  MCF_NSA_Solve( int i,char *InFile, char *OutFile,MCF_network_p net,
                            int StaticFlag, int Number_Of_AGVs,int balance );

static long int TimeValueInSecond = 0;


long int GlobalInitialTimeQuayCrane;
bool     DynamicSchedulingStarted=false;
bool     AllocateMemoryForNSA    = false;

int     *TimeAGVToC,*TimeCToC;
int     *CostAGVToC,*CostCToC;
int     *Route_Table;


MCF_network_t  net;

//------------------------------------------------------------------------------
__fastcall TMCFAlgorithmForm::TMCFAlgorithmForm(TComponent* Owner)
        : TForm(Owner)
{
	ArmSec = Digitalformat->Width / 5;
	MinArm=(4 * ArmSec) / 5;
	HourArm=(2* ArmSec) / 5;

	ClockCenter.x= Digitalformat->Width  /2 ;
	ClockCenter.y= Digitalformat->Width  /2 ;
	CoordSec.x   = ClockCenter.x;
	CoordSec.y   = ClockCenter.y;
	CoordMin.x   = ClockCenter.x;
	CoordMin.y   = ClockCenter.y;
	CoordHour.x  = ClockCenter.x;
	CoordHour.y  = ClockCenter.y;

	DegToRad = (3.1416/180);
	ClockBoarder= ArmSec + 3;
	AnalogMode  = true;
	AlarmSet    = false;
	FullClock   = true;
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

  //      if(SourcePoint.IsEmpty() || DestPoint.IsEmpty())
  //      if(VarIsEmpty(locvalues[1]) || VarIsEmpty(locvalues[2]))
        if(SourcePoint.c_str() == "" || DestPoint.c_str()  == "")
            return(0);

        if(SourcePoint == DestPoint) return(0);
        try
        {
          bool b = PortLayoutForm->Table1->Locate("Portname;Sourcepoint;DestPoint", VarArrayOf(locvalues, 2), Opts);
          if (b)
          {
             DistanceStr = PortLayoutForm->Table1->FieldByName("Distance")->AsString;
             return(StrToInt(DistanceStr));
          } else
          return(0);
        }
        catch(...)        {
          Application->MessageBox("Invalid Points !! ", "Error", MB_OK);
          return(0);
        }

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
void __fastcall TMCFAlgorithmForm::Initialize_New_Schedule(AnsiString PortNameStr)
{
      TLocateOptions Opts;
      Set  <TLocateOption,0,1> flags;
      flags << loCaseInsensitive << loPartialKey;

      int  Quay_Crane_Time_Window = StrToInt(MCFAlgorithmForm->Edit18->Text);
      int  Time_Window_Vehicle    = StrToInt(MCFAlgorithmForm->Edit30->Text);

      Variant locvalues[2];

      int Number_Of_AGVs = PortAGVForm->Table2->RecordCount;
      PortAGVForm->Table2->First() ;
      for(int VN=1;VN<=Number_Of_AGVs;VN++)
      {
           int NumberOfJobs = 0;

           String     AGVLocation    = PortAGVForm->Table2->FieldValues["StartLocation"];
           String     PreAGVLocation = PortAGVForm->Table2->FieldValues["PreviousLocation"];

           TBookmark Bookmark2 = PortAGVForm->Table2->GetBookmark ()  ;

           PortAGVForm->Table2->Edit() ;
           if( MCFAlgorithmForm->Table4->RecordCount > 0)
           {
               MCFAlgorithmForm->Table4->First(); // Get the first row of Vehicle-List
               String     ContainerID = MCFAlgorithmForm->Table4->FieldByName("ContainerID")->AsString;
               String     PortName    = MCFAlgorithmForm->Table4->FieldValues["Portname"];
               String     AGVNameStrS = MCFAlgorithmForm->Table4->FieldValues["AGVName"];
               int        ReadyTime   = MCFAlgorithmForm->Table4->FieldValues["ContainerReadyTime"];
               int        VehicleTime = MCFAlgorithmForm->Table4->FieldValues["VehicleTime"];
               int        ActualTime  = MCFAlgorithmForm->Table4->FieldValues["ActualTime"];
               int        QCraneTime  = MCFAlgorithmForm->Table4->FieldValues["QuayCraneTime"];
               String     SourcePoint = MCFAlgorithmForm->Table4->FieldByName("SourcePoint")->AsString;
               String     DestinPoint = MCFAlgorithmForm->Table4->FieldByName("DestPoint")->AsString;

               while(MCFAlgorithmForm->Table4->RecordCount > 0) // Delete others
               {
                   MCFAlgorithmForm->Table4->First();
                   Vehicle_Buff[VN-1].Number_Of_Jobs--; // Initilize_new_schedule
                   MCFAlgorithmForm->Table4->Delete();
               }
               // if the destination of first job in the vehicel list is Quay side, it should be held

            // if(DestinPoint.SubString(1,3) == "W/P" &&
            //    SourcePoint == PreAGVLocation &&  AGVLocation == DestinPoint && DynamicSchedulingStarted )

               if(DestinPoint.SubString(1,3) == "W/P" &&
                  AGVLocation == DestinPoint && DynamicSchedulingStarted )
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
                  MCFAlgorithmForm->Table4->FieldValues["DestPoint"]             = DestinPoint;
                  MCFAlgorithmForm->Table4->FieldValues["QuayCraneTime"]         = QCraneTime;
                  MCFAlgorithmForm->Table4->Post ();
                  MCFAlgorithmForm->Table4->AutoCalcFields = true;

                  int CN;
                  if (SourcePoint.SubString(1,3)== "W/P")
                      CN = StrToInt(SourcePoint.SubString(5,SourcePoint.Length()-4 ));
                  else if (DestinPoint.SubString(1,3)== "W/P")
                      CN = StrToInt(DestinPoint.SubString(5,DestinPoint.Length()-4 ));

                  Crane_Buff[CN-1].Last_Completed_Temp = ActualTime + Quay_Crane_Time_Window ;// Initialize_New_Schedule

                  NumberOfJobs = 1;

                  Vehicle_Buff[VN-1].Number_Of_Jobs = NumberOfJobs;
                  Vehicle_Buff[VN-1].Last_Completed_Temp = ActualTime + Time_Window_Vehicle;
                                                         // Initialize_New_Schedule
                  locvalues[0] = PortName;
                  locvalues[1] = ContainerID;
                  Opts << loCaseInsensitive ;

                  bool b = PortContainerForm->Table1->Locate("Portname;ContainerID", VarArrayOf(locvalues, 1) , Opts);
                  if  (b)  PortContainerForm->Table1->Delete();

               }
           }
           MCFAlgorithmForm->Table4->Refresh();
           MCFAlgorithmForm->DBChart1->Refresh() ;
           MCFAlgorithmForm->DBGrid3->Refresh ();

           PortAGVForm->Table2->FieldValues["NumberOfContainers"] =  NumberOfJobs;
           PortAGVForm->Table2->Post() ;

           PortAGVForm->Table2->GotoBookmark(Bookmark2);

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
void  __fastcall  TMCFAlgorithmForm::Update_Cranes_And_Vehicles()
{
      int W1=0, W2=0;
      if (DynamicSchedulingStarted)
      {
          W1  = StrToInt(MCFAlgorithmForm->Edit30->Text);
          W2  = StrToInt(MCFAlgorithmForm->Edit18->Text);
      }

      for(int CN=1;CN<=Port_Buff.NumberOfWorkingPosition ;CN++)
      {
          Crane_Buff[CN-1].Last_Completed_Temp = Crane_Buff[CN-1].Last_Completed_Time + W2;
      }

      for(int VN=1;VN<=Port_Buff.NumberOfAGVs;VN++)
           Vehicle_Buff[VN-1].Last_Completed_Temp = Vehicle_Buff[VN-1].Last_Completed_Time + W1;
}
//------------------------------------------------------------------------------
void __fastcall  Read_NSA_Solution(char *FileName   ,int Num_Arcs    , int M_AGVs,
                                   int   N_Nodes    ,int staticflag,
                                   int Number_Of_Container_Jobs                      )
{
     TLocateOptions Opts;
     Variant locvalues[3];

     int   Time_Window_Vehicle;
     AnsiString buff2, b3 , b4, PortName;
     int   ActualTime,AGVConRec;
     int   VehicleTime;
     char  buff[500];

     MCF_arc_p arc;

     int   Row=0, Num_Iteration, CPU_Time;
     unsigned long  Objective_Function;

     int   Quay_Crane_Time_Window,  CN , Number_Of_Cranes;

     int Number_Of_Jobs ; // For Container Jobs
     int Number_Of_Remaining_Jobs;

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


     CPU_Time = net.cpu_time ;
     Objective_Function = net.optcost ;
     Num_Iteration = net.iterations;

     MCFAlgorithmForm->Edit13->Text = IntToStr(CPU_Time);
     MCFAlgorithmForm->Edit16->Text = IntToStr(Num_Iteration);
     MCFAlgorithmForm->Edit17->Text = IntToStr(Objective_Function);

     if(staticflag)
     {
       PortName = MCFAlgorithmForm->ListBox1->Items->Strings[MCFAlgorithmForm->ListBox1->ItemIndex];
       Time_Window_Vehicle = StrToInt(MCFAlgorithmForm->Edit5->Text);
     }
     else
     {
       PortName = MCFAlgorithmForm->ListBox3->Items->Strings[MCFAlgorithmForm->ListBox3->ItemIndex];
       Time_Window_Vehicle = StrToInt(MCFAlgorithmForm->Edit30->Text);
     }

     PortAGVForm->Table2->Filter =   AnsiString("Portname = '") + PortName + "'";
     PortAGVForm->Table2->Filtered = true;

     Set  <TLocateOption,0,1> flags;
     flags << loCaseInsensitive << loPartialKey;

     PortContainerForm->Table2->Filter =   AnsiString("Portname = '") + PortName + "'";
     PortContainerForm->Table2->Filtered = true;

     Opts.Clear();
     int AGVn = 1;
     AnsiString AGVNameStrS = "AGV-" + IntToStr(AGVn);
     locvalues[0] = PortName   ;
     locvalues[1] = AGVNameStrS;

     bool b ;
     if(staticflag)
     {
        PortAGVForm->Table2->First() ;
        for(int i=1;i<=M_AGVs;i++)
        {
            while(MCFAlgorithmForm->Table4->RecordCount > 0)
            {
                MCFAlgorithmForm->Table4->First();
                MCFAlgorithmForm->Table4->Delete();
            }
            PortAGVForm->Table2->Next() ;
        }
     }

     for( arc = net.arcs; arc != net.stop_arcs; arc++ )
     {
        if(arc->ident == MCF_DELETED) continue; // DNSA
        if( MCF_ABS(arc->flow) > (MCF_flow_t)MCF_ZERO_EPS )
           *(Route_Table + arc->tail->number) = arc->head->number;
     }
     // This Loop set vehicle in Container Buff
     for(int VN=1;VN<=M_AGVs;VN++)
     {
        Vehicle_Buff[VN-1].No_Job_Temp = 0;
        int next = *(Route_Table + VN);
        while(next != N_Nodes)
        {
          if(!next ) break; // DNSA
          int c_node = (next - M_AGVs)/2 + 1;

          next = *(Route_Table + next);
          if( next == N_Nodes) break;
          if( (next - M_AGVs) % 2 == 0 )
              next = *(Route_Table+next);
          Container_Buff[c_node-1].Vehicle = VN;
        }
     }
     // This loop will process container jobs as their Times order and write to Table5
     for(int JobNo=1;JobNo<=Number_Of_Container_Jobs;JobNo++)
     {
         int VN = Container_Buff[JobNo-1].Vehicle;
         if( VN == 0) continue;
         // if(Container_Buff[JobNo-1].Empty == 1) continue;

         AnsiString AGVNameStrS = "AGV-"; AGVNameStrS = AGVNameStrS + StrToInt(VN);

         String SourcePointStr = Container_Buff[JobNo-1].SourcePointStr;
         String DestinPointStr = Container_Buff[JobNo-1].DestPointStr  ;
         if (SourcePointStr.SubString(1,3)== "W/P")
             CN = StrToInt(SourcePointStr.SubString(5,SourcePointStr.Length()-4 ));
         else if (DestinPointStr.SubString(1,3)== "W/P")
             CN = StrToInt(DestinPointStr.SubString(5,DestinPointStr.Length()-4 ));

         Vehicle_Buff[VN-1].Number_Of_Jobs++;

         if(Vehicle_Buff[VN-1].No_Job_Temp == 0)
         {
            VehicleTime  = 0;
            VehicleTime += *(TimeAGVToC + (VN-1)*Number_Of_Container_Jobs  + (JobNo - 1 ));
            Vehicle_Buff[VN-1].Cost_Temp = *(CostAGVToC + (VN-1)*Number_Of_Container_Jobs  + (JobNo - 1 ));
         }else
         {
            VehicleTime = Vehicle_Buff[VN-1].Last_Completed_Temp ;
            VehicleTime += * (TimeCToC + (Vehicle_Buff[VN-1].PreJob_Temp-1)*Number_Of_Container_Jobs + JobNo -1);
            Vehicle_Buff[VN-1].Cost_Temp += * (CostCToC + (Vehicle_Buff[VN-1].PreJob_Temp-1)*Number_Of_Container_Jobs + JobNo-1);
         }
         Vehicle_Buff[VN-1].PreJob_Temp = JobNo;
         Vehicle_Buff[VN-1].No_Job_Temp++;

         int QuayCraneTime, ActualTime;

         QuayCraneTime = Crane_Buff[CN-1].Last_Completed_Temp ;
         ActualTime    = max (QuayCraneTime , VehicleTime );

         Crane_Buff[CN-1].Last_Completed_Temp = ActualTime + Quay_Crane_Time_Window;
           // NSA
         Vehicle_Buff[VN-1].Last_Completed_Temp = ActualTime + Time_Window_Vehicle;

         MCFAlgorithmForm->Table5->Append();
         MCFAlgorithmForm->Table5->FieldValues["Portname"]= PortName;
         MCFAlgorithmForm->Table5->FieldValues["AGVName"] = AGVNameStrS;
         MCFAlgorithmForm->Table5->FieldValues["AGVNumber"]= VN;
         MCFAlgorithmForm->Table5->FieldValues["ContainerID"]  = Container_Buff[JobNo-1].IDStr;
         MCFAlgorithmForm->Table5->FieldValues["JobNo"]  = JobNo ;
         MCFAlgorithmForm->Table5->FieldValues["ContainerReadyTime"]= Container_Buff[JobNo-1].ReadyTime;
         MCFAlgorithmForm->Table5->FieldValues["SourcePoint"]       = Container_Buff[JobNo-1].SourcePointStr;
         MCFAlgorithmForm->Table5->FieldValues["DestPoint"]         = Container_Buff[JobNo-1].DestPointStr;
         MCFAlgorithmForm->Table5->FieldValues["VehicleTime"]= VehicleTime;
         MCFAlgorithmForm->Table5->FieldValues["QuayCraneTime"]= QuayCraneTime;
         MCFAlgorithmForm->Table5->FieldValues["ActualTime"]= ActualTime;
         MCFAlgorithmForm->Table5->Post();

     }

     // This Loop updates number and cost of Jobs for each vehicle
     PortAGVForm->Table2->First() ;
     for(int VN=1;VN<=M_AGVs;VN++)
     {
        int NumContainers = PortAGVForm->Table2->FieldValues["NumberOfContainers"] ;

        PortAGVForm->Table2->Edit();
        if(staticflag)
        {
             if(Vehicle_Buff[VN-1].No_Job_Temp)
                PortAGVForm->Table2->FieldValues["CostAGV"] = Vehicle_Buff[VN-1].Cost_Temp;
             else
                PortAGVForm->Table2->FieldValues["CostAGV"] = 0; // 25/05/2004
             Number_Of_Remaining_Jobs -= Vehicle_Buff[VN-1].No_Job_Temp;
             MCFAlgorithmForm->Edit21->Text = Number_Of_Remaining_Jobs;
        }
        PortAGVForm->Table2->FieldValues["NumberOfContainers"] = Vehicle_Buff[VN-1].No_Job_Temp + NumContainers;
        PortAGVForm->Table2->Post();
        PortAGVForm->Table2->Next();
     }

     MCFAlgorithmForm->Table5->Refresh() ;
     MCFAlgorithmForm->Table4->Refresh() ;
     MCFAlgorithmForm->DBChart1->Refresh() ;
     MCFAlgorithmForm->DBGrid3->Refresh ();

     PortAGVForm->Table2->Refresh() ;
     MCFAlgorithmForm->DBGrid4->Refresh() ;
     MCFAlgorithmForm->GroupBox14->Refresh() ;

}
//------------------------------------------------------------------------------
void __fastcall Append_Supply_Nodes(int Number_Of_AGVs,FILE *fp,MCF_network_p net)
{

     AnsiString Str1;
     fprintf(fp,"c Create supply nodes  \n");
     MCFAlgorithmForm->Label25->Caption = "Create Supply Nodes";
     MCFAlgorithmForm->GroupBox15->Refresh() ;
     MCF_node_p node;
     node = net->nodes;
     for (int i1=1;i1<=Number_Of_AGVs;i1++)
     {
         //                Node    Supply
         Str1.printf("n    %5d     1",i1);
         int t = i1 ; node[t].balance = 1; // (MCF_flow_t)c;
         if( MCFAlgorithmForm->RadioButton7->Checked)
             fprintf(fp,"%s  \n", Str1.c_str());
         #if defined TSP-PORT
             fprintf(fp,"%s  \n", Str1.c_str());
         #endif
         MCFAlgorithmForm->ProgressBar1->Position = 100*i1/Number_Of_AGVs;
     }
}
//------------------------------------------------------------------------------
void __fastcall Append_Demand_Node(int Number_Of_AGVs, int Number_Of_Nodes,FILE *fp,MCF_network_p net)
{
     AnsiString Str1 ;
     MCF_node_p node;
     node = net->nodes;
     fprintf(fp, "c Create demand node \n");
     //                Sink    Demand
     Str1.printf("n    %5d     %5d   ",Number_Of_Nodes,-1*Number_Of_AGVs);
     int t = Number_Of_Nodes ; node[t].balance = -1*Number_Of_AGVs;
     if( MCFAlgorithmForm->RadioButton7->Checked)
         fprintf(fp,"%s  \n", Str1.c_str());
     #if defined TSP-PORT
         fprintf(fp,"%s  \n", Str1.c_str());
     #endif
}
//------------------------------------------------------------------------------
MCF_arc_p __fastcall  Append_Arcs_From_AGV_Nodes_To_Sink_Node(int Number_Of_AGVs,
                     int *Number_Of_ARCs,
                     AnsiString SinkNodeStr,FILE *fp,
                     MCF_network_p net,MCF_arc_p arc)
{
     AnsiString Str1;
     fprintf(fp, "c Create arcs from AGV Nodes to the Sink node \n");
     MCFAlgorithmForm->Label25->Caption = "Arcs from AGV to Sink";
     MCFAlgorithmForm->Refresh() ;

     MCF_node_p node;
     node = net->nodes;

     for (int i1=1;i1<=Number_Of_AGVs;i1++)
     {
            // define     AGV m     Sink  L    U     C
         Str1.printf("a    %5d     %5d    0    1     0", i1,StrToInt(SinkNodeStr));
         MCFAlgorithmForm->ProgressBar1->Position  = 100*i1/Number_Of_AGVs;

         int t= i1;                    arc->tail = &(node[t]);
         int h= StrToInt(SinkNodeStr); arc->head = &(node[h]);
         arc->lower = 0; arc->upper = 1; arc->cost = 0;
         arc->nextout = arc->tail->firstout;
         arc->tail->firstout = arc;
         arc->nextin = arc->head->firstin;
         arc->head->firstin =  arc;
         arc++;
         if( MCFAlgorithmForm->RadioButton7->Checked)
             fprintf(fp,"%s  \n", Str1.c_str());
         #if defined TSP-PORT
             fprintf(fp,"%s  \n", Str1.c_str());
         #endif
         (*Number_Of_ARCs)++;
     }
     return(arc) ;
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
        return(1000); // 10 // Distance*10);
     }
     return(1000);
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
        return(1000); // 100 // Distance*100);
     }
     return(1000);
}
//------------------------------------------------------------------------------
MCF_arc_p __fastcall Append_Arcs_From_AGVs_Nodes_To_Container_Nodes(int Number_Of_AGVs
                        ,int *Number_Of_ARCs      , AnsiString PortNameStr
                        ,int  Number_Container_Job, FILE *fp, int StaticFlag
                        ,MCF_network_p net,MCF_arc_p arc)
{
     int ReadyTimeAGV;
     int RecTable2, Distance ;
     int W1, W2;
     AnsiString Str1;

     float Penalty = Penalty1(PortNameStr);

     if(StaticFlag)
     {
        W1 = StrToFloat(MCFAlgorithmForm->Edit22->Text);
        W2 = StrToFloat(MCFAlgorithmForm->Edit2->Text);
     } else
     {
        W1 = StrToFloat(MCFAlgorithmForm->Edit22->Text); // 14->Text); //  28/04/2004
        W2 = StrToFloat(MCFAlgorithmForm->Edit2->Text);  //10->Text);  //  28/04/2004
     }

     MCF_node_p node;
     node = net->nodes;

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
             String ContainerIDStr          = Container_Buff[r-1].IDStr ;
             String SourcePointContainerStr = Container_Buff[r-1].SourcePointStr ;
             String DestPointContainerStr   = Container_Buff[r-1].DestPointStr;
             int ReadyTimeContainer         = Container_Buff[r-1].ReadyTime ;
             AnsiString PortNameIDStr       = PortNameStr;

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

             *(TimeAGVToC  + (i1-1)*Number_Container_Job + r-1) = ReadyTimeAGV + Distance + TimeValueInSecond; //28/04/04
             int DT1, DT2;
             // make a higher priority for for late jobs
            //  DT1 = ReadyTimeContainer - (Distance +  ReadyTimeAGV + TimeValueInSecond)
            //      - Late *(TimeValueInSecond- ReadyTimeContainer)
            //  if (DT1 < 0) DT1 = 0;

             DT1 = ReadyTimeContainer - (Distance +  ReadyTimeAGV + TimeValueInSecond);
             if( DT1 >= 0 )
             {
                DT1 = W1 * (ReadyTimeContainer - (Distance +  ReadyTimeAGV + TimeValueInSecond))
                    + W2 * (Distance +  ReadyTimeAGV ) ;
                if (TimeValueInSecond > ReadyTimeContainer)  // 30/04/04
                {
                      DT1 -= 2*Penalty *(TimeValueInSecond - ReadyTimeContainer);
                      if (DT1 < 0 ) DT1 = 0;
                }
                //    + ReadyTimeContainer ;
                //  + (ReadyTimeContainer - TimeValueInSecond)
                //  if (DT1 < 0 ) DT1 = 0;
                // define       AGV m     c      L   U     C
                Str1.printf("a    % 5.0d     % 5.0d     0    1    %10.1f", i1,i2,(float ) DT1);

                int t= i1; arc->tail = &(node[t]);
                int h= i2; arc->head = &(node[h]);
                arc->lower = 0; arc->upper = 1; arc->cost = DT1;
                if(net->max_cost < DT1)
                {
                   net->max_cost = DT1;
                   net->max_cost_arc = arc;
                }
                arc->nextout = arc->tail->firstout;
                arc->tail->firstout = arc;
                arc->nextin = arc->head->firstin;
                arc->head->firstin =  arc;
                arc++;

                (*Number_Of_ARCs)++;
                if( MCFAlgorithmForm->RadioButton7->Checked)
                    fprintf(fp,"%s  \n", Str1.c_str());
                #if defined TSP-PORT
                    DT1 = 1 + random(100);
                    Str1.printf("a    % 5.0d     % 5.0d     0    1    %10.1f", i1,i2,(float ) DT1);
                    fprintf(fp,"%s  \n", Str1.c_str());
                #endif
                *(CostAGVToC+(i1-1)*Number_Container_Job+ RecTable2-1) =  DT1;
             }
             else{
                DT2 = Penalty *(ReadyTimeAGV + Distance + TimeValueInSecond - ReadyTimeContainer);
                //   + W2 * Penalty *(Distance + ReadyTimeAGV );
                //    + ReadyTimeContainer ;

                if (TimeValueInSecond > ReadyTimeContainer)  // 28/04/04
                {
                      DT2 -= 2*Penalty *(TimeValueInSecond - ReadyTimeContainer);
                      if (DT2 < 0 ) DT2 = 0;
                }
                 // define arc AGV m     c     L    U    C
                Str1.printf("a    % 5.0d     % 5.0d     0    1    %10.1f", i1,i2,(float ) DT2);

                int t= i1; arc->tail = &(node[t]);
                int h= i2; arc->head = &(node[h]);
                arc->lower = 0; arc->upper = 1; arc->cost = DT2;
                if(net->max_cost < DT2)
                {
                   net->max_cost = DT2;
                   net->max_cost_arc = arc;
                }
                arc->nextout = arc->tail->firstout;
                arc->tail->firstout = arc;
                arc->nextin = arc->head->firstin;
                arc->head->firstin =  arc;
                arc++;

                (*Number_Of_ARCs)++;
                if( MCFAlgorithmForm->RadioButton7->Checked)
                    fprintf(fp,"%s  \n", Str1.c_str());
                #if defined TSP-PORT
                    DT1 = 1 + random(100);
                    Str1.printf("a    % 5.0d     % 5.0d     0    1    %10.1f", i1,i2,(float ) DT1);
                    fprintf(fp,"%s  \n", Str1.c_str());
                #endif
                *(CostAGVToC+(i1-1)*Number_Container_Job+ RecTable2-1) = DT2;
             }
             RecTable2++;
             i2 = i2 + 2;
         }
     }
     return(arc);
}
// -----------------------------------------------------------------------------
MCF_arc_p __fastcall Append_Arcs_From_CNode_To_CPNodes(int Number_Of_AGVs,
                                                       int Number_Of_Nodes,
                                                       int *Number_Of_ARCs,FILE *fp,
                                                       MCF_network_p net,MCF_arc_p arc)
{
     AnsiString Str1;
     fprintf(fp,"c Create arcs from any c node to c' container node\n");
     MCFAlgorithmForm->Label25->Caption = "Arcs from c to c'";
     MCFAlgorithmForm->Refresh() ;

     MCF_node_p node;
     node = net->nodes;

     for (int i1=Number_Of_AGVs+1;i1<=Number_Of_Nodes-1;i1=i1+2)
     {
         // define arc from any c to c'   L    U   C
         Str1.printf("a    %5d     %5d    1    1   0", i1,i1+1);

         int t= i1;     arc->tail = &(node[t]);
         int h= i1+ 1 ; arc->head = &(node[h]);
         arc->lower = 1; arc->upper = 1; arc->cost = 0;
         arc->nextout = arc->tail->firstout;
         arc->tail->firstout = arc;
         arc->nextin = arc->head->firstin;
         arc->head->firstin =  arc;
         arc++;

         if( MCFAlgorithmForm->RadioButton7->Checked)
            fprintf(fp,"%s  \n", Str1.c_str());
         #if defined TSP-PORT
             fprintf(fp,"%s  \n", Str1.c_str());
         #endif
         (*Number_Of_ARCs)++;
         MCFAlgorithmForm->ProgressBar1->Position = 100*(i1-Number_Of_AGVs-1)/(Number_Of_Nodes-1 - Number_Of_AGVs -1);
     }
     return(arc);
}
//------------------------------------------------------------------------------
MCF_arc_p __fastcall Append_Arcs_From_CPNodes_To_Sink_Node(int Number_Of_AGVs
                                ,int Number_Of_Nodes, AnsiString SinkNodeStr
                                ,int *Number_Of_ARCs,FILE *fp
                                ,MCF_network_p net,MCF_arc_p arc)
{
     AnsiString Str1;

     fprintf(fp,"c Create arcs from c' node to sink node \n");
     MCFAlgorithmForm->Label25->Caption = "Arcs from c' to Sink";
     MCFAlgorithmForm->Refresh() ;

     MCF_node_p node;
     node = net->nodes;

     for (int i1=Number_Of_AGVs+2;i1<=Number_Of_Nodes-1;i1=i1+2)
     {
         //                 C'     Sink   L   U   C
         Str1.printf("a    %5d     %5d    0   1   0", i1,StrToInt(SinkNodeStr));

         int t= i1;                     arc->tail = &(node[t]);
         int h= StrToInt(SinkNodeStr) ; arc->head = &(node[h]);
         arc->lower = 0; arc->upper = 1; arc->cost = 0;
         arc->nextout = arc->tail->firstout;
         arc->tail->firstout = arc;
         arc->nextin = arc->head->firstin;
         arc->head->firstin =  arc;
         arc++;
         if( MCFAlgorithmForm->RadioButton7->Checked)
             fprintf(fp,"%s  \n", Str1.c_str());
         #if defined TSP-PORT
             fprintf(fp,"%s  \n", Str1.c_str());
         #endif
         (*Number_Of_ARCs)++;
         if(Number_Of_Nodes-1 - Number_Of_AGVs -2 > 0 )
            MCFAlgorithmForm->ProgressBar1->Position = 100*(i1-Number_Of_AGVs-2)/(Number_Of_Nodes-1 - Number_Of_AGVs -2);
     }
     return(arc);
}
//------------------------------------------------------------------------------
MCF_arc_p __fastcall Append_Arcs_From_CPNodes_To_Any_Other_CNodes(int Number_Of_AGVs
                        ,int Number_Of_Container_Jobs, int *Number_Of_ARCs
                        ,AnsiString PortNameIDStr    ,FILE *fp
                        ,int StaticFlag              ,MCF_network_p net,MCF_arc_p arc)
{
     int Tij ;
     int W1, W2;

     if (Number_Of_Container_Jobs <= 1) return(arc);

     float Penalty = Penalty2(PortNameIDStr);

     if(StaticFlag)
     {
        W1 = StrToFloat(MCFAlgorithmForm->Edit22->Text);
        W2 = StrToFloat(MCFAlgorithmForm->Edit2->Text);
     } else
     {
        W1 = StrToFloat(MCFAlgorithmForm->Edit22->Text); // 14->Text); // 28/04/2004
        W2 = StrToFloat(MCFAlgorithmForm->Edit2->Text); // 10->Text);  // 28/04/2004
     }

     MCF_node_p node;
     node = net->nodes;

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
                     //   + ReadyTimeContainer2;
                     //                 Agv m   C     L   U   C
                     Str1.printf("a    %5d     %5d    0   1   %10.1f", i1,i2,(float ) DT1 );

                     int t= i1; arc->tail = &(node[t]);
                     int h= i2; arc->head = &(node[h]);
                     arc->lower = 0; arc->upper = 1; arc->cost = DT1;
                     if(net->max_cost < DT1)
                     {
                        net->max_cost = DT1;
                        net->max_cost_arc = arc;
                     }   
                     arc->nextout = arc->tail->firstout;
                     arc->tail->firstout = arc;
                     arc->nextin = arc->head->firstin;
                     arc->head->firstin =  arc;
                     arc++;
                     if( MCFAlgorithmForm->RadioButton7->Checked)
                        fprintf(fp,"%s  \n", Str1.c_str());
                     #if defined TSP-PORT
                         DT1 = 1 + random(100);
                         Str1.printf("a    % 5.0d     % 5.0d     0    1    %10.1f", i1,i2,(float ) DT1);
                         fprintf(fp,"%s  \n", Str1.c_str());
                     #endif
                     *(CostCToC + (RecTable2-1)*Number_Of_Container_Jobs + RecTable3-1) = DT1;
                  }else
                  {
                     *Number_Of_ARCs = *Number_Of_ARCs + 1;
                     DT2 = Penalty * (ReadyTimeContainer + Tij - ReadyTimeContainer2);
                     //    + W2 * Penalty * Tij;
                     //    + ReadyTimeContainer;
                     //     - Late *(TimeValueInSecond - ReadyTimeContainer2)
                     // if (DT2 < 0) DT2 = 0;
                     //                 Agv m   C     L   U   C
                     Str1.printf("a    %5d     %5d    0   1   %10.1f", i1,i2,(float ) DT2 );

                     int t= i1; arc->tail = &(node[t]);
                     int h= i2; arc->head = &(node[h]);
                     arc->lower = 0; arc->upper = 1; arc->cost = DT2;
                     if(net->max_cost < DT2)
                     {
                        net->max_cost = DT2;
                        net->max_cost_arc = arc;
                     }

                     arc->nextout = arc->tail->firstout;
                     arc->tail->firstout = arc;
                     arc->nextin = arc->head->firstin;
                     arc->head->firstin =  arc;
                     arc++;

                     if( MCFAlgorithmForm->RadioButton7->Checked)
                         fprintf(fp,"%s  \n", Str1.c_str());
                     #if defined TSP-PORT
                         DT1 = 1 + random(100);
                         Str1.printf("a    % 5.0d     % 5.0d     0    1    %10.1f", i1,i2,(float ) DT1);
                         fprintf(fp,"%s  \n", Str1.c_str());
                     #endif
                     *(CostCToC + (RecTable2-1)*Number_Of_Container_Jobs + RecTable3-1) = DT2;
                  }
                }
                i2 = i2 + 2;
          }
          RecTable2++;
     }
     return(arc);
}
//------------------------------------------------------------------------------
int  __fastcall TMCFAlgorithmForm::Read_Container_Jobs(int Dynamic_Flag)
{
     int  i;
     int  CTime = TimeValueInSecond;
     int  Time2 = 0 ;

     int Number_Of_Container_Jobs = PortContainerForm->Table2->RecordCount;
     Port_Buff.NumberOfJobs       = Number_Of_Container_Jobs;

     int r = 0;
     PortContainerForm->Table2->First();
     for(i=1;i<=Number_Of_Container_Jobs;i++)
     {
         if(r > Maximum_Container_Jobs ) break;

         AnsiString ContainerIDStr          = PortContainerForm->Table2->FieldByName("ContainerID")->AsString;
         AnsiString SourcePointContainerStr = PortContainerForm->Table2->FieldByName("SOurcePoint")->AsString;
         AnsiString DestPointContainerStr   = PortContainerForm->Table2->FieldByName("DestPoint")->AsString;
         int ReadyTimeContainer             = StrToInt(PortContainerForm->Table2->FieldByName("ReadyTime")->AsString);
         int QuayCraneTime                  = StrToInt(PortContainerForm->Table2->FieldByName("QuayCraneTime")->AsString);
         bool Done                          = PortContainerForm->Table2->FieldValues["Done"];

         if(!Done && RadioButton31->Checked)
         {
           Container_Buff[r].IDStr          = ContainerIDStr;
           Container_Buff[r].SourcePointStr = SourcePointContainerStr;
           Container_Buff[r].DestPointStr   = DestPointContainerStr;
           Container_Buff[r].ReadyTime      = ReadyTimeContainer;
           Container_Buff[r].QCraneTime     = QuayCraneTime;
           Container_Buff[r].Done           = 0;
           r++;
         }else if(RadioButton30->Checked || RadioButton22->Checked || RadioButton32->Checked)
         {
           Container_Buff[r].IDStr          = ContainerIDStr;
           Container_Buff[r].SourcePointStr = SourcePointContainerStr;
           Container_Buff[r].DestPointStr   = DestPointContainerStr;
           Container_Buff[r].ReadyTime      = ReadyTimeContainer;
           Container_Buff[r].QCraneTime     = QuayCraneTime;
           Container_Buff[r].Vehicle        = 0;
           if(Dynamic_Flag)
              Container_Buff[r].Empty = 0;
           r++;
         }
         PortContainerForm->Table2->Next();
     }
     return(r);
}
//------------------------------------------------------------------------------
void __fastcall TMCFAlgorithmForm::Initilize_Dynamic_Fashion()
{     // Start of Dynamic fashion
     int Index = MCFAlgorithmForm->ListBox3->ItemIndex;
     AnsiString PortNameStr = MCFAlgorithmForm->ListBox3->Items->Strings[Index];

     if(!Read_Port(PortNameStr)) return;

     Read_Route(PortNameStr);


     PortAGVForm->Table2->First ();
     for(int VN=1;VN <= PortAGVForm->Table2->RecordCount ; VN++)
     {
         String AGVLocation = PortAGVForm->Table2->FieldValues["StartLocation"];
         if(MCFAlgorithmForm->Table4->RecordCount > 0)
         {
            MCFAlgorithmForm->Table4->First();
            int Junc_D ;
            int Junc_S  =  Get_Junction(PortNameStr,AGVLocation);
            String SourceStr  = MCFAlgorithmForm->Table4->FieldByName("SourcePoint")->AsString;
            String DestStr    = MCFAlgorithmForm->Table4->FieldByName("DestPoint")->AsString;
            if(AGVLocation != SourceStr)
               Junc_D =  Get_Junction(PortNameStr,SourceStr);
            else
               Junc_D =  Get_Junction(PortNameStr,DestStr  );

            int Distance = Shortest_Path_Dijkstra(PortNameStr,Junc_S,Junc_D,VN );
            if (Distance < Infinity )
            {
                PortAGVForm->Table2->Edit();
                PortAGVForm->Table2->FieldValues["Lane"] = Vehicle_Buff[VN-1].Lane[0];
                PortAGVForm->Table2->FieldValues["TimeInLane"] = 0;
                PortAGVForm->Table2->FieldValues["NextLane"] = Vehicle_Buff[VN-1].Lane[1];
                PortAGVForm->Table2->Post();
            }
         }
         PortAGVForm->Table2->Next ();
     }
     PortAGVForm->Table2->Refresh();

}
//------------------------------------------------------------------------------
void __fastcall TMCFAlgorithmForm::Initialize_Static_Fashion()
{
       for(int CN=1;CN<=Port_Buff.NumberOfWorkingPosition;CN++)
       {
           Crane_Buff[CN-1].Number_Of_Done_Jobs = 0;
           Crane_Buff[CN-1].Next_Generation_Time= StrToInt(MCFAlgorithmForm->Edit23->Text);
           Crane_Buff[CN-1].Last_Completed_Time = StrToInt(MCFAlgorithmForm->Edit23->Text);
       }
       for(int VN=1;VN<=Port_Buff.NumberOfAGVs;VN++)
       {
           Vehicle_Buff[VN-1].Number_Of_Jobs = 0;
           Vehicle_Buff[VN-1].Last_Completed_Time = TimeValueInSecond;
       }
}
//------------------------------------------------------------------------------
void __fastcall TMCFAlgorithmForm::ListBox1Click(TObject *Sender)
{
     TLocateOptions Opts;

     Variant locvalues[2];

     Opts.Clear();

     AnsiString PortName = ListBox1->Items->Strings[ListBox1->ItemIndex];
     if(Read_Port(PortName))
     {
         Edit3->Text  = IntToStr(Port_Buff.NumberOfBlockYard);
         Edit6->Text  = IntToStr(Port_Buff.NumberOfWorkingPosition);
         Edit4->Text  = IntToStr(Port_Buff.NumberOfAGVs);
     } else
         return;

     PortLayoutForm->Table2->Filter   = AnsiString("Portname = '") + ListBox1->Items->Strings[ListBox1->ItemIndex] + "'";

     MCFAlgorithmForm->Table1->Filter = AnsiString("Portname = '") + ListBox1->Items->Strings[ListBox1->ItemIndex] + "'";

     Label35->Caption = ListBox1->Items->Strings[ListBox1->ItemIndex];
     MCFAlgorithmForm->Table1->Filtered = true;

     MCFAlgorithmForm->Table2->Filter = AnsiString("Portname = '") + ListBox1->Items->Strings[ListBox1->ItemIndex] + "'";
     MCFAlgorithmForm->Table2->Filtered = true;

     PortContainerForm->Table2->Filter= AnsiString("Portname = '") + ListBox1->Items->Strings[ListBox1->ItemIndex] + "'";

     PortAGVForm->Table2->Filter = AnsiString("Portname = '") + ListBox1->Items->Strings[ListBox1->ItemIndex] + "'";

     PortLayoutForm->DataSource1->Enabled = true;
     PortContainerForm->DataSource1->Enabled = true;
     PortAGVForm->DataSource1->Enabled = true;
     DataSource3->Enabled = true;
     DataSource1->Enabled = true;
     Button11->Enabled = true;
     Initialize_Static_Fashion(); // Static fashion
}
//------------------------------------------------------------------------------
int  __fastcall Prime_Mover(int  Number_Of_AGVs      , AnsiString PortNameStr
                           ,int  Number_Container_Job, FILE  *fp
                           ,int  StaticFlag )
{
     TLocateOptions Opts;
     Variant locvalues[3];

     int ReadyTimeAGV, RV=0;
     int RecTable2, Distance ;
     int W1, W2;
     AnsiString Str1;
     unsigned long Max_Cost=0;

     float Penalty = Penalty1(PortNameStr);

     int Number_Of_Jobs, Number_Of_Remaining_Jobs, Quay_Crane_Time_Window, Number_Of_Cranes, Time_Window_Vehicle;

     int QuayCraneTime, ActualTime, VehicleTime , CN;

     if(StaticFlag)
     {
         Number_Of_Jobs = StrToInt(MCFAlgorithmForm->Edit1->Text);
         Number_Of_Remaining_Jobs = Number_Of_Jobs;
         Quay_Crane_Time_Window = StrToInt(MCFAlgorithmForm->Edit26->Text);
         Time_Window_Vehicle    = StrToInt(MCFAlgorithmForm->Edit5->Text);
         Number_Of_Cranes       = StrToInt(MCFAlgorithmForm->Edit6->Text);
         W1 = StrToFloat(MCFAlgorithmForm->Edit22->Text);
         W2 = StrToFloat(MCFAlgorithmForm->Edit2->Text);
     } else
     {
         Quay_Crane_Time_Window = StrToInt(MCFAlgorithmForm->Edit18->Text);
         Time_Window_Vehicle    = StrToInt(MCFAlgorithmForm->Edit30->Text);
         Number_Of_Cranes       = StrToInt(MCFAlgorithmForm->Edit12->Text);
         W1 = StrToFloat(MCFAlgorithmForm->Edit22->Text); // 14->Text); // 28/04/2004
         W2 = StrToFloat(MCFAlgorithmForm->Edit2->Text);  // 10->Text); // 28/04/2004
     }

     if( MCFAlgorithmForm->RadioButton7->Checked && fp)
        fprintf(fp, "c Finding a Job with minimum cost for the vehicles\n");
     MCFAlgorithmForm->Edit28->Text = "Looking for Prime-Mover";
     MCFAlgorithmForm->Refresh() ;


     PortAGVForm->Table2->First() ;
     for (int VN=1;VN<=Number_Of_AGVs;VN++)
     {
         MCFAlgorithmForm->ProgressBar1->Position = 100*VN/Number_Of_AGVs;
         int i2=Number_Of_AGVs+1;
         AnsiString  StartLocationAGVStr = Get_StartLocation_And_ReadyTime_AGV(PortNameStr,VN, &ReadyTimeAGV);
         RecTable2=1;
         int NoRecTable2 = Number_Container_Job;

         int JobNo = 0;

         unsigned long MinCost = Infinity;

         AnsiString AGVNameStrS = "AGV-"; AGVNameStrS = AGVNameStrS + StrToInt(VN);

         String  AGVStatus =  PortAGVForm->Table2->FieldValues["Status"];

         if (AGVStatus != "Idle" )
         {
             PortAGVForm->Table2->Next() ;
             continue;
         }

         for(int r=1;r<=NoRecTable2;r++)
         {
             String ContainerIDStr          = Container_Buff[r-1].IDStr ;
             String SourcePointContainerStr = Container_Buff[r-1].SourcePointStr ;
             String DestPointContainerStr   = Container_Buff[r-1].DestPointStr;
             int ReadyTimeContainer         = Container_Buff[r-1].ReadyTime ;
             AnsiString PortNameIDStr       = PortNameStr;

             if(Container_Buff[r-1].Done == 1) continue;

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

             int DT1, DT2;
             DT1 = ReadyTimeContainer - (Distance +  ReadyTimeAGV + TimeValueInSecond);
             if( DT1 >= 0 )
             {
                DT1 = W1 * (ReadyTimeContainer - (Distance +  ReadyTimeAGV + TimeValueInSecond))
                    + W2 * (Distance +  ReadyTimeAGV ) ;

                Str1.printf("C From %10.10s To %10.10s %10.1f", AGVNameStrS,Container_Buff[r-1].IDStr,(float ) DT1);
                if( MCFAlgorithmForm->RadioButton7->Checked && fp)
                    fprintf(fp,"%s  \n", Str1.c_str());

                if(DT1 <  MinCost)
                {
                   JobNo = r; MinCost = DT1;
                   VehicleTime = Distance +  ReadyTimeAGV + TimeValueInSecond + Time_Window_Vehicle ;
                }
                if( Max_Cost < DT1) Max_Cost = DT1;

             }
             else{
                DT2 = Penalty *(ReadyTimeAGV + Distance + TimeValueInSecond - ReadyTimeContainer);
                Str1.printf("C From %10.10s To %10.10s %10.1f", AGVNameStrS,Container_Buff[r-1].IDStr,(float ) DT2);

                if( MCFAlgorithmForm->RadioButton7->Checked && fp)
                    fprintf(fp,"%s  \n", Str1.c_str());

                if(DT2 <  MinCost)
                {
                   JobNo = r; MinCost = DT1;
                   VehicleTime = Distance +  ReadyTimeAGV + TimeValueInSecond + Time_Window_Vehicle ;
                }
                if( Max_Cost < DT2) Max_Cost = DT2;
             }
             RecTable2++;
             i2 = i2 + 2;
         }

         locvalues[0] = PortNameStr;
         locvalues[1] = Container_Buff[JobNo -1].IDStr;
         Opts << loCaseInsensitive ;
         bool b = PortContainerForm->Table1->Locate("Portname;ContainerID", VarArrayOf(locvalues, 1) , Opts);
         if (b && !StaticFlag && JobNo)
         {
             PortContainerForm->Table1->Edit();
             PortContainerForm->Table1->FieldValues["Done"]= true;
             PortContainerForm->Table1->Post();
             PortContainerForm->Table2->Refresh() ;
             RV = 1;
         }
         if( JobNo )
         {
             Container_Buff[JobNo -1].Done = 1;

             MCFAlgorithmForm->Table5->Append();

             MCFAlgorithmForm->Table5->FieldValues["Portname"]= PortNameStr;
             MCFAlgorithmForm->Table5->FieldValues["AGVName"] = AGVNameStrS;
             MCFAlgorithmForm->Table5->FieldValues["AGVNumber"]= VN;
             MCFAlgorithmForm->Table5->FieldValues["ContainerID"]  = Container_Buff[JobNo -1].IDStr;

             MCFAlgorithmForm->Table5->FieldValues["ContainerReadyTime"]= Container_Buff[JobNo -1].ReadyTime;
             MCFAlgorithmForm->Table5->FieldValues["SourcePoint"]       = Container_Buff[JobNo -1].SourcePointStr;
             MCFAlgorithmForm->Table5->FieldValues["DestPoint"]         = Container_Buff[JobNo -1].DestPointStr;

             Vehicle_Buff[VN-1].Number_Of_Jobs = 1;

             MCFAlgorithmForm->Table5->FieldValues["VehicleTime"]= VehicleTime;

             String SourcePointContainerStr = Container_Buff[JobNo-1].SourcePointStr ;
             String DestPointContainerStr   = Container_Buff[JobNo-1].DestPointStr;

             if (SourcePointContainerStr.SubString(1,3)== "W/P")
                 CN = StrToInt(SourcePointContainerStr.SubString(5,SourcePointContainerStr.Length()-4 ));
             else if (DestPointContainerStr.SubString(1,3)== "W/P")
                      CN = StrToInt(DestPointContainerStr.SubString(5,DestPointContainerStr.Length()-4 ));

             QuayCraneTime = Crane_Buff[CN-1].Last_Completed_Temp; //   Prime_Mover

             ActualTime    = max (QuayCraneTime , VehicleTime );

             Crane_Buff[CN-1].Last_Completed_Temp = ActualTime + Quay_Crane_Time_Window ; // emp = ActualTime;
             Crane_Buff[CN-1].Next_Generation_Time = ActualTime + Quay_Crane_Time_Window ;

             Vehicle_Buff[VN-1].Last_Completed_Temp = ActualTime; // emp = ActualTime;

             MCFAlgorithmForm->Table5->FieldValues["QuayCraneTime"]= QuayCraneTime;
             MCFAlgorithmForm->Table5->FieldValues["ActualTime"]= ActualTime;

             MCFAlgorithmForm->Table5->Post();

             PortAGVForm->Table2->Edit() ;
             PortAGVForm->Table2->FieldValues["NumberOfContainers"] =  1;
             PortAGVForm->Table2->Post() ;
             RV =  1;
         }
         PortAGVForm->Table2->Next() ;
     }
     if(StaticFlag)
     {
        MCFAlgorithmForm->Edit21->Text = Number_Container_Job - Number_Of_AGVs;
        MCFAlgorithmForm->Edit15->Text = IntToStr(Max_Cost);
     }
     MCFAlgorithmForm->Table5->Refresh() ;
     MCFAlgorithmForm->GroupBox14->Refresh() ;
     return(RV);
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
                GroupBox30->Visible = false;

                break;
       case 1 : // Model
                GroupBox3->Visible = false;
                GroupBox7->Visible = false;
                GroupBox18->Visible = false;
                GroupBox19->Visible = false;
                GroupBox30->Visible = false;
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
                GroupBox30->Visible = false;

                break;
       case 3 : // Result
                GroupBox3->Visible = false;
                GroupBox10->Visible = false;
                GroupBox7->Visible = false;
                GroupBox19->Visible = false;
                Button4->Enabled = false;
                GroupBox18->Visible = true;
                GroupBox30->Visible = false;

                break;
       case 4 : // Graph
                GroupBox3->Visible = false;
                GroupBox10->Visible = false;
                GroupBox7->Visible = false;
                GroupBox18->Visible = false;
                GroupBox19->Visible = true;
                GroupBox30->Visible = false;
                Button4->Enabled = false;
                PortAGVForm->Table2->First() ;
//                for(int m=1;m<=PortAGVForm->Table2->RecordCount;m++)
//                {
                for(int m=1;m<=4;m++)
                {
                   QRChart1->Chart->Series[m-1]->AddXY(m,PortAGVForm->Table2->FieldValues["CostAGV"],"Cost",0);
                   QRChart1->Chart->Series[m-1]->AddXY(m,PortAGVForm->Table2->FieldValues["WaitedTime"],"Wait",0);
                   PortAGVForm->Table2->Next() ;
                }
                break;
       case 5 : // Heuristic
                GroupBox3->Visible = false;
                GroupBox10->Visible = false;
                GroupBox7->Visible = false;
                GroupBox19->Visible = false;
                Button4->Enabled = false;
                GroupBox18->Visible = false;
                GroupBox30->Visible = true;
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

//     int  *TAGVToC, *TCToC;

//     int  *CostAGVToC,*CostCToC;

     if(ListBox3->ItemIndex < 0) return;

     if (TimeValueInSecond) return;

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

     MCFAlgorithmForm->Table2->Filter  = AnsiString("Portname = '") + ListBox3->Items->Strings[ListBox3->ItemIndex] + "'";
     MCFAlgorithmForm->Table2->Filtered= true;

     Label35->Caption = ListBox3->Items->Strings[ListBox3->ItemIndex];

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

     Initilize_Dynamic_Fashion();// Start of Dynamic fashion
     Update_Cranes_And_Vehicles();
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

     fclose(Port_Buff.Fout3);
     Set_Staus_of_Vehilce_to_Stopping();

     DynamicSchedulingStarted=false;
     ListBox3->Enabled = true;
     GroupBox30->Enabled = true;

     Screen->Cursor =  crArrow;

     Timer3->Enabled = false;

     Edit11->Enabled = true;
     Edit9->Enabled  = true;

     Edit30->Enabled = true;
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
{    // RUN

     if(MCFAlgorithmForm->ListBox3->ItemIndex < 0 )
     {
        Application->MessageBox(" No port !! ", "Error", MB_OK);
        return;
     }

   //  Initilize_Dynamic_Fashion();// Start of Dynamic fashion
     Update_Cranes_And_Vehicles();
     for(int r=0;r<Maximum_Container_Jobs;r++)
         Container_Buff[r].Empty = 1;

     ListBox3->Enabled = false;
     GroupBox30->Enabled = false;
     Screen->Cursor =  crHourGlass;    // Show hourglass cursor

     int Number_Of_AGVs = PortAGVForm->Table2->RecordCount;

     int Current_AGV = PortAGVForm->Table2->RecNo  ;

     PortAGVForm->Table2->First() ;
     for(int VN=1;VN<=Number_Of_AGVs;VN++)
     {
           PortAGVForm->Table2->Edit() ;
           PortAGVForm->Table2->FieldValues["CostAGV"] = 0;
           PortAGVForm->Table2->FieldValues["WaitedTime"] = 0;
           PortAGVForm->Table2->Post() ;
           PortAGVForm->Table2->Next() ;
     }
     PortAGVForm->Table2->Refresh();

     MCFAlgorithmForm->DBGrid4->Columns->Items[9]->Title->Caption  = "Travelled Time"  ;
     MCFAlgorithmForm->DBGrid4->Refresh ();

     PortAGVForm->Table2->RecNo = Current_AGV;

     if(BenchOptionForm->RadioButton1->Checked)
         Timer3->Interval = 1000;
     else if(BenchOptionForm->RadioButton2->Checked)
          {
             Timer3->Interval = 100;
             BenchOptionForm->CheckBox1->Checked = false;
          }else if(BenchOptionForm->RadioButton3->Checked)
                {
                   Timer3->Interval = 10;
                   BenchOptionForm->CheckBox1->Checked = false;
                }

     Timer3->Enabled = true;

     Button1->Enabled = false; // Resume
     Button2->Enabled = true; //  Suspend
     Button3->Enabled = false; //  Close
     Button7->Enabled = false;
     Button8->Enabled = true; //  Stop
     Button9->Enabled = false; //  Reset

     Edit11->Enabled = false;
     Edit9->Enabled  = false;
     Edit18->Enabled = false;
     Edit30->Enabled = false;

     TDateTime DateTime = Time();  // store the current date and time
     AnsiString Str = TimeToStr(DateTime); // convert the time to a string
     Edit24->Text = Str;

     if(RadioButton31->Checked)
     {
        PortAGVForm->Table2->First() ;
        for(int VN=1;VN<=Port_Buff.NumberOfAGVs ;VN++)
        {
            while(MCFAlgorithmForm->Table4->RecordCount > 0)
            {
                MCFAlgorithmForm->Table4->First();
                MCFAlgorithmForm->Table4->Delete();
            }
            PortAGVForm->Table2->Next() ;
        }
        int Number_Of_Container_Jobs = Read_Container_Jobs(1);
        int RV = Prime_Mover(Port_Buff.NumberOfAGVs,Port_Buff.Port,
                    Number_Of_Container_Jobs,Port_Buff.Fout3,0);

     }
     if(RadioButton30->Checked || RadioButton22->Checked) // Network Simplex Algorithm
     {
        Button9Click(Sender);  // Reset Every thing

        for(int CN=1;CN<=Port_Buff.NumberOfWorkingPosition;CN++)
        {

            if (Crane_Buff[CN-1].Number_Of_Jobs == 0)
                Insert_Containers_To_Table(CN); // Generate Container

            Job_Crane_Buff[CN-1].Number_Of_Fulfilled = 0;
            Job_Crane_Buff[CN-1].Number_Of_New_Jobs  = 0;
        }
        //Update_Cranes_And_Vehicles();
        //Initialize_New_Schedule(PortNameStr);

        Timer1Timer(Sender); // Making the model, solving it and Generating new schedule:

        Button2Click(Sender); // Suspend  for Test.
     }
}
//------------------------------------------------------------------------------

void __fastcall TMCFAlgorithmForm::Button2Click(TObject *Sender)
{    // Suspend
     Set_Staus_of_Vehilce_to_Stopping();
     Button2->Enabled = false;
     Button1->Enabled = true;

     Timer1->Enabled = false;
     Timer3->Enabled = false;
}
//------------------------------------------------------------------------------

void __fastcall TMCFAlgorithmForm::Button1Click(TObject *Sender)
{    // Resume
     Button2->Enabled = true;
     Button1->Enabled = false;
   //  Timer1->Enabled = true;
     Timer3->Enabled = true;
}
//------------------------------------------------------------------------------

void __fastcall TMCFAlgorithmForm::Table4CalcFields(TDataSet *DataSet)
{
       Table4WaitingTime->Value = Table4->FieldByName("VehicleTime")->Value
                                - Table4->FieldByName("QuayCraneTime")->Value;

       AnsiString Port   = Table4->FieldByName ("Portname")->AsString;
       AnsiString Source = Table4->FieldByName ("SourcePoint")->Value;
       AnsiString Dest   = Table4->FieldByName ("DestPoint")->Value;
       Table4DistanceSD->Value =  Get_Time_Between_Two_Points(Port,Source,Dest);
    
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
void __fastcall Set_Shortest_Path_Dijkstra(String PortNameStr,String SourceStr, String DestStr, int VN)
{
    int Junc_S = Get_Junction(PortNameStr,SourceStr);
    int Junc_D = Get_Junction(PortNameStr,DestStr  );

    int Distance = Shortest_Path_Dijkstra(PortNameStr,Junc_S,Junc_D,VN );
    if (Distance < Infinity )
    {
      PortAGVForm->Table2->FieldValues["Lane"] = Vehicle_Buff[VN-1].Lane[0];
      PortAGVForm->Table2->FieldValues["TimeInLane"] = 0;
      PortAGVForm->Table2->FieldValues["NextLane"] = Vehicle_Buff[VN-1].Lane[1];
    }
}

//------------------------------------------------------------------------------
void  __fastcall Set_Next_Location_Vehicle_Q_Source_Q(int ActualTime, int PassTime,  bool StartOfNewSchedule,AnsiString PortNameStr,
                         int a,int b,AnsiString SourcePointContainerStr,AnsiString DestPointContainerStr,int m)
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
            PortAGVForm->Table2->FieldValues["Lane"]      = 0;
            PortAGVForm->Table2->FieldValues["NextLane"]  = 0;
            PortAGVForm->Table2->FieldValues["TimeInLane"]= 0;
        }else
        {
            PortAGVForm->Table2->FieldValues["PreviousLocation"]= SourcePointContainerStr;
            PortAGVForm->Table2->FieldValues["StartLocation"]= DestPointContainerStr;
            PortAGVForm->Table2->FieldValues["ReadyTime"] = b  + StrToInt(MCFAlgorithmForm->Edit30->Text ) ;
            PortAGVForm->Table2->FieldValues["Status"] = "Going" ;
            PortAGVForm->Table2->FieldValues["Load"]   = true;
            Set_Shortest_Path_Dijkstra(PortNameStr,SourcePointContainerStr,DestPointContainerStr,m);
         }
    } else // a > 0 , Source <> Vehicle's Location
    {
         String SLocation = PortAGVForm->Table2->FieldValues["StartLocation"];
         PortAGVForm->Table2->FieldValues["PreviousLocation"]= SLocation;
         PortAGVForm->Table2->FieldValues["StartLocation"]   = SourcePointContainerStr;
         PortAGVForm->Table2->FieldValues["ReadyTime"]    = a + StrToInt(MCFAlgorithmForm->Edit30->Text ) ;
         PortAGVForm->Table2->FieldValues["Status"] = "Going" ;
         PortAGVForm->Table2->FieldValues["Load"]   = false;
         Set_Shortest_Path_Dijkstra(PortNameStr,SLocation,SourcePointContainerStr,m);
    }
}
//---------------------------------------------------------------------------
void  __fastcall Set_Next_Location_Vehicle_Q_Dest_Q(int ActualTime, int PassTime,  bool StartOfNewSchedule,AnsiString PortNameStr,
                         int a,int b,AnsiString SourcePointContainerStr,AnsiString DestPointContainerStr,int m)
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
          PortAGVForm->Table2->FieldValues["Lane"]      = 0;
          PortAGVForm->Table2->FieldValues["NextLane"]  = 0;
          PortAGVForm->Table2->FieldValues["TimeInLane"]= 0;
       }else
       {
         //  MCFAlgorithmForm->Table4->Next() ;//  25/04/2004
         if( !MCFAlgorithmForm->Table4->Eof  )
         {
              MCFAlgorithmForm->Table4->Next() ;//  25/04/2004
              String SourcePointContainerStr2 = MCFAlgorithmForm->Table4->FieldByName("SourcePoint")->AsString;
              String DestPointContainerStr2   = MCFAlgorithmForm->Table4->FieldByName("DestPoint")->AsString;
              String AGVLocation = PortAGVForm->Table2->FieldValues["StartLocation"];
              PortAGVForm->Table2->FieldValues["PreviousLocation"]= DestPointContainerStr;
              PortAGVForm->Table2->FieldValues["StartLocation"]= SourcePointContainerStr2;
              int d = Get_Time_Between_Two_Points(PortNameStr,AGVLocation , SourcePointContainerStr2);
              PortAGVForm->Table2->FieldValues["ReadyTime"]    = d + StrToInt(MCFAlgorithmForm->Edit30->Text ) ;
              PortAGVForm->Table2->FieldValues["Status"] = "Going" ;
              PortAGVForm->Table2->FieldValues["Load"]   = false;
              Set_Shortest_Path_Dijkstra(PortNameStr,DestPointContainerStr,SourcePointContainerStr2,m);
         } else  // This else block is appended on 25/04/2004
         {
              String AGVLocation = PortAGVForm->Table2->FieldValues["StartLocation"];
              PortAGVForm->Table2->FieldValues["PreviousLocation"]= AGVLocation;
              PortAGVForm->Table2->FieldValues["StartLocation"]= DestPointContainerStr;
              PortAGVForm->Table2->FieldValues["Status"] = "Idle" ;
              PortAGVForm->Table2->FieldValues["Load"]   = false ;
              PortAGVForm->Table2->FieldValues["ReadyTime"]  = 0;
              PortAGVForm->Table2->FieldValues["Lane"]       = 0;
              PortAGVForm->Table2->FieldValues["TimeInLane"] = 0;
              PortAGVForm->Table2->FieldValues["NextLane"]   = 0;
         }
       }
    }  else
    {
         String SLocation = PortAGVForm->Table2->FieldValues["StartLocation"];
         PortAGVForm->Table2->FieldValues["PreviousLocation"]= SLocation;
         PortAGVForm->Table2->FieldValues["StartLocation"]= SourcePointContainerStr;
         PortAGVForm->Table2->FieldValues["ReadyTime"]    = a ; // 26/04/2004 + StrToInt(MCFAlgorithmForm->Edit30->Text );
         PortAGVForm->Table2->FieldValues["Status"] = "Going" ;
         PortAGVForm->Table2->FieldValues["Load"] = false;
         Set_Shortest_Path_Dijkstra(PortNameStr,SLocation,SourcePointContainerStr,m);
    }
}
//------------------------------------------------------------------------------
void  __fastcall Set_Next_Location_Vehicle_Y_Source_Q(int ActualTime, int PassTime, bool StartOfNewSchedule,AnsiString PortNameStr,
                         int a,int b,String SourcePointContainerStr,String DestPointContainerStr,int m)
{  // case 3: V.S.D=Y.Q.Y
       String SLocation = PortAGVForm->Table2->FieldValues["StartLocation"];
       PortAGVForm->Table2->FieldValues["PreviousLocation"]= SLocation;
       PortAGVForm->Table2->FieldValues["StartLocation"]= SourcePointContainerStr;
       PortAGVForm->Table2->FieldValues["ReadyTime"]    = a;
       PortAGVForm->Table2->FieldValues["Status"] = "Going" ;
       PortAGVForm->Table2->FieldValues["Load"] = false;
       Set_Shortest_Path_Dijkstra(PortNameStr,SLocation,SourcePointContainerStr,m);
}
//------------------------------------------------------------------------------
void  __fastcall Set_Next_Location_Vehicle_Y_Dest_Q(int ActualTime, int PassTime, bool StartOfNewSchedule,AnsiString PortNameStr,
                         int a,int b,AnsiString SourcePointContainerStr,AnsiString DestPointContainerStr,int m)
{   // case 4: V.S.D=Y.Y.Q
    if ( a == 0) // Vehicle location = Source
    {
       PortAGVForm->Table2->FieldValues["Load"] = true;
       String SLocation = PortAGVForm->Table2->FieldValues["StartLocation"];
       PortAGVForm->Table2->FieldValues["PreviousLocation"]= SLocation;
       PortAGVForm->Table2->FieldValues["StartLocation"]= DestPointContainerStr;
       PortAGVForm->Table2->FieldValues["ReadyTime"]    = b;
       PortAGVForm->Table2->FieldValues["Status"] = "Going" ;
       Set_Shortest_Path_Dijkstra(PortNameStr,SLocation,DestPointContainerStr,m);
    }else
    {
       String SLocation = PortAGVForm->Table2->FieldValues["StartLocation"];
       PortAGVForm->Table2->FieldValues["PreviousLocation"]= SLocation;
       PortAGVForm->Table2->FieldValues["StartLocation"]= SourcePointContainerStr;
       PortAGVForm->Table2->FieldValues["ReadyTime"]    = a;
       PortAGVForm->Table2->FieldValues["Status"] = "Going" ;
       PortAGVForm->Table2->FieldValues["Load"] = false;
       Set_Shortest_Path_Dijkstra(PortNameStr,SLocation,SourcePointContainerStr,m);
    }
}
//------------------------------------------------------------------------------
void   __fastcall  Set_Next_Location_And_Path_For_Vehilce(
                   String PortNameStr,int Vehicle_No,
                   String AGVLocation,int PassTime  , bool StartOfNewSchedule)
{
    bool Load = PortAGVForm->Table2->FieldValues["Load"];
    if(AGVLocation.SubString(1,3)  == "W/P")
    {
     //  if( !MCFAlgorithmForm->Table4->Eof )
       if( MCFAlgorithmForm->Table4->RecordCount > 0 ) // 25/04/2004
       {
         MCFAlgorithmForm->Table4->First();
         String SourceStr = MCFAlgorithmForm->Table4->FieldByName("SourcePoint")->AsString;
         String DestStr   = MCFAlgorithmForm->Table4->FieldByName("DestPoint")->AsString;
         int ActualTime   = MCFAlgorithmForm->Table4->FieldValues["ActualTime"];
         int a = Get_Time_Between_Two_Points(PortNameStr,AGVLocation,SourceStr);
         int b = Get_Time_Between_Two_Points(PortNameStr,SourceStr  ,DestStr);
         if(SourceStr.SubString(1,3) == "W/P")
            Set_Next_Location_Vehicle_Q_Source_Q(ActualTime, PassTime,StartOfNewSchedule,PortNameStr,
                                      a,b,SourceStr,DestStr,Vehicle_No);
         else
            Set_Next_Location_Vehicle_Q_Dest_Q(ActualTime, PassTime,StartOfNewSchedule,PortNameStr,
                                      a,b,SourceStr,DestStr,Vehicle_No);
       }else
       {
          PortAGVForm->Table2->FieldValues["Status"] = "Idle" ;
          PortAGVForm->Table2->FieldValues["Load"]   = false ;
          PortAGVForm->Table2->FieldValues["ReadyTime"]  = 0;
          PortAGVForm->Table2->FieldValues["Lane"]       = 0;
          PortAGVForm->Table2->FieldValues["TimeInLane"] = 0;
          PortAGVForm->Table2->FieldValues["NextLane"]   = 0;
       }
    }else // AGVLocation = Yard
    {
      // if( !MCFAlgorithmForm->Table4->Eof )
       if( MCFAlgorithmForm->Table4->RecordCount > 0 ) // 25/04/2004
       {
         MCFAlgorithmForm->Table4->First();
         String SourceStr = MCFAlgorithmForm->Table4->FieldByName("SourcePoint")->AsString;
         String DestStr   = MCFAlgorithmForm->Table4->FieldByName("DestPoint")->AsString;
         int ActualTime   = MCFAlgorithmForm->Table4->FieldValues["ActualTime"];
         int a = Get_Time_Between_Two_Points(PortNameStr,AGVLocation,SourceStr);
         int b = Get_Time_Between_Two_Points(PortNameStr,SourceStr  ,DestStr);
         if(SourceStr.SubString(1,3) == "W/P")
            Set_Next_Location_Vehicle_Y_Source_Q(ActualTime, PassTime,StartOfNewSchedule, PortNameStr,
                                      a,b,SourceStr,DestStr,Vehicle_No);
         else
            Set_Next_Location_Vehicle_Y_Dest_Q(ActualTime, PassTime, StartOfNewSchedule, PortNameStr,
                                      a,b,SourceStr,DestStr,Vehicle_No);
       }else
       {
          PortAGVForm->Table2->FieldValues["Status"] = "Idle" ;
          PortAGVForm->Table2->FieldValues["Load"]   = false;
          PortAGVForm->Table2->FieldValues["ReadyTime"]  = 0;
          PortAGVForm->Table2->FieldValues["Lane"]       = 0;
          PortAGVForm->Table2->FieldValues["TimeInLane"] = 0;
          PortAGVForm->Table2->FieldValues["NextLane"]   = 0;
       }
    }
}
//------------------------------------------------------------------------------
void __fastcall PostPone_List_Of_Vehicle(int t)
{
      if(MCFAlgorithmForm->Table4->RecordCount == 0)
         return;

      MCFAlgorithmForm->Table4->Active = false;
      MCFAlgorithmForm->Table4->Exclusive = true;
      MCFAlgorithmForm->Table4->IndexFieldNames = String("PortName;AGVNumber;ContainerID");
      MCFAlgorithmForm->Table4->Exclusive = false;
      MCFAlgorithmForm->Table4->Active = true;

      MCFAlgorithmForm->Table4->First();
      for(int j=1;j<= MCFAlgorithmForm->Table4->RecordCount;j++)
      {
         int    VehicleTime = MCFAlgorithmForm->Table4->FieldValues["VehicleTime"];
         int    QCraneTime  = MCFAlgorithmForm->Table4->FieldValues["QuayCraneTime"];

         MCFAlgorithmForm->Table4->Edit();
         MCFAlgorithmForm->Table4->FieldValues["VehicleTime"]  = VehicleTime + t ;
         MCFAlgorithmForm->Table4->FieldValues["QuayCraneTime"]= QCraneTime  ;
         MCFAlgorithmForm->Table4->FieldValues["ActualTime"]   = max (VehicleTime + t , QCraneTime );
         MCFAlgorithmForm->Table4->Post();

         MCFAlgorithmForm->Table4->Next();
     }

     MCFAlgorithmForm->Table4->Active = false;
     MCFAlgorithmForm->Table4->Exclusive = true;
     MCFAlgorithmForm->Table4->IndexFieldNames = String("PortName;AGVNumber;ActualTime");
     MCFAlgorithmForm->Table4->Exclusive = false;
     MCFAlgorithmForm->Table4->Active = true;

}

//------------------------------------------------------------------------------
void   __fastcall  Set_Location_And_ReadyTime_AGV(AnsiString PortNameStr,int VN,
                                                  int PassTime,bool StartOfNewSchedule)
{
        AnsiString AGVLocation;
        int ReadyTimeAGV, LaneAGV, TimeInLaneAGV , WaitedTime, PL, L;

        AGVLocation   = PortAGVForm->Table2->FieldValues["StartLocation"];
        TimeInLaneAGV = PortAGVForm->Table2->FieldValues["TimeInLane"];
        LaneAGV       = PortAGVForm->Table2->FieldValues["Lane"];
        ReadyTimeAGV  = PortAGVForm->Table2->FieldValues["ReadyTime"];
        WaitedTime    = PortAGVForm->Table2->FieldValues["WaitedTime"];

        TimeInLaneAGV++;  ReadyTimeAGV--;
        PortAGVForm->Table2->Edit();
        PL = Vehicle_Buff[VN-1].Lane_No ;
        if(ReadyTimeAGV <= 0)
        {
          //  MessageBeep(-1);

          Route_Buff2 [Vehicle_Buff[VN-1].Lane[PL]].Vehicle = 0;
          Set_Next_Location_And_Path_For_Vehilce(PortNameStr,VN,AGVLocation,PassTime,StartOfNewSchedule);
        } else
        {
          if (TimeInLaneAGV < Route_Buff2 [LaneAGV-1].Duration)
          {
             PortAGVForm->Table2->FieldValues["TimeInLane"] = TimeInLaneAGV;
             Vehicle_Buff[VN-1].Time_in_Lane                = TimeInLaneAGV;
             PortAGVForm->Table2->FieldValues["Status"]     = "Going" ;

             int TravelledTime = PortAGVForm->Table2->FieldValues["CostAGV"];
             PortAGVForm->Table2->FieldValues["CostAGV"]  = TravelledTime + 1;
             PortAGVForm->Table2->FieldValues["ReadyTime"]= ReadyTimeAGV ;
          } else
          {
              L = Vehicle_Buff[VN-1].Lane_No  + 1;
             if( L  < Vehicle_Buff[VN - 1].Number_of_Lanes)
             {
                // Checking status of next lane: Collision, Congestion, Deadlock
                 if(Route_Buff2 [Vehicle_Buff[VN-1].Lane[L]].Vehicle && MCFAlgorithmForm->CheckBox2->Checked)
                 {
                    PortAGVForm->Table2->FieldValues["Status"] = "Collision" ;

                    MessageBeep(-1);

                    WaitedTime++;
                    PortAGVForm->Table2->FieldValues["WaitedTime"] = WaitedTime ;
                    PostPone_List_Of_Vehicle(1);
                 }
                 else
                 {  // Increasing travelling Time
                    PortAGVForm->Table2->FieldValues["Status"] = "Going" ;
                    int TravelledTime = PortAGVForm->Table2->FieldValues["CostAGV"];
                    PortAGVForm->Table2->FieldValues["CostAGV"]  = TravelledTime + 1;
                    PortAGVForm->Table2->FieldValues["ReadyTime"]= ReadyTimeAGV ;
                    // Set new lane,
                    Route_Buff2 [Vehicle_Buff[VN-1].Lane[L ]].Vehicle = VN; // It occupies the lane
                    Route_Buff2 [Vehicle_Buff[VN-1].Lane[PL]].Vehicle = 0;  // It Leaves the previous lane
                    Vehicle_Buff[VN-1].Lane_No  = L ;
                    PortAGVForm->Table2->FieldValues["Lane"] = Vehicle_Buff[VN-1].Lane[L];
                    PortAGVForm->Table2->FieldValues["TimeInLane"] = 0;
                    if( L + 1 < Vehicle_Buff[VN-1].Number_of_Lanes)
                        PortAGVForm->Table2->FieldValues["NextLane"] = Vehicle_Buff[VN-1].Lane[L+1];
                    else
                        PortAGVForm->Table2->FieldValues["NextLane"] = 0;
                 }
             }else
             {
                 PortAGVForm->Table2->FieldValues["Status"] = "Going" ;
                 int TravelledTime = PortAGVForm->Table2->FieldValues["CostAGV"];
                 PortAGVForm->Table2->FieldValues["CostAGV"]  = TravelledTime + 1;
                 PortAGVForm->Table2->FieldValues["ReadyTime"]= ReadyTimeAGV ;

                 PortAGVForm->Table2->FieldValues["Lane"]    = 0;
                 Route_Buff2 [Vehicle_Buff[VN-1].Lane[PL]].Vehicle = 0;
                 PortAGVForm->Table2->FieldValues["TimeInLane"] = 0;
                 Vehicle_Buff[VN-1].Time_in_Lane      = 0;
            }
          }
        }
        PortAGVForm->Table2->Post();
}
//------------------------------------------------------------------------------
void  __fastcall  Update_Information_Container_Jobs(int TimeValueInSecond)
{
}
//------------------------------------------------------------------------------
void  __fastcall TMCFAlgorithmForm::Delete_Processed_Container_Jobs(long int Passed_Time, bool StartOfNewSchedule)
{
       TLocateOptions Opts;
       Set  <TLocateOption,0,1> flags;
    //   flags << loCaseInsensitive << loPartialKey;
       flags << loPartialKey;

       long int ReadyTimeContainer;
       long int DueTimeContainer;

       int  Quay_Crane_Time_Window = StrToInt(MCFAlgorithmForm->Edit18->Text);
       int  Time_Window_Vehicle    = StrToInt(MCFAlgorithmForm->Edit30->Text);


       Variant locvalues[3];
       AnsiString PortNameStr = MCFAlgorithmForm->ListBox3->Items->Strings[MCFAlgorithmForm->ListBox3->ItemIndex];
       locvalues[0] = PortNameStr;

       flags << loCaseInsensitive << loPartialKey;
       Opts.Clear();

       int Number_Of_AGVs = PortAGVForm->Table2->RecordCount;

       PortAGVForm->Table2->First() ;
       for(int VN=1;VN<=Number_Of_AGVs;VN++)
       {
          TBookmark Bookmark2 = PortAGVForm->Table2->GetBookmark ()  ;
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
                int        VN          = MCFAlgorithmForm->Table4->FieldValues["AGVNumber"];
                int        ReadyTime   = MCFAlgorithmForm->Table4->FieldValues["ContainerReadyTime"];
                int        QCraneTime  = MCFAlgorithmForm->Table4->FieldValues["QuayCraneTime"];
                int        VehicleTime = MCFAlgorithmForm->Table4->FieldValues["VehicleTime"];
                AnsiString SourcePoint = MCFAlgorithmForm->Table4->FieldByName("SourcePoint")->AsString;
                AnsiString DestinPoint = MCFAlgorithmForm->Table4->FieldByName("DestPoint")->AsString;
                int        JNo         = MCFAlgorithmForm->Table4->FieldValues["JobNO"];

                MCFAlgorithmForm->Table4->Delete() ;

                locvalues[0] = PortName;
                locvalues[1] = ContainerID;
                Opts << loCaseInsensitive ;

                bool b = PortContainerForm->Table1->Locate("Portname;ContainerID", VarArrayOf(locvalues, 1) , Opts);
                if  (b)  PortContainerForm->Table1->Delete();

                int CN;
                if (SourcePoint.SubString(1,3)== "W/P")
                    CN = StrToInt(SourcePoint.SubString(5,SourcePoint.Length()-4 ));
                else if (DestinPoint.SubString(1,3)== "W/P")
                    CN = StrToInt(DestinPoint.SubString(5,DestinPoint.Length()-4 ));

                Job_Crane_Buff[CN-1].Deleted_Jobs[Job_Crane_Buff[CN-1].Number_Of_Fulfilled] = JNo;
                Job_Crane_Buff[CN-1].Number_Of_Fulfilled++;

                Container_Buff[JNo-1].Empty = 1;


                Crane_Buff[CN-1].Last_Completed_Time = ActualTime; //Delete_Processed
                Crane_Buff[CN-1].Number_Of_Jobs--;
                Crane_Buff[CN-1].Number_Of_Done_Jobs++;

                Vehicle_Buff[VN-1].Last_Completed_Time = ActualTime;
                Vehicle_Buff[VN-1].Number_Of_Jobs --;  // Delete processed jobs
                Vehicle_Buff[VN-1].Number_Of_Done_Jobs++;


                MCFAlgorithmForm->Table1->Append ();
                MCFAlgorithmForm->Table1->FieldByName("ContainerID")->AsString = ContainerID;
                MCFAlgorithmForm->Table1->FieldValues["Portname"]              = PortName   ;
                MCFAlgorithmForm->Table1->FieldValues["AGVName"]               = AGVNameStrS;
                MCFAlgorithmForm->Table1->FieldValues["AGVNumber"]             = VN         ;
                MCFAlgorithmForm->Table1->FieldValues["ContainerReadyTime"]    = ReadyTime  ;
                MCFAlgorithmForm->Table1->FieldValues["QuayCraneTime"]         = QCraneTime ;
                MCFAlgorithmForm->Table1->FieldValues["VehicleTime"]           = VehicleTime;
                MCFAlgorithmForm->Table1->FieldValues["ActualTime"]            = ActualTime ;
                MCFAlgorithmForm->Table1->FieldValues["SourcePoint"]           = SourcePoint;
                MCFAlgorithmForm->Table1->FieldValues["DestPoint"]             = DestinPoint;

                MCFAlgorithmForm->Table1->Post ();

                if (ActualTime < ReadyTime)
                {
                    long D = ReadyTime - ActualTime;
                    Port_Buff.TotalEarlyTimes += D ;
                    Port_Buff.TotalDfApAc     += D ; //D*D;
                    Port_Buff.TotalEarlyJobs  ++;
                }
                if (ActualTime > ReadyTime)
                {
                    long D2 = -(ReadyTime - ActualTime) ;
                    Port_Buff.TotalDfApAc     += D2; // D2*D2;
                    Port_Buff.TotalLateTimes  += D2;
                    Port_Buff.TotalLateJobs  ++;
                }
                if (ActualTime > QCraneTime)
                    Port_Buff.TotalQCWTimes   +=   ActualTime - QCraneTime;
                if (ActualTime > VehicleTime)
                    Port_Buff.TotalVWTimes    +=   ActualTime - VehicleTime;

                Port_Buff.TotalDoneJobs       += 1;

                PortAGVForm->Table2->Edit();
                int NumContainers = PortAGVForm->Table2->FieldValues["NumberOfContainers"] ;
                if ( NumContainers > 0 )
                    PortAGVForm->Table2->FieldValues["NumberOfContainers"] = NumContainers - 1;
                PortAGVForm->Table2->Post();

              }
          }
          PortAGVForm->Table2->GotoBookmark(Bookmark2);
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
void   __fastcall TMCFAlgorithmForm::Insert_Containers_To_Table(int CN)
{
       int i,i1,r;
       AnsiString BlockStrS, BlockStrD;

       int Number_Of_Container_Per_Minute;
       if(DynamicSchedulingStarted)
         Number_Of_Container_Per_Minute =  StrToInt(Edit9->Text);
       else
         Number_Of_Container_Per_Minute =  StrToInt(Edit1->Text);

       if(!Number_Of_Container_Per_Minute) return;


       static int Next_Quay_Crane = 1;
       if (DynamicSchedulingStarted) Next_Quay_Crane = 1;

       long int BaseTime = TimeValueInSecond + GlobalInitialTimeQuayCrane;

       int Quay_Crane_Time_Window = StrToInt(MCFAlgorithmForm->Edit18->Text);
       int Number_Of_Cranes       = Port_Buff.NumberOfWorkingPosition ; // Edit12->Text

       TDateTime DateTime = Time();
       AnsiString TimeStr = TimeToStr(DateTime);
       TimeStr = TimeStr.SubString(1,6);

       Edit28->Text = "Incoming Containers...";
       MCFAlgorithmForm->GroupBox16->Refresh();

       if( DynamicSchedulingStarted )
           Crane_Buff[CN-1].Next_Generation_Time = Crane_Buff[CN-1].Last_Completed_Time
                                                 + Quay_Crane_Time_Window  ;
       else
           Crane_Buff[CN-1].Next_Generation_Time = Crane_Buff[CN-1].Last_Completed_Time ;

       for(i=1; i<=Number_Of_Container_Per_Minute;i++)
       {
          if(MCFAlgorithmForm->CheckBox1->Checked )
             srand(1); // 28/04/04
          else
             randomize();
          r = 3 + random(2); // 4 if we like four cases
           // 3 because we like to have only Yard to berth and vice versa
          switch(r)
          {
             case 3: // Yard to Berth
                if(MCFAlgorithmForm->CheckBox1->Checked )
                  srand(2); // 28/04/04
                else
                  randomize();
                i1 = 1+ random(StrToInt(MCFAlgorithmForm->Edit11->Text));
                BlockStrS = "Block "; BlockStrS = BlockStrS + i1;
                BlockStrD = "W/P "; BlockStrD = BlockStrD + CN;
                break;
             case 4: // Berth to Yard
                if(MCFAlgorithmForm->CheckBox1->Checked )
                  srand(3); // 28/04/04
                else
                  randomize();
                i1 = 1+ random(StrToInt(MCFAlgorithmForm->Edit11->Text));
                BlockStrS = "W/P "; BlockStrS = BlockStrS + CN;
                BlockStrD = "Block "; BlockStrD = BlockStrD + i1;
                break;
          }


          PortContainerForm->Table1->Append();

          AnsiString ContainerIDStrS = "C-DA-";                  //IntToStr(TimeValueInSecond)
          ContainerIDStrS = ContainerIDStrS + IntToStr(CN)+ "-" + IntToStr(Crane_Buff[CN-1].Number_Of_Done_Jobs)
                          + "-" + IntToStr(i);

          PortContainerForm->Table1->FieldValues["ContainerID"]= ContainerIDStrS;
          PortContainerForm->Table1->FieldValues["Portname"]   = MCFAlgorithmForm->ListBox3->Items->Strings[MCFAlgorithmForm->ListBox3->ItemIndex];

          PortContainerForm->Table1->FieldValues["SourcePoint"]= BlockStrS;
          PortContainerForm->Table1->FieldValues["DestPoint"]  = BlockStrD;

          int ReadyTime =  Crane_Buff[CN-1].Next_Generation_Time;
          Crane_Buff[CN-1].Next_Generation_Time = Quay_Crane_Time_Window + ReadyTime;

          Crane_Buff[CN-1].Number_Of_Jobs++;

          PortContainerForm->Table1->FieldValues["ReadyTime"]     = ReadyTime;
          PortContainerForm->Table1->FieldValues["QuayCraneTime"] = ReadyTime;
          PortContainerForm->Table1->FieldValues["Done"]          = false;

          PortContainerForm->Table1->Post();
          for(int r=0;r<Maximum_Container_Jobs;r++)
          {
              if(Container_Buff[r].Empty == 1)
              {
                 Container_Buff[r].Empty          = 0;
                 Container_Buff[r].IDStr          = ContainerIDStrS;
                 Container_Buff[r].SourcePointStr = BlockStrS;
                 Container_Buff[r].DestPointStr   = BlockStrD;
                 Container_Buff[r].ReadyTime      = ReadyTime;
                 Container_Buff[r].QCraneTime     = ReadyTime;
                 Container_Buff[r].Vehicle        = 0;

                 Job_Crane_Buff[CN-1].New_Jobs[Job_Crane_Buff[CN-1].Number_Of_New_Jobs] = r + 1;
                 Job_Crane_Buff[CN-1].Number_Of_New_Jobs++;
                 break;
              }
          }

       }
       Edit28->Text = "Containers Came";
       MCFAlgorithmForm->GroupBox16->Refresh();
       PortContainerForm->Table2->Refresh() ;
}
//------------------------------------------------------------------------------
void __fastcall  Write_Model_To_ListBox2()
{    // Static Fashion
     MCFAlgorithmForm->ListBox2->Items->Clear();
     char Fname[100];
     sprintf(&Fname[0],"%s",BenchOptionForm->Edit1->Text);
  //   FILE *fp = fopen("temp.inp","r+b"); // outstream  string
     FILE *fp = fopen(Fname,"r+b"); // outstream  string
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
  //   FILE *fp2 = fopen("temp.out","r+b");
     char Fname2[100];
     sprintf(&Fname2[0],"%s",BenchOptionForm->Edit2->Text);
     FILE *fp2 = fopen(Fname2,"r+b");
     while(1)
     {
         if(feof(fp2)) break;
         if(!fgets(buff,100,fp2)) break;
         MCFAlgorithmForm->ListBox5->Items->Add(buff);
     }
     fclose(fp2);
}
//------------------------------------------------------------------------------
void __fastcall  MCF_free(MCF_network_p net)
{
    // free((void *)net->arcs );
   //  free((void *)net->dummy_arcs );
     free((void *)net->nodes);
     net->nodes = net->stop_nodes = NULL;
     net->arcs  = net->stop_arcs = NULL;
     net->dummy_arcs = net->stop_dummy = NULL;
}
//------------------------------------------------------------------------------
void __fastcall TMCFAlgorithmForm::Timer1Timer(TObject *Sender)
{     // network Simplex Algrithm: making the model and solve it
     TLocateOptions Opts;

     int   Number_Of_AGVs, Number_Of_Container_Jobs, Number_Of_Nodes, Number_Of_ARCs=0;
     int   Max_Number_Of_Container_Jobs, Max_Number_Of_Nodes, Max_Number_Of_ARCs;

     int   ReadyTimeAGV;
     int   Tij, Number_ARCs;

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
         Edit3->Text    = OpenPortForm->Table2->FieldByName("NumberOfBlockYard")->AsString;
         Edit6->Text    = OpenPortForm->Table2->FieldByName("NumberOfWorkingPosition")->AsString;
         Edit4->Text    = OpenPortForm->Table2->FieldByName("NumberOfAGVs")->AsString;
         Number_Of_AGVs = PortAGVForm->Table2->RecordCount;
     } else
         return;

     PortLayoutForm->Table2->Filter   = AnsiString("Portname = '") + ListBox3->Items->Strings[ListBox3->ItemIndex] + "'";

     PortContainerForm->Table2->Filter= AnsiString("Portname = '") + ListBox3->Items->Strings[ListBox3->ItemIndex] + "'";

     PortAGVForm->Table2->Filter      = AnsiString("Portname = '") + ListBox3->Items->Strings[ListBox3->ItemIndex] + "'";


                                                   // Dynamic Flag
     Number_Of_Container_Jobs = Read_Container_Jobs(1);


     Number_ARCs  =  ( Number_Of_AGVs * Number_Of_Container_Jobs)
                  +  ( Number_Of_Container_Jobs * 2)
                  +    Number_Of_AGVs
                  +  Number_Of_Container_Jobs * (Number_Of_Container_Jobs-1);

     Number_Of_Nodes = Number_Of_AGVs + Number_Of_Container_Jobs*2 + 1;


     if(Number_Of_Container_Jobs*Number_Of_AGVs == 0 ) return;

     TCursor Save_Cursor = Screen->Cursor;
     Screen->Cursor = crHourGlass;    // Show hourglass cursor



     char Fname[100];
     sprintf(&Fname[0],"%s",BenchOptionForm->Edit1->Text);
     FILE *fp = fopen(Fname,"w+b"); // outstream  string

     if(RadioButton30->Checked || RadioButton22->Checked)  // network Simplex Algrithm
     {
        memset( (void *)(&net), 0, (size_t)(sizeof(MCF_network_t)) );

        long t, h, t2, h2, l, u, c,Max_Cost=0;
        register long i;
        MCF_arc_p arc;
        MCF_node_p node;
        long up, down;

        h  = Number_ARCs;
        t  = Number_Of_Nodes;
        /* The basis structure needs one artificial node, called the root. */
        net.n = (long)t;
        net.m = (long)h;

        net.nodes      = (MCF_node_p) calloc( t+1, sizeof(MCF_node_t) );
        net.arcs       = (MCF_arc_p)  calloc( h,   sizeof(MCF_arc_t) );
        net.dummy_arcs = (MCF_arc_p)  calloc( t,   sizeof(MCF_arc_t) );

        net.stop_nodes = net.nodes + (t+1); /* one artificial node! */
        net.stop_arcs  = net.arcs + h;
        net.stop_dummy = net.dummy_arcs + t;

        if( !( net.nodes && net.arcs && net.dummy_arcs ) )
        {
            Application->MessageBox("Not Enough Memory !! ", "Error", MB_OK);
            MCF_free( &net );
            return;
        }

        for( node = net.nodes, i = 0; i <= t; i++, node++ )
        {
             node->number = i;
             node->ident  = node->pre_ident = MCF_FIXED;
        }

        net.max_cost = 0;

        ListBox2->Items->Clear();
        ListBox5->Items->Clear();

        Str1 = "c  Port Name : " ; Str1 = Str1 + ListBox3->Items->Strings[ListBox3->ItemIndex];
        ListBox2->Items->Add(Str1);

        Str1 = "c  Number of Container Jobs : " ; Str1 = Str1 + IntToStr(Number_Of_Container_Jobs);
        ListBox2->Items->Add(Str1);

        Str1 = "c  Number of AGVs           : " ; Str1 = Str1 + IntToStr(Number_Of_AGVs);
        ListBox2->Items->Add(Str1);

        Str1 = "p min " ;  // define Min, number of nodes and arcs
        Str1 = Str1 + IntToStr(Number_Of_Nodes)+ "    " + IntToStr(Number_ARCs);
        if( MCFAlgorithmForm->RadioButton7->Checked)
            ListBox2->Items->Add(Str1);
        SinkNodeStr = IntToStr(Number_Of_Nodes);

        TimeAGVToC = (int *) malloc(sizeof(int)*Number_Of_AGVs*Number_Of_Container_Jobs);
        TimeCToC   = (int *) malloc(sizeof(int)*Number_Of_Container_Jobs*Number_Of_Container_Jobs);

        CostAGVToC = (int *) malloc(sizeof(int)*Number_Of_AGVs*Number_Of_Container_Jobs);
        CostCToC   = (int *) malloc(sizeof(int)*Number_Of_Container_Jobs*Number_Of_Container_Jobs);

        Route_Table   = (int *) malloc(sizeof(int)*(Number_ARCs+1));
        memset(Route_Table, 0 , (Number_ARCs+1));

        if(!CostCToC || !CostAGVToC || !TimeAGVToC || !TimeCToC || !Route_Table )
        {
            Application->MessageBox("Not Enough Memory !! ", "Error", MB_OK);
            MCF_free( &net );
            return;
        }

        Edit28->Text = "Making The Model";
        GroupBox16->Refresh();


        Str1 = "p min " ;  // define Min, number of nodes and arcs
        Str1 = Str1 + IntToStr(Number_Of_Nodes)+ "   " + IntToStr(Number_ARCs) ;
        if( MCFAlgorithmForm->RadioButton7->Checked)
            ListBox2->Items->Add(Str1);

        fprintf(fp,"%s \n", Str1);
        SinkNodeStr = IntToStr(Number_Of_Nodes);

        Append_Supply_Nodes(Number_Of_AGVs,fp,&net);

        Append_Demand_Node (Number_Of_AGVs,Number_Of_Nodes,fp,&net);

        arc = net.arcs;
        arc = Append_Arcs_From_AGVs_Nodes_To_Container_Nodes(Number_Of_AGVs,&Number_Of_ARCs,       //SF
                                       PortNameStr,Number_Of_Container_Jobs,fp,0,&net,arc);

        arc = Append_Arcs_From_AGV_Nodes_To_Sink_Node(Number_Of_AGVs,&Number_Of_ARCs,SinkNodeStr,fp,&net,arc);

        arc = Append_Arcs_From_CNode_To_CPNodes(Number_Of_AGVs,Number_Of_Nodes,&Number_Of_ARCs,fp,&net,arc);

        arc = Append_Arcs_From_CPNodes_To_Sink_Node(Number_Of_AGVs,Number_Of_Nodes,SinkNodeStr,&Number_Of_ARCs,fp,&net,arc);

        arc = Append_Arcs_From_CPNodes_To_Any_Other_CNodes(Number_Of_AGVs,Number_Of_Container_Jobs,
                                   &Number_Of_ARCs,PortNameStr,fp,0,&net,arc);
                                                                                  //SF
     }

     fclose(fp);

     if( MCFAlgorithmForm->RadioButton7->Checked )
         Write_Model_To_ListBox2();


     Edit28->Text = "Solving The Model";
     GroupBox16->Refresh();

     sprintf(&Fname[0],"%s",BenchOptionForm->Edit1->Text);
     char Fname2[100];
     sprintf(&Fname2[0],"%s",BenchOptionForm->Edit2->Text);

     int stat;

     if(RadioButton30->Checked || RadioButton22->Checked)
     {
        if (RadioButton30->Checked )
            net.algorithm_opt = 0;
        else if (RadioButton22->Checked )
             {
                 if(RadioButton20->Checked)
                    net.algorithm_opt = 1;
                 else
                    net.algorithm_opt = 2;
             }
#if defined MCF_DEBUG_4
    FILE *temp_out;

    temp_out = fopen( "tmp.tmp", "w" );
    fprintf(temp_out,"\n");
    fclose(temp_out);
#endif                                                // SF
        stat = MCF_NSA_Solve( 2,Fname,Fname2,&net,1,Number_Of_AGVs,0); // Start of Dynamic Fashion:
     } else
        stat = 0;

     switch(stat)
     {
        case -1 :
           Edit28->Text = "Not Enough Mempry";
           Label25->Caption = "Not Enough Memory !! ";
           Label25->Color = clRed;
           break;
        case  1 :
           Edit28->Text = "Invalid Model!!";
           Label25->Caption = " Inconsistent Model!! ";
           Label25->Color = clRed;
           break;
        case 0 :
           Edit28->Text = "Solution is Ready";
           Label25->Color = clGreen ;
           Label25->Caption = "Solution is Ready";

           if( MCFAlgorithmForm->RadioButton5->Checked && ( RadioButton30->Checked || RadioButton22->Checked))
               Write_Solution_To_ListBox5();

           if(RadioButton30->Checked || RadioButton22->Checked)
              Read_NSA_Solution(Fname2,Number_ARCs, Number_Of_AGVs, Number_Of_Nodes,0,Number_Of_Container_Jobs);
                                                                                //  StaticFlag = 0
     }

     Screen->Cursor = Save_Cursor; // always restore the cursor
     GroupBox16->Refresh();

}
//------------------------------------------------------------------------------

void __fastcall TMCFAlgorithmForm::Button9Click(TObject *Sender)
{       // Reset

       if (!PortContainerForm->Table2->RecordCount)
        return;
        
       DynamicSchedulingStarted=false;

       AllocateMemoryForNSA = false;

       free(TimeAGVToC);
       free(TimeCToC);
       free(CostAGVToC);
       free(CostCToC);
       free(Route_Table);

       MCF_free( &net );

       if(MCFAlgorithmForm->ListBox3->ItemIndex < 0 )
       {
          Application->MessageBox(" No port !! ", "Error", MB_OK);
          return;
       }

       Read_Port(ListBox3->Items->Strings[ListBox3->ItemIndex]);

         // Deleting container Jobs from Container Job Files
       TLocateOptions Opts;
       Set  <TLocateOption,0,1> flags;
       flags << loCaseInsensitive << loPartialKey;

       for(int r=0;r<Maximum_Container_Jobs;r++)
           Container_Buff[r].Empty = 1;

       for(int CN=1;CN<=Maximum_Number_Cranes;CN++)
       {
           Crane_Buff[CN-1].Number_Of_Jobs  = 0;
           Crane_Buff[CN-1].Number_Of_Done_Jobs = 0;
           Crane_Buff[CN-1].Next_Generation_Time= StrToInt(Edit23->Text);
           Crane_Buff[CN-1].Last_Completed_Time = StrToInt(Edit23->Text);

           Job_Crane_Buff[CN-1].Number_Of_Fulfilled = 0;
           Job_Crane_Buff[CN-1].Number_Of_New_Jobs  = 0;
       }
     /*
       PortContainerForm->Table2->Active = false;
       PortContainerForm->Table2->EmptyTable();
       PortContainerForm->Table2->Active = true;
      */
       while(PortContainerForm->Table2->RecordCount > 0)
       {
             PortContainerForm->Table2->First();
             PortContainerForm->Table2->Delete();
       }
     /*
       MCFAlgorithmForm->Table1->Active = false;
       MCFAlgorithmForm->Table1->EmptyTable();
       MCFAlgorithmForm->Table1->Active = true;
      */
       while(MCFAlgorithmForm->Table1->RecordCount > 0)
       {
             MCFAlgorithmForm->Table1->First();
             MCFAlgorithmForm->Table1->Delete();
       }

    /*
       DBChart2->Enabled = false;
       DBChart3->Enabled = false;

       MCFAlgorithmForm->Table2->Active = false;
       MCFAlgorithmForm->Table2->EmptyTable();
       MCFAlgorithmForm->Table2->Active = true;

       DBChart2->Enabled = true;
       DBChart3->Enabled = true;
     */
       while(MCFAlgorithmForm->Table2->RecordCount > 0)
       {
             MCFAlgorithmForm->Table2->First();
             MCFAlgorithmForm->Table2->Delete();
       }
       if (PortAGVForm->Table2->RecordCount > 0)
         PortAGVForm->Table2->First() ;

       for(int VN=1;VN<=PortAGVForm->Table2->RecordCount;VN++)
       {
           Vehicle_Buff[VN-1].Last_Completed_Time = 0;
           Vehicle_Buff[VN-1].Number_Of_Jobs      = 0;

           PortAGVForm->Table2->Edit() ;

         /*  MCFAlgorithmForm->Table4->Active = false;
           MCFAlgorithmForm->Table4->EmptyTable();
           MCFAlgorithmForm->Table4->Active = true;
          */
           while(MCFAlgorithmForm->Table4->RecordCount > 0)
           {
              MCFAlgorithmForm->Table4->First();
              MCFAlgorithmForm->Table4->Delete();
           }

           PortAGVForm->Table2->FieldValues["CostAGV"] = 0;
           PortAGVForm->Table2->FieldValues["Load"] = false;
           PortAGVForm->Table2->FieldValues["Status"] = "Idle" ;
           PortAGVForm->Table2->FieldValues["ReadyTime"] = 0;
           PortAGVForm->Table2->FieldValues["WaitedTime"] = 0;
           PortAGVForm->Table2->FieldValues["NumberOfContainers"] = 0;
           PortAGVForm->Table2->FieldValues["Lane"] = 0;
           PortAGVForm->Table2->FieldValues["TimeInLane"] = 0;
           PortAGVForm->Table2->FieldValues["NextLane"] = 0;
           PortAGVForm->Table2->Post() ;

           PortAGVForm->Table2->Next() ;
       }
       PortAGVForm->Table2->Refresh () ;

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

         Button9Click(Sender);
         
    /*	Hourbox->Visible =!Hourbox->Visible;
	Minutebox->Visible =!Minutebox->Visible;
	Image9->Visible =!Image9->Visible;
	Image11->Visible =!Image11->Visible;
	Image12->Visible =!Image12->Visible;
     */
}
//------------------------------------------------------------------------------
int  __fastcall TMCFAlgorithmForm::FindIdleCrane()
{
       for(int CN=1;CN<=Port_Buff.NumberOfWorkingPosition;CN++)
           if (Crane_Buff[CN-1].Number_Of_Jobs == 0)
               return(CN);

       return(0);
}
//------------------------------------------------------------------------------
int  __fastcall TMCFAlgorithmForm::FindIdleAGV()
{
       for(int VN=1;VN<=Port_Buff.NumberOfAGVs;VN++)
           if (Vehicle_Buff[VN-1].Number_Of_Jobs == 0)
               return(VN);

       return(0);
}
//------------------------------------------------------------------------------
void __fastcall Write_Jobs_And_Shcedule(FILE *Fout3)
{
       char Buff[100];
       fprintf(Fout3,"\n\nStatus of the system at Time:%d ",TimeValueInSecond);
       fprintf(Fout3,"\nJobs to be carried.\n");

       fprintf(Fout3,"\n%15.15s,%s","C-ID","\tSource\t\tDestination\tReady Time");
       fprintf(Fout3,"\n%s","--------------------------------------------------------------");
       for(int i=0;i<PortContainerForm->Table2->RecordCount;i++)
       {
            fprintf(Fout3,"\n%15.15s"  ,Container_Buff[i].IDStr);
            fprintf(Fout3,"\t\t%s",Container_Buff[i].SourcePointStr);
            fprintf(Fout3,"\t\t%s",Container_Buff[i].DestPointStr  );
            fprintf(Fout3,"\t\t%d",Container_Buff[i].ReadyTime     );
       }

       int Number_Of_AGVs = PortAGVForm->Table2->RecordCount;
       PortAGVForm->Table2->First() ;
       for(int m=1;m<=Number_Of_AGVs;m++)
       {
          fprintf(Fout3,"\n\nAGVNo");
          fprintf(Fout3,"\tAGVName");
          fprintf(Fout3,"\tSLoc");
          fprintf(Fout3,"\t\tRTime");
          fprintf(Fout3,"\tNJobs");
          fprintf(Fout3,"\tTTime");
          fprintf(Fout3,"\tStatus");
          fprintf(Fout3,"\tLoad");
          fprintf(Fout3,"\tWTime");
          fprintf(Fout3,"\tPLoc");
          fprintf(Fout3,"\t\tLane");
          fprintf(Fout3,"\tTInLane");
          fprintf(Fout3,"\tNexLane");
          fprintf(Fout3,"\n-----------------------------------------------------------------------------------------------------------------------");

          int        FieldName2 = PortAGVForm->Table2->FieldByName("AGVNumber")->Value;
          AnsiString FieldName3 = PortAGVForm->Table2->FieldByName("AGVName")->AsString;
          AnsiString FieldName4 = PortAGVForm->Table2->FieldByName("StartLocation")->AsString;
          int        FieldName5 = PortAGVForm->Table2->FieldByName("ReadyTime")->Value;
          int        FieldName6 = PortAGVForm->Table2->FieldByName("NumberOfContainers")->Value;
          int        FieldName7 = PortAGVForm->Table2->FieldByName("CostAGV")->Value;
          AnsiString FieldName8 = PortAGVForm->Table2->FieldByName("Status")->AsString;
          bool       FieldName9 = PortAGVForm->Table2->FieldByName("Load")->Value;
          int        FieldNameA = PortAGVForm->Table2->FieldByName("WaitedTime")->Value;
          AnsiString FieldNameC = PortAGVForm->Table2->FieldByName("PreviousLocation")->AsString;
          int        FieldNameD = PortAGVForm->Table2->FieldByName("Lane")->Value;
          int        FieldNameE = PortAGVForm->Table2->FieldByName("TimeInLane")->Value;
          int        FieldNameF = PortAGVForm->Table2->FieldByName("Nextlane")->Value;
          fprintf(Fout3,"\n%d"  ,FieldName2);
          fprintf(Fout3,"\t%s"  ,FieldName3);
          fprintf(Fout3,"\t%s"  ,FieldName4);
          fprintf(Fout3,"\t\t%d",FieldName5);
          fprintf(Fout3,"\t%d"  ,FieldName6);
          fprintf(Fout3,"\t%d"  ,FieldName7);
          fprintf(Fout3,"\t%s"  ,FieldName8);
          fprintf(Fout3,"\t%d"  ,FieldName9);
          fprintf(Fout3,"\t%d"  ,FieldNameA);
          fprintf(Fout3,"\t%s"  ,FieldNameC);
          fprintf(Fout3,"\t\t%d",FieldNameD);
          fprintf(Fout3,"\t%d"  ,FieldNameE);
          fprintf(Fout3,"\t%d"  ,FieldNameF);

          MCFAlgorithmForm->Table4->First();
          fprintf(Fout3,"\n\nJobs for Vehicle %d", m);
          fprintf(Fout3,"\n%s","C-ID"   );
          fprintf(Fout3,"\t%s"  ,"RTime"  );
          fprintf(Fout3,"\t%s"  ,"VTime"  );
          fprintf(Fout3,"\t%s"  ,"ATime"  );
          fprintf(Fout3,"\t%s"  ,"QTime"  );
          fprintf(Fout3,"\t%s"  ,"SPoint" );
          fprintf(Fout3,"\t\t%s","DPoint" );
          fprintf(Fout3,"\n-------------------------------------------------------------");

          while(!MCFAlgorithmForm->Table4->Eof)
          {
                AnsiString ContainerID = MCFAlgorithmForm->Table4->FieldByName("ContainerID")->AsString;
                int        ReadyTime   = MCFAlgorithmForm->Table4->FieldValues["ContainerReadyTime"];
                int        VehicleTime = MCFAlgorithmForm->Table4->FieldValues["VehicleTime"];
                int        ActualTime  = MCFAlgorithmForm->Table4->FieldValues["ActualTime"];
                int        QCraneTime  = MCFAlgorithmForm->Table4->FieldValues["QuayCraneTime"];
                AnsiString SourcePoint = MCFAlgorithmForm->Table4->FieldByName("SourcePoint")->AsString;
                AnsiString DestPoint   = MCFAlgorithmForm->Table4->FieldByName("DestPoint")->AsString;
                fprintf(Fout3,"\n%s"  ,ContainerID );
                fprintf(Fout3,"\t%d"  ,ReadyTime   );
                fprintf(Fout3,"\t%d"  ,VehicleTime );
                fprintf(Fout3,"\t%d"  ,ActualTime  );
                fprintf(Fout3,"\t%d"  ,QCraneTime  );
                fprintf(Fout3,"\t%s"  ,SourcePoint );
                fprintf(Fout3,"\t\t%s",DestPoint   );
                MCFAlgorithmForm->Table4->Next();
          }
          PortAGVForm->Table2->Next() ;
      }
}
//------------------------------------------------------------------------------
MCF_arc_p __fastcall Update_Arcs_From_AGVs_Nodes_To_Container_Nodes(int Number_Of_AGVs
                        ,int *Number_Of_ARCs      , AnsiString PortNameStr
                        ,int  Number_Container_Job, FILE *fp, int StaticFlag
                        ,MCF_network_p net,MCF_arc_p arc)
{
     int ReadyTimeAGV;
     int RecTable2, Distance ;
     int W1, W2;
     AnsiString Str1;

     float Penalty = Penalty1(PortNameStr);

     if(StaticFlag)
     {
        W1 = StrToFloat(MCFAlgorithmForm->Edit22->Text);
        W2 = StrToFloat(MCFAlgorithmForm->Edit2->Text);
     } else
     {
        W1 = StrToFloat(MCFAlgorithmForm->Edit22->Text); // 14->Text); //  28/04/2004
        W2 = StrToFloat(MCFAlgorithmForm->Edit2->Text);  //10->Text);  //  28/04/2004
     }

     MCF_node_p node;
     node = net->nodes;

     fprintf(fp, "c Update arcs from any AGV node to any container node, if it is compatible\n");
     MCFAlgorithmForm->Label25->Caption = "Arcs from AGV to Jobs";
     MCFAlgorithmForm->Refresh() ;
     for (int i1=1;i1<=Number_Of_AGVs;i1++)
     {
         MCFAlgorithmForm->ProgressBar1->Position = 100*i1/Number_Of_AGVs;
         int i2=Number_Of_AGVs+1;
         AnsiString  StartLocationAGVStr = Get_StartLocation_And_ReadyTime_AGV(PortNameStr,
                                            i1, &ReadyTimeAGV);
         RecTable2=1;
         int Number_Container_Job = (net->n -  Number_Of_AGVs - 1) / 2 ;
         int NoRecTable2 = Number_Container_Job; // PortContainerForm->Table2->RecordCount;
         for(int r=1;r<=NoRecTable2;r++)
         {
             if(Container_Buff[r-1].Empty == 1)
             {
                arc++;
                continue;
             }
             String ContainerIDStr          = Container_Buff[r-1].IDStr ;
             String SourcePointContainerStr = Container_Buff[r-1].SourcePointStr ;
             String DestPointContainerStr   = Container_Buff[r-1].DestPointStr;
             int ReadyTimeContainer         = Container_Buff[r-1].ReadyTime ;
             AnsiString PortNameIDStr       = PortNameStr;

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

             *(TimeAGVToC  + (i1-1)*Number_Container_Job + r-1) = ReadyTimeAGV + Distance + TimeValueInSecond; //28/04/04
             int DT1, DT2;
             // make a higher priority for for late jobs
            //  DT1 = ReadyTimeContainer - (Distance +  ReadyTimeAGV + TimeValueInSecond)
            //      - Late *(TimeValueInSecond- ReadyTimeContainer)
            //  if (DT1 < 0) DT1 = 0;

             DT1 = ReadyTimeContainer - (Distance +  ReadyTimeAGV + TimeValueInSecond);
             if( DT1 >= 0 )
             {
                DT1 = W1 * (ReadyTimeContainer - (Distance +  ReadyTimeAGV + TimeValueInSecond))
                    + W2 * (Distance +  ReadyTimeAGV ) ;
                if (TimeValueInSecond > ReadyTimeContainer)  // 30/04/04
                {
                      DT1 -= 2*Penalty *(TimeValueInSecond - ReadyTimeContainer);
                      if (DT1 < 0 ) DT1 = 0;
                }
                //    + ReadyTimeContainer ;
                //  + (ReadyTimeContainer - TimeValueInSecond)
                //  if (DT1 < 0 ) DT1 = 0;
                // define       AGV m     c      L   U     C
                Str1.printf("a    % 5.0d     % 5.0d     0    1    %10.1f", i1,i2,(float ) DT1);

                arc->cost = DT1;
                if(net->max_cost < DT1)
                {
                   net->max_cost = DT1;
                   net->max_cost_arc = arc;
                }
                arc++;

                (*Number_Of_ARCs)++;
                if( MCFAlgorithmForm->RadioButton7->Checked)
                    fprintf(fp,"%s  \n", Str1.c_str());
                #if defined TSP-PORT
                    DT1 = 1 + random(100);
                    Str1.printf("a    % 5.0d     % 5.0d     0    1    %10.1f", i1,i2,(float ) DT1);
                    fprintf(fp,"%s  \n", Str1.c_str());
                #endif
                *(CostAGVToC+(i1-1)*Number_Container_Job+ RecTable2-1) =  DT1;
             }
             else{
                DT2 = Penalty *(ReadyTimeAGV + Distance + TimeValueInSecond - ReadyTimeContainer);
                //   + W2 * Penalty *(Distance + ReadyTimeAGV );
                //    + ReadyTimeContainer ;

                if (TimeValueInSecond > ReadyTimeContainer)  // 28/04/04
                {
                      DT2 -= 2*Penalty *(TimeValueInSecond - ReadyTimeContainer);
                      if (DT2 < 0 ) DT2 = 0;
                }
                 // define arc AGV m     c     L    U    C
                Str1.printf("a    % 5.0d     % 5.0d     0    1    %10.1f", i1,i2,(float ) DT2);

                arc->cost = DT2;
                if(net->max_cost < DT2)
                {
                   net->max_cost = DT2;
                   net->max_cost_arc = arc;
                }
                arc++;

                (*Number_Of_ARCs)++;
                if( MCFAlgorithmForm->RadioButton7->Checked)
                    fprintf(fp,"%s  \n", Str1.c_str());
                #if defined TSP-PORT
                    DT1 = 1 + random(100);
                    Str1.printf("a    % 5.0d     % 5.0d     0    1    %10.1f", i1,i2,(float ) DT1);
                    fprintf(fp,"%s  \n", Str1.c_str());
                #endif
                *(CostAGVToC+(i1-1)*Number_Container_Job+ RecTable2-1) = DT2;
             }
             RecTable2++;
             i2 = i2 + 2;
         }
     }
     return(arc);
}
//------------------------------------------------------------------------------
int  __fastcall Update_Arcs_From_CPNodes_To_Any_Other_CNodes(int Number_Of_AGVs
                        ,int Number_Of_Container_Jobs, int *Number_Of_ARCs
                        ,AnsiString PortNameIDStr    , FILE *fp
                        ,int StaticFlag              , MCF_network_p net,MCF_arc_p arc
                        ,int RecTable2               , int RecTable3)
{
     int Tij ;
     int W1, W2;

     if (Number_Of_Container_Jobs <= 1) return(0);

     float Penalty = Penalty2(PortNameIDStr);

     if(StaticFlag)
     {
        W1 = StrToFloat(MCFAlgorithmForm->Edit22->Text);
        W2 = StrToFloat(MCFAlgorithmForm->Edit2->Text);
     } else
     {
        W1 = StrToFloat(MCFAlgorithmForm->Edit22->Text); // 14->Text); // 28/04/2004
        W2 = StrToFloat(MCFAlgorithmForm->Edit2->Text); // 10->Text);  // 28/04/2004
     }

     MCF_node_p node;
     node = net->nodes;

     AnsiString Str1;

     fprintf(fp, "c Update arcs from any c' node to any other c node\n");

     MCFAlgorithmForm->GroupBox15->Refresh() ;
     int NoRecTable3 = Number_Of_Container_Jobs; // PortContainerForm->Table2->RecordCount;

     MCFAlgorithmForm->Label25->Caption = "Arcs from c' to other c";

     int i2 = Number_Of_AGVs + 1;
     int i1 = Number_Of_AGVs + 1 + (RecTable2-1) * 2;

     AnsiString ContainerIDStr  = Container_Buff[RecTable2-1].IDStr ;
     //(Container_Buff[RecTable3-1].Empty == 1) return(0) ;
     AnsiString ContainerIDStr2 = Container_Buff[RecTable3-1].IDStr ;

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
           //   + ReadyTimeContainer2;
           //                 Agv m   C     L   U   C
           Str1.printf("a    %5d     %5d    0   1   %10.1f", i1,i2,(float ) DT1 );

           arc->cost = DT1;
           if(net->max_cost < DT1)
           {
              net->max_cost = DT1;
              net->max_cost_arc = arc;
           }
           if( MCFAlgorithmForm->RadioButton7->Checked)
              fprintf(fp,"%s  \n", Str1.c_str());
           #if defined TSP-PORT
               DT1 = 1 + random(100);
               Str1.printf("a    % 5.0d     % 5.0d     0    1    %10.1f", i1,i2,(float ) DT1);
               fprintf(fp,"%s  \n", Str1.c_str());
           #endif
           *(CostCToC + (RecTable2-1)*Number_Of_Container_Jobs + RecTable3-1) = DT1;
        }else
        {
           *Number_Of_ARCs = *Number_Of_ARCs + 1;
           DT2 = Penalty * (ReadyTimeContainer + Tij - ReadyTimeContainer2);
           //    + W2 * Penalty * Tij;
           //    + ReadyTimeContainer;
           //     - Late *(TimeValueInSecond - ReadyTimeContainer2)
           // if (DT2 < 0) DT2 = 0;
           //                 Agv m   C     L   U   C
           Str1.printf("a    %5d     %5d    0   1   %10.1f", i1,i2,(float ) DT2 );

           arc->cost = DT2;
           if(net->max_cost < DT2)
           {
              net->max_cost = DT2;
              net->max_cost_arc = arc;
           }

           if( MCFAlgorithmForm->RadioButton7->Checked)
               fprintf(fp,"%s  \n", Str1.c_str());
           #if defined TSP-PORT
               DT1 = 1 + random(100);
               Str1.printf("a    % 5.0d     % 5.0d     0    1    %10.1f", i1,i2,(float ) DT1);
               fprintf(fp,"%s  \n", Str1.c_str());
           #endif
           *(CostCToC + (RecTable2-1)*Number_Of_Container_Jobs + RecTable3-1) = DT2;
        }
        return(1);
     }

     return(0);

}
//------------------------------------------------------------------------------
void __fastcall Update_Previous_Solution()
{
     MCF_arc_p arc;
     MCF_node_p node;

     int Number_Of_AGVs = PortAGVForm->Table2->RecordCount;

     int Number_Of_Container_Jobs = (net.n -  Number_Of_AGVs - 1) / 2;

     int Number_ARCs  =  ( Number_Of_AGVs * Number_Of_Container_Jobs)
                      +  ( Number_Of_Container_Jobs * 2)
                      +    Number_Of_AGVs
                      +    Number_Of_Container_Jobs * (Number_Of_Container_Jobs-1);

     int Number_Of_Nodes = Number_Of_AGVs + Number_Of_Container_Jobs*2 + 1;

     for(int CN=1;CN<=Port_Buff.NumberOfWorkingPosition;CN++)
         for(int j=1;j<=Job_Crane_Buff[CN-1].Number_Of_Fulfilled;j++)
            if(Job_Crane_Buff[CN-1].Flag[j-1] == 1)
            {
                int j2= Job_Crane_Buff[CN-1].Deleted_Jobs[j-1] - 1;
                int VN = Container_Buff[j2].Vehicle;
                int I1 = VN;
                int JN = *(Route_Table + VN);
                while(JN != Number_Of_Nodes)
                {
                  if(!JN ) break; // DNSA
                  if( JN == j2 )
                  {
                     int JNP = *(Route_Table + JN);
                     int I2  = *(Route_Table + JNP);
                     *(Route_Table + I1) = *(Route_Table + I2);
                     // Set arc from I1 to I2
                     arc = net.arcs + Number_Of_AGVs * Number_Of_Container_Jobs
                                    + Number_Of_AGVs
                                    + Number_Of_Container_Jobs
                                    + Number_Of_Container_Jobs
                                    + I1 * (Number_Of_Container_Jobs-1)
                                    + I2 - 1;

                     arc->flow = 1;
                     break;
                  }
                  I1 = JN;
                  JN = *(Route_Table + JN);
                }
            }
}
//------------------------------------------------------------------------------
void __fastcall TMCFAlgorithmForm::RepairGraphModel(TObject *Sender)
{
     TLocateOptions Opts;

     int   Number_Of_AGVs, Number_Of_Container_Jobs, Number_Of_Nodes, Number_Of_ARCs=0;
     int   Max_Number_Of_Container_Jobs, Max_Number_Of_Nodes, Max_Number_Of_ARCs;

     int   ReadyTimeAGV, Number_Of_Fulfilled_Jobs = 0,Number_Of_New_Jobs = 0;
     int   Tij, Number_ARCs;

     MCF_arc_p arc;
     MCF_node_p node;

     if(MCFAlgorithmForm->ListBox3->ItemIndex < 0) return;

     AnsiString PortNameStr, Str1 , Str2, SinkNodeStr;
     AnsiString DistanceStr;
     Variant locvalues[2];

     PortNameStr = MCFAlgorithmForm->ListBox3->Items->Strings[MCFAlgorithmForm->ListBox3->ItemIndex];
     SinkNodeStr = IntToStr(net.n);
     Number_Of_AGVs = PortAGVForm->Table2->RecordCount;

     TCursor Save_Cursor = Screen->Cursor;
     Screen->Cursor = crHourGlass;    // Show hourglass cursor


     char Fname[100];
     sprintf(&Fname[0],"%s",BenchOptionForm->Edit1->Text);
     FILE *fp = fopen(Fname,"w+b"); // outstream  string

     Number_Of_Container_Jobs = (net.n -  Number_Of_AGVs - 1) / 2;

     Number_ARCs  =  ( Number_Of_AGVs * Number_Of_Container_Jobs)
                  +  ( Number_Of_Container_Jobs * 2)
                  +    Number_Of_AGVs
                  +    Number_Of_Container_Jobs * (Number_Of_Container_Jobs-1);

     Number_Of_Nodes = Number_Of_AGVs + Number_Of_Container_Jobs*2 + 1;

     MCFAlgorithmForm->Edit28->Text = "Making The Model";
     MCFAlgorithmForm->GroupBox16->Refresh();

#if defined MCF_DEBUG_4
    FILE *temp_out;

    temp_out = fopen( "tmp.tmp", "a" );
    fprintf(temp_out,"The current solution\n");
    fclose(temp_out);
    MCF_write_intermediate2(&net);
    MCF_write_intermediate3(&net);
#endif

     // Deleting the fulfilled Jobs
     for(int CN=1;CN<=Port_Buff.NumberOfWorkingPosition;CN++)
     {
         Number_Of_Fulfilled_Jobs += Job_Crane_Buff[CN-1].Number_Of_Fulfilled;
         for(int j=1;j<=Job_Crane_Buff[CN-1].Number_Of_Fulfilled;j++)
         {
            int j2= Job_Crane_Buff[CN-1].Deleted_Jobs[j-1] - 1;

            Job_Crane_Buff[CN-1].Flag[j-1] = 1;

            node = net.nodes + Number_Of_AGVs + j2 * 2 + 1;
            node->pre_ident  = node->ident;
            node->ident  = MCF_DELETED;
            node++;
            node->pre_ident  = node->ident;
            node->ident  = MCF_DELETED;

            arc = net.dummy_arcs + Number_Of_AGVs + j2 * 2 ; // 0 To J
            arc->pre_ident = arc->ident;
            arc->ident = MCF_DELETED;
            arc++; // arc + j2 + 1 + Number_Of_AGVs ;
            arc->pre_ident = arc->ident;
            arc->ident = MCF_DELETED;        // J' to 0

            for(int VN=1;VN<=Number_Of_AGVs;VN++)
            {
               arc = net.arcs + j2 + (VN-1)*Number_Of_Container_Jobs;
               // AGV node to J
               arc->pre_ident = arc->ident;
               arc->ident = MCF_DELETED;
            //   if(arc->flow)
             //  {
                  //arc = net.arcs + Number_Of_AGVs * Number_Of_Container_Jobs + Number_Of_AGVs + Number_Of_Container_Jobs + j2;
             //  }

            }

            arc = net.arcs + Number_Of_AGVs * Number_Of_Container_Jobs + Number_Of_AGVs + j2;
            arc->pre_ident = arc->ident;
            arc->ident = MCF_DELETED; // J to J'

            arc = net.arcs + Number_Of_AGVs * Number_Of_Container_Jobs + Number_Of_AGVs + Number_Of_Container_Jobs + j2;
            arc->pre_ident = arc->ident;
            arc->ident = MCF_DELETED; // J' To Sink Node


            for(int JN=1;JN<=Number_Of_Container_Jobs;JN++)
            {
               if( j2 + 1 == JN ) continue;
               arc = net.arcs + Number_Of_AGVs * Number_Of_Container_Jobs
                              + Number_Of_AGVs
                              + Number_Of_Container_Jobs
                              + Number_Of_Container_Jobs
                              + j2 * (Number_Of_Container_Jobs-1)
                              + JN - 1;
               if( j2 + 1 < JN )
                   arc--;
               // J' to Others
               arc->pre_ident = arc->ident;
               arc->ident = MCF_DELETED;

               arc = net.arcs + Number_Of_AGVs * Number_Of_Container_Jobs
                              + Number_Of_AGVs
                              + Number_Of_Container_Jobs
                              + Number_Of_Container_Jobs
                              + (JN-1) * (Number_Of_Container_Jobs-1)
                              + j2 ;
               if( JN  < j2 + 1)
                   arc--;
               if(Container_Buff[JN-1].Empty == 0  && Container_Buff[j2].Empty == 1)
               {
                  arc->pre_ident = arc->ident;
                  arc->ident = MCF_DELETED;
               }
            }
         }
     }

#if defined MCF_DEBUG_4
     temp_out = fopen( "tmp.tmp", "a" );
     fprintf(temp_out,"After Deleted Arcs\n");
     fclose(temp_out);
     MCF_write_intermediate2(&net);
     MCF_write_intermediate3(&net);
#endif

     arc = net.arcs;  // RepairGraphModel
     arc = Update_Arcs_From_AGVs_Nodes_To_Container_Nodes(Number_Of_AGVs,&Number_Of_ARCs,       //SF
                                    PortNameStr,Number_Of_Container_Jobs,fp,0,&net,arc);

     // Inserting New Jobs
     for(int CN=1;CN<=Port_Buff.NumberOfWorkingPosition;CN++)
     {
         Number_Of_New_Jobs += Job_Crane_Buff[CN-1].Number_Of_New_Jobs;
         for(int j=1;j<=Job_Crane_Buff[CN-1].Number_Of_New_Jobs;j++)
         {
            int j2= Job_Crane_Buff[CN-1].New_Jobs[j-1] - 1;
            Job_Crane_Buff[CN-1].Flag[j-1] = 0;

            node = net.nodes + Number_Of_AGVs + j2 * 2 + 1;
            if(node->pre_ident != MCF_DELETED && node->ident == MCF_DELETED )
               node->ident  = MCF_FIXED  ;
            else
               node->ident  = MCF_INSERTED;

            node++;
            if(node->pre_ident != MCF_DELETED && node->ident == MCF_DELETED )
               node->ident  = MCF_FIXED  ;
            else
               node->ident  = MCF_INSERTED;

            arc = net.dummy_arcs + Number_Of_AGVs + j2 * 2 ; // 0 To J
            if(arc->pre_ident != MCF_DELETED && arc->ident == MCF_DELETED )
               arc->ident = arc->pre_ident;
            else
            //   arc->ident = MCF_AT_LOWER;
                 arc->ident = MCF_INSERTED;

            arc++; // arc + j2 + 1 + Number_Of_AGVs ;
            if(arc->pre_ident != MCF_DELETED && arc->ident == MCF_DELETED )
               arc->ident = arc->pre_ident;
            else
              // arc->ident = MCF_AT_LOWER;
               arc->ident = MCF_INSERTED;       // J' to 0

            for(int VN=1;VN<=Number_Of_AGVs;VN++)
            {
               arc = net.arcs + j2 + (VN-1)*Number_Of_Container_Jobs;
               // AGV node to J
               if(arc->pre_ident != MCF_DELETED && arc->ident == MCF_DELETED )
                  arc->ident = arc->pre_ident;
               else
               //   arc->ident = MCF_AT_LOWER;
                  arc->ident = MCF_INSERTED;
            }

            arc = net.arcs + Number_Of_AGVs * Number_Of_Container_Jobs + Number_Of_AGVs + j2;
            if(arc->pre_ident != MCF_DELETED && arc->ident == MCF_DELETED )
               arc->ident = arc->pre_ident;
            else
               //arc->ident = MCF_AT_LOWER;
               arc->ident = MCF_INSERTED; // J to J'

            arc = net.arcs + Number_Of_AGVs * Number_Of_Container_Jobs + Number_Of_AGVs + Number_Of_Container_Jobs + j2;
            if(arc->pre_ident != MCF_DELETED && arc->ident == MCF_DELETED )
               arc->ident = arc->pre_ident;
            else
               //arc->ident = MCF_AT_LOWER;
               arc->ident = MCF_INSERTED; // J' To Sink Node


            for(int JN=1;JN<=Number_Of_Container_Jobs;JN++)
            {
               if( j2 + 1 == JN ) continue;
               arc = net.arcs + Number_Of_AGVs * Number_Of_Container_Jobs
                              + Number_Of_AGVs
                              + Number_Of_Container_Jobs
                              + Number_Of_Container_Jobs
                              + j2 * (Number_Of_Container_Jobs-1)
                              + JN - 1;
               if( j2 + 1 < JN )
                   arc--;

               if(Container_Buff[JN-1].Empty == 1 )
               {
                  arc->pre_ident = arc->ident;
                  arc->ident     = MCF_DELETED;
                  continue;
               }
               // to J' to Others
               Update_Arcs_From_CPNodes_To_Any_Other_CNodes(Number_Of_AGVs,Number_Of_Container_Jobs,
                              &Number_Of_ARCs,PortNameStr,fp,0,&net,arc, j2 + 1, JN);
               if(arc->pre_ident != MCF_DELETED && arc->ident == MCF_DELETED )
                  arc->ident = arc->pre_ident;
               else
                  //arc->ident = MCF_AT_LOWER;
                  arc->ident = MCF_INSERTED;
            }
         }
     }
     fclose(fp);

#if defined MCF_DEBUG_4
    temp_out = fopen( "tmp.tmp", "a" );
    fprintf(temp_out,"After Inserted Jobs\n");
    fclose(temp_out);
    MCF_write_intermediate2(&net);
    MCF_write_intermediate3(&net);
#endif

     int stat, balance=0;
     if( Number_Of_Fulfilled_Jobs != Number_Of_New_Jobs )
         Update_Previous_Solution();
     else
         balance = 1;

     MCFAlgorithmForm->Edit28->Text = "Solving The Model";
     MCFAlgorithmForm->GroupBox16->Refresh();

     sprintf(&Fname[0],"%s",BenchOptionForm->Edit1->Text);
     char Fname2[100];
     sprintf(&Fname2[0],"%s",BenchOptionForm->Edit2->Text);


     stat = MCF_NSA_Solve( 2,Fname,Fname2,&net,0,Number_Of_AGVs, balance); // Dynamic Fashion : SF = 0

     if(!stat)
     {
        MCFAlgorithmForm->Edit28->Text = "Solution is Ready";
        MCFAlgorithmForm->Label25->Color = clGreen ;
        MCFAlgorithmForm->Label25->Caption = "Solution is Ready";

        if(MCFAlgorithmForm->RadioButton30->Checked || MCFAlgorithmForm->RadioButton22->Checked)
           Read_NSA_Solution(Fname2,Number_ARCs, Number_Of_AGVs, Number_Of_Nodes,0,Number_Of_Container_Jobs);
                                                                               //  StaticFlag = 0
     }
     Screen->Cursor = Save_Cursor; // Always restore the cursor
     MCFAlgorithmForm->GroupBox16->Refresh();
}
//------------------------------------------------------------------------------
void __fastcall TMCFAlgorithmForm::Timer3Timer(TObject *Sender)
{
      static TDateTime PreviousDateTime;
      int Time_Window_Vehicle = StrToInt(MCFAlgorithmForm->Edit30->Text);
      TDateTime DateTime = Time();  // store the current date and time
      AnsiString TimeStr = TimeToStr(DateTime); // convert the time to a string

      if(MCFAlgorithmForm->ListBox3->ItemIndex < 0 )
      {
        Application->MessageBox(" No port !! ", "Error", MB_OK);
        Button8Click(Sender); // Stop
        // Timer3->Enabled = false;
        return;
      }

      Edit25->Text = TimeStr;
      FILE *fp3;


      if(TimeValueInSecond == 0 && BenchOptionForm->CheckBox1->Checked == true)
      {
          char   Fname3[100], Buff[100];
          sprintf(&Fname3[0],"%s",BenchOptionForm->Edit3->Text);
          Port_Buff.Fout3   = fopen(Fname3,"a+t");
      }

      TimeValueInSecond += 1;

      Edit27->Text = IntToStr(TimeValueInSecond);

      AnsiString PortNameStr = MCFAlgorithmForm->ListBox3->Items->Strings[MCFAlgorithmForm->ListBox3->ItemIndex];

      TBookmark Bookmark =PortAGVForm->Table2->GetBookmark ()  ;

      int Number_Of_AGVs = PortAGVForm->Table2->RecordCount;
      int Vehicle_No = 1;
      PortAGVForm->Table2->First() ;
      while(!PortAGVForm->Table2->Eof )
      {
           MCFAlgorithmForm->Table4->First() ;
           Set_Location_And_ReadyTime_AGV(PortNameStr,Vehicle_No,TimeValueInSecond,DynamicSchedulingStarted);
           PortAGVForm->Table2->Next() ;
           Vehicle_No++;
      }

      Delete_Processed_Container_Jobs(TimeValueInSecond,DynamicSchedulingStarted);

      int Crane_Number = FindIdleCrane();
      if (Crane_Number)
      {
          Insert_Containers_To_Table(Crane_Number); // Generate Container

          if(RadioButton30->Checked || RadioButton22->Checked) // Network Simplex Algorithm
          {
            Update_Cranes_And_Vehicles();
            Initialize_New_Schedule(PortNameStr);

            RepairGraphModel(Sender);  // Inserting/removing/updating the job/node/arcs in the graph
            Button2Click(Sender); // Suspend for Test.

            if(BenchOptionForm->CheckBox1->Checked)
               Write_Jobs_And_Shcedule(Port_Buff.Fout3);
            if(MCFAlgorithmForm->CheckBox1->Checked)
            {
               GroupBox16->Refresh();
               Button2Click(Sender); // Suspend
            }
          }
      }
      else if(RadioButton31->Checked)
      {
         int Number_Of_Container_Jobs = Read_Container_Jobs(1);
         TBookmark Bookmark2 = PortAGVForm->Table2->GetBookmark ()  ;
         int RV = Prime_Mover(Number_Of_AGVs,PortNameStr,Number_Of_Container_Jobs,Port_Buff.Fout3,0);
         PortAGVForm->Table2->GotoBookmark(Bookmark2);
         if(MCFAlgorithmForm->CheckBox1->Checked && RV)
         {
               GroupBox16->Refresh();
               Button2Click(Sender);// Suspend
         }
      }

      PortAGVForm->Table2->GotoBookmark(Bookmark);

      if(PreviousDateTime != DateTime)
      {
         Painta();
         PreviousDateTime = DateTime;
      }
      if(RadioButton15->Checked)
         Timer4Timer(Sender);
      else if(RadioButton16->Checked && (TimeValueInSecond % 30 == 0) )
              Timer4Timer(Sender);
          else if(RadioButton17->Checked && (TimeValueInSecond % 60 == 0) )
                  Timer4Timer(Sender);

      DynamicSchedulingStarted = true;
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
       case 5 : // Heuristic
                w1 = GroupBox30->Width ;
                l1 = GroupBox30->Left ;
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
    GroupBox30->Width = w2;
    GroupBox30->Left = l2;


    GroupBox14->Width = w1;
    GroupBox14->Left = l1;
    switch(SwapPanel)
    {
       case 0 :    DBGrid4->Width = 409; // 305;
                   DBGrid3->Width = 409; // 305;
                   ListBox2->Width = 705;  // 678; // 551;
                   ListBox5->Width = 705; // 678; // 551;
                   break;
       case 1 :    DBGrid4->Width = 705; // 678; // 605;
                   DBGrid3->Width = 705; // 678; // 605;
                   ListBox2->Width = 409; // 305;
                   ListBox5->Width = 409; // 305;
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

//     FILE *fp = fopen("temp.inp","r+"); // outstream  string
     char Fname[100];
     sprintf(&Fname[0],"%s",BenchOptionForm->Edit1->Text);
     FILE *fp = fopen(Fname,"r+"); // outstream  string

     AnsiString Str1 = "p min " ;  // define Min, number of nodes and arcs
     Str1 = Str1 + IntToStr(Number_Of_Nodes)+ "     "  + IntToStr(Number_Of_ARCs) ;
     fprintf(fp,"%s     ", Str1.c_str());
     fclose(fp);
}
//------------------------------------------------------------------------------
void __fastcall TMCFAlgorithmForm::Intialize_AGV_SA()
{
   int Number_Of_AGVs = PortAGVForm->Table2->RecordCount;
   PortAGVForm->Table2->First() ;
   for(int VN=1;VN<=Number_Of_AGVs;VN++)
   {
      String AGVLocation  = PortAGVForm->Table2->FieldValues["StartLocation"];
      AGV [VN-1].StartLoc  = AGVLocation;
      AGV [VN-1].Capacity  = CAPACITY;
      PortAGVForm->Table2->Next() ;
   }
}
//------------------------------------------------------------------------------
void __fastcall TMCFAlgorithmForm::Intialize_SA_By_NSA_Solution()
{      // From Network Simplex algorithm
   int Number_Of_AGVs = PortAGVForm->Table2->RecordCount;
   PortAGVForm->Table2->First() ;
   for(int VN=1;VN<=Number_Of_AGVs;VN++)
   {
      TAGV[VN-1].Length = 0;

      MCFAlgorithmForm->Table4->First();
      for(int j=0;j<MCFAlgorithmForm->Table4->RecordCount;j++)
      {
        String ContainerID = MCFAlgorithmForm->Table4->FieldValues["ContainerID"];
        int    ReadyTime   = MCFAlgorithmForm->Table4->FieldValues["ContainerReadyTime"];
        int    JobNo       = MCFAlgorithmForm->Table4->FieldValues["JobNo"];
        String SourcePoint = MCFAlgorithmForm->Table4->FieldValues["SourcePoint"];
        String DestinPoint = MCFAlgorithmForm->Table4->FieldValues["DestPoint"];
        int    VehicleTime = MCFAlgorithmForm->Table4->FieldValues["VehicleTime"];
        int    ActualTime  = MCFAlgorithmForm->Table4->FieldValues["ActualTime"];
        int    QCraneTime  = MCFAlgorithmForm->Table4->FieldValues["QuayCraneTime"];


        TAGV[VN-1].PortTrip[TAGV[VN-1].Length].Load      = ONE;
        TAGV[VN-1].PortTrip[TAGV[VN-1].Length].Contents[0].Action = PICKUP;
        TAGV[VN-1].PortTrip[TAGV[VN-1].Length].Contents[0].JobNo  = JobNo;
        TAGV[VN-1].PortTrip[TAGV[VN-1].Length].LocToVisit = SourcePoint;

        if(SourcePoint.SubString(1,3) == "W/P" )
        {
           TAGV[VN-1].PortTrip[TAGV[VN-1].Length].VehicleTime = VehicleTime;
           TAGV[VN-1].PortTrip[TAGV[VN-1].Length].ActualTime  = ActualTime;
           CJob[JobNo-1].QCraneTime      = QCraneTime;
        } else
        {
           if(TAGV[VN-1].Length == 0)
              TAGV[VN-1].PortTrip[TAGV[VN-1].Length].VehicleTime = Get_Time_Between_Two_Points(Port_Buff.Port,
              AGV [VN-1].StartLoc,TAGV[VN-1].PortTrip[0].LocToVisit);
           else
              TAGV[VN-1].PortTrip[TAGV[VN-1].Length].VehicleTime = Get_Time_Between_Two_Points(Port_Buff.Port,
              TAGV[VN-1].PortTrip[TAGV[VN-1].Length-1].LocToVisit,
              TAGV[VN-1].PortTrip[TAGV[VN-1].Length  ].LocToVisit   ) +
              TAGV[VN-1].PortTrip[TAGV[VN-1].Length-1].ActualTime     ;

           TAGV[VN-1].PortTrip[TAGV[VN-1].Length].ActualTime =  \
           TAGV[VN-1].PortTrip[TAGV[VN-1].Length].VehicleTime   ;
        }
        TAGV[VN-1].Length++;

        TAGV[VN-1].PortTrip[TAGV[VN-1].Length].Load      = ZERO;
        TAGV[VN-1].PortTrip[TAGV[VN-1].Length].Contents[0].JobNo = JobNo;
        TAGV[VN-1].PortTrip[TAGV[VN-1].Length].Contents[0].Action= DELIVER;
        TAGV[VN-1].PortTrip[TAGV[VN-1].Length].LocToVisit = DestinPoint  ;

        if(DestinPoint.SubString(1,3) == "W/P" )
        {
           TAGV[VN-1].PortTrip[TAGV[VN-1].Length].VehicleTime = VehicleTime;
           TAGV[VN-1].PortTrip[TAGV[VN-1].Length].ActualTime  = ActualTime;
           CJob[JobNo-1].QCraneTime      = QCraneTime;
        } else
        {
           if(TAGV[VN-1].Length == 0)
              TAGV[VN-1].PortTrip[TAGV[VN-1].Length].VehicleTime = Get_Time_Between_Two_Points(Port_Buff.Port,
              AGV [VN-1].StartLoc,TAGV[VN-1].PortTrip[0].LocToVisit);
           else
              TAGV[VN-1].PortTrip[TAGV[VN-1].Length].VehicleTime =      \
                       Get_Time_Between_Two_Points(Port_Buff.Port,      \
              TAGV[VN-1].PortTrip[TAGV[VN-1].Length-1].LocToVisit,      \
              TAGV[VN-1].PortTrip[TAGV[VN-1].Length  ].LocToVisit   ) + \
              TAGV[VN-1].PortTrip[TAGV[VN-1].Length-1].ActualTime       ;

           TAGV[VN-1].PortTrip[TAGV[VN-1].Length].ActualTime =   \
           TAGV[VN-1].PortTrip[TAGV[VN-1].Length].VehicleTime    ;
        }
        TAGV[VN-1].Length++;
        MCFAlgorithmForm->Table4->Next();
      }
      PortAGVForm->Table2->Next() ;
   }
}
//------------------------------------------------------------------------------
void __fastcall TMCFAlgorithmForm::Handle_Multi_Load_AGVS()
{
     char   *filePara,*filenameIn,*filenameOut,*fileBestTour,*fileBestXls ;

     filePara     = new char[MAXChar];
     filenameIn   = new char[MAXChar];
     fileBestTour = new char[MAXChar];
     fileBestXls  = new char[MAXChar];
     filenameOut  = new char[MAXChar];

 /*    sprintf(filenameIn,"%s",MCFAlgorithmForm->Edit39->Text);
     ifstream fIn(filenameIn);
   */
     sprintf(filenameOut,"%s",BenchOptionForm->Edit2->Text);
     ofstream fOut(filenameOut);

     sprintf(fileBestTour,"%s","BestSNA.txt"); //MCFAlgorithmForm->Edit41->Text);
     ofstream fOutB(fileBestTour);

     sprintf(fileBestXls,"%s","BestXSNA.txt"); //_MCFAlgorithmForm->Edit42->Text);
     ofstream fOutX(fileBestXls);

     char tmpbuf[128];
     _strtime( tmpbuf );
     fOut<<"\nOS time:\t\t"<<tmpbuf<<"\n";

     SAAVRP VRP_2;

     VRP_2.PortName = MCFAlgorithmForm->ListBox1->Items->Strings[MCFAlgorithmForm->ListBox1->ItemIndex];
     VRP_2.tInitial = StrToFloat(MCFAlgorithmForm->Edit46->Text); //4;
     VRP_2.tEnd     = StrToFloat(MCFAlgorithmForm->Edit47->Text);// 1;
     VRP_2.rCooling = StrToFloat(MCFAlgorithmForm->Edit48->Text);// 0.999;

     VRP_2.MaxTChange    = StrToInt(MCFAlgorithmForm->Edit49->Text); // 1000;
     VRP_2.MaxIterations = StrToInt(MCFAlgorithmForm->Edit53->Text); // 100000;

  //   VRP_2.dLimit = StrToInt(MCFAlgorithmForm->Edit50->Text); //10;
     VRP_2.sLimit = StrToInt(MCFAlgorithmForm->Edit51->Text);// 3;

   //  VRP_2.criticalRatio = StrToFloat(MCFAlgorithmForm->Edit52->Text); // 0.01;

     VRP_2.weight1 = StrToInt  (MCFAlgorithmForm->Edit2->Text); // Travelling Time1;
     VRP_2.weight2 = StrToFloat(MCFAlgorithmForm->Edit22->Text); // Waiting time , 0.5;
     VRP_2.weight3 = Penalty1(Port_Buff.Port);                  // Penalty for Late, 10;

     VRP_2.TWindowVehicle = StrToFloat(MCFAlgorithmForm->Edit30->Text);

     VRP_2.ReadData();
     Intialize_AGV_SA();
     if(MCFAlgorithmForm->RadioButton27->Checked )
     {
        Intialize_SA_By_NSA_Solution();
        fOut<<"\nStarting SA With result From NSA:";
     } else if(MCFAlgorithmForm->RadioButton24->Checked )
             VRP_2.InitialTour1(fOut); // Constructive Random Initial Solution
          else if(MCFAlgorithmForm->RadioButton25->Checked )
                 VRP_2.InitialTour2(fOut); // Random initial feasible solution
               else if(MCFAlgorithmForm->RadioButton29->Checked )
                       VRP_2.InitialTour11(fOut);// Deterministic Initial Feasible Solution
                    else if(MCFAlgorithmForm->RadioButton26->Checked )
                            VRP_2.InitialTour3(fOut);

     VRP_2.CalcValues(TAGV);
     VRP_2.PrintInfo(fOut,TAGV);
     double IVOF = VRP_2.ObjectiveFunction(TAGV);
     fOut<<"\n Objective Value = "<<IVOF;
     fOut.flush ();
     MCFAlgorithmForm->Edit50->Text = FloatToStr(IVOF);

     VRP_2.SimulatedAnnealing(fOut,fOutB,fOutX);

     double FVOF = VRP_2.ObjectiveFunction(BestT);
     if(FVOF < IVOF)
        MCFAlgorithmForm->Edit52->Text = FloatToStr(FVOF);
     else
        MCFAlgorithmForm->Edit52->Text = FloatToStr(FVOF);

     VRP_2.DeleteMemory();

     _strtime( tmpbuf );
     fOut<<"\n\nOS time:\t\t"<<tmpbuf<<"\n";

     fOut.close();
     fOutB.close();
     fOutX.close();
  //   fIn.close();
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

       static int Next_Quay_Crane = 1;
       if (DynamicSchedulingStarted) Next_Quay_Crane = 1;

       long int BaseTime = TimeValueInSecond + GlobalInitialTimeQuayCrane;

       int Quay_Crane_Time_Window = StrToInt(MCFAlgorithmForm->Edit26->Text);
       int Number_Of_Cranes       = StrToInt(MCFAlgorithmForm->Edit6->Text);
       int Number_Of_Vehicles     = StrToInt(MCFAlgorithmForm->Edit4->Text);

       for(int CN=1;CN<=Number_Of_Cranes;CN++)
       {
           Crane_Buff[CN-1].Number_Of_Done_Jobs = 0;// reset button
           Crane_Buff[CN-1].Next_Generation_Time= StrToInt(Edit23->Text);
           Crane_Buff[CN-1].Last_Completed_Time = StrToInt(Edit23->Text);
           Crane_Buff[CN-1].Last_Completed_Temp = StrToInt(Edit23->Text);
       }

       for(int VN=1;VN<=Number_Of_Vehicles;VN++)
       {
           Vehicle_Buff[VN-1].Number_Of_Jobs      = 0;
           Vehicle_Buff[VN-1].Number_Of_Done_Jobs = 0;

           Vehicle_Buff[VN-1].Last_Completed_Time = 0;
       }

       locvalues[0] = ListBox1->Items->Strings[ListBox1->ItemIndex];

       bool b = PortContainerForm->Table1->Locate("Portname", VarArrayOf(locvalues, 0), Opts);

       Label25->Color = clGray ;
       Label25->Caption = "Generating Jobs...";// Static Fashion
       GroupBox15->Refresh();

          if(MCFAlgorithmForm->CheckBox1->Checked )
             srand(1); // 30/04/04
          else
             randomize();

       if( b == false)
         for(i=1; i<=StrToInt(Edit1->Text);i++)
         {
          ProgressBar1->Position = 100*i/StrToInt(Edit1->Text);
          r = 3 + random(2); // 4 if we like four cases
               // 3 because we like to have only Yard to berth and vice versa
          switch(r)
          {
             case 3: // Yard to Berth
               // if(MCFAlgorithmForm->CheckBox1->Checked )
               //    srand(2); // 30/04/04
               // else
               //   randomize();
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
             //   if(MCFAlgorithmForm->CheckBox1->Checked )
             //      srand(3); // 30/04/04
             //   else
             //      randomize();
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

          AnsiString ContainerIDStrS = "C-BS-";
          ContainerIDStrS = ContainerIDStrS + IntToStr(i);

          PortContainerForm->Table1->FieldValues["ContainerID"]= ContainerIDStrS;
          PortContainerForm->Table1->FieldValues["Portname"]   = ListBox1->Items->Strings[ListBox1->ItemIndex];

          PortContainerForm->Table1->FieldValues["SourcePoint"]= BlockStrS;
          PortContainerForm->Table1->FieldValues["DestPoint"]  = BlockStrD;

          int CN = i2;
          int ReadyTime = Quay_Crane_Time_Window * Crane_Buff[CN-1].Number_Of_Jobs
                        + Crane_Buff[CN-1].Last_Completed_Time;

          Crane_Buff[CN-1].Number_Of_Jobs++;

          PortContainerForm->Table1->FieldValues["ReadyTime"]     = ReadyTime;
          PortContainerForm->Table1->FieldValues["QuayCraneTime"] = ReadyTime;

          PortContainerForm->Table1->FieldValues["Done"]          = false;
          PortContainerForm->Table1->Post();
        }

     // Generate schedule
     PortContainerForm->Table2->Refresh() ;

     int   Number_Of_AGVs, Number_Of_Container_Jobs, Number_Of_Nodes, Number_Of_ARCs=0;

     int   ReadyTimeAGV;
     int   Tij, Number_ARCs;

     char Fname[100];
     sprintf(&Fname[0],"%s",BenchOptionForm->Edit1->Text);
     FILE *fp = fopen(Fname,"w+b"); // outstream  string

     AnsiString PortNameStr, Str1 , Str2, SinkNodeStr;

     Opts.Clear();

     PortNameStr = ListBox1->Items->Strings[ListBox1->ItemIndex];

     Number_Of_AGVs = StrToInt(Edit4->Text) ;

     TCursor Save_Cursor = Screen->Cursor;
     Screen->Cursor = crHourGlass;    // Show hourglass cursor

                                                 // Dynamic Flag
     Number_Of_Container_Jobs =  Read_Container_Jobs(0 );
     if (Number_Of_Container_Jobs > 0 && DynamicSchedulingStarted)
         Edit1->Text  = IntToStr(Number_Of_Container_Jobs);

     if(RadioButton31->Checked)  // Prime-Mover Algrithm
     {
        Update_Cranes_And_Vehicles();
        Prime_Mover(Number_Of_AGVs,PortNameStr,Number_Of_Container_Jobs,fp,1 );
     } else if(RadioButton30->Checked || RadioButton22->Checked || ( RadioButton32->Checked && RadioButton27->Checked))
      // network Simplex Algrithm
     {
         if(Number_Of_Container_Jobs*Number_Of_AGVs == 0 ) return;

         Label25->Color = clBlue;
         Label25->Caption = "Making the Model...";// Static Fashion
         GroupBox15->Refresh();


        // Just for test
        Number_ARCs  =  ( Number_Of_AGVs * Number_Of_Container_Jobs)
                     +  ( Number_Of_Container_Jobs * 2)
                     +    Number_Of_AGVs
                     +  Number_Of_Container_Jobs * (Number_Of_Container_Jobs-1);

        Number_Of_Nodes = Number_Of_AGVs + Number_Of_Container_Jobs*2 + 1;

        memset( (void *)(&net), 0, (size_t)(sizeof(MCF_network_t)) );

        long t, h, l, u, c,Max_Cost=0;

        //     register long i;
        MCF_arc_p arc;
        MCF_node_p node;
        long up, down;

        h = Number_ARCs;
        t = Number_Of_Nodes;
        /* The basis structure needs one artificial node, called the root. */
        net.n = (long)t;
        net.m = (long)h;

        /* Allocate memory for the node- and arc-vectors.
           In every artificial initial start basis structure every node
           will be connected to the root 0 by the artificial arc (0,j) or
           (j,0), therefor there must be additionally allocated memory
           for this |V| artificial arcs.
        */
        net.nodes      = (MCF_node_p) calloc( t+1, sizeof(MCF_node_t) );
        net.arcs       = (MCF_arc_p)  calloc( h,   sizeof(MCF_arc_t) );
        net.dummy_arcs = (MCF_arc_p)  calloc( t,   sizeof(MCF_arc_t) );

        net.stop_nodes = net.nodes + (t+1); /* one artificial node! */
        net.stop_arcs  = net.arcs + h;
        net.stop_dummy = net.dummy_arcs + t;

        if( !( net.nodes && net.arcs && net.dummy_arcs ) )
        {
            Application->MessageBox("Not Enough Memory for Graph!! ", "Error", MB_OK);
            MCF_free( &net );
            return;
        }

        for( node = net.nodes, i = 0; i <= t; i++, node++ )
        {
             node->number = i;
             node->ident  = node->pre_ident = MCF_FIXED;
        }



        Str1 = "c  Port Name : " ; Str1 = Str1 + ListBox1->Items->Strings[ListBox1->ItemIndex];
        ListBox2->Items->Add(Str1);

        Str1 = "c  Number of Container Jobs : " ; Str1 = Str1 + IntToStr(Number_Of_Container_Jobs);
        ListBox2->Items->Add(Str1);

        Str1 = "c  Number of AGVs           : " ; Str1 = Str1 + IntToStr(Number_Of_AGVs);
        ListBox2->Items->Add(Str1);

        TimeAGVToC    = (int *) malloc(sizeof(int)*Number_Of_AGVs*Number_Of_Container_Jobs);
        TimeCToC      = (int *) malloc(sizeof(int)*Number_Of_Container_Jobs*Number_Of_Container_Jobs);

        CostAGVToC    = (int *) malloc(sizeof(int)*Number_Of_AGVs*Number_Of_Container_Jobs);
        CostCToC      = (int *) malloc(sizeof(int)*Number_Of_Container_Jobs*Number_Of_Container_Jobs);

        Route_Table   = (int *) malloc(sizeof(int)*(Number_ARCs+1));
        memset(Route_Table, 0 , (Number_ARCs+1));

        if(!CostCToC || !CostAGVToC || !TimeAGVToC || !TimeCToC || !Route_Table)
        {
            Application->MessageBox("Not Enough Memory !! ", "Error", MB_OK);
            return;
        }

        Str1 = "p min " ;  // define Min, number of nodes and arcs
        Str1 = Str1 + IntToStr(Number_Of_Nodes)+ "     "  + IntToStr(Number_ARCs) ;
        fprintf(fp,"%s                       \n", Str1.c_str());
        SinkNodeStr = IntToStr(Number_Of_Nodes);

        Append_Supply_Nodes(Number_Of_AGVs,fp,&net); //  Number_Of_ARCs += Number_Of_AGVs;

        Append_Demand_Node (Number_Of_AGVs,Number_Of_Nodes,fp,&net); // Number_Of_ARCs++;

        arc = net.arcs;
        arc = Append_Arcs_From_AGVs_Nodes_To_Container_Nodes(Number_Of_AGVs,&Number_Of_ARCs,              //SF
                                       PortNameStr,Number_Of_Container_Jobs,fp,1,&net,arc);

        arc = Append_Arcs_From_AGV_Nodes_To_Sink_Node(Number_Of_AGVs,&Number_Of_ARCs,SinkNodeStr,fp,&net,arc);

        arc = Append_Arcs_From_CNode_To_CPNodes(Number_Of_AGVs,Number_Of_Nodes,&Number_Of_ARCs,fp,&net,arc);

        arc = Append_Arcs_From_CPNodes_To_Sink_Node(Number_Of_AGVs,Number_Of_Nodes,SinkNodeStr,&Number_Of_ARCs,fp,&net,arc);

        arc = Append_Arcs_From_CPNodes_To_Any_Other_CNodes(Number_Of_AGVs,Number_Of_Container_Jobs,
                                                    &Number_Of_ARCs,PortNameStr,fp,1,&net,arc);
                                                                                                 // SF
     }
     fclose(fp);


     if( MCFAlgorithmForm->RadioButton7->Checked)
     {
         Label23->Caption=  " Costs ";
         Write_Model_To_ListBox2();
         if(RadioButton30->Checked || RadioButton22->Checked)
            Update_Num_Of_Arcs_To_File(Number_Of_Nodes,Number_Of_ARCs);
     }

     if(RadioButton30->Checked || RadioButton22->Checked || RadioButton32->Checked)
     {
        MCFAlgorithmForm->Edit15->Text = net.max_cost;
        Label25->Color = clMaroon ;
        Label25->Caption = "Solving the Model...";
        GroupBox15->Refresh();
     }

     #if defined TSP-PORT
        MCF_free( &net );  // if it needs to read the model from any file, we have to uncommented that
     #endif

     char Fname2[100];
     sprintf(&Fname[0],"%s",BenchOptionForm->Edit1->Text);
     sprintf(&Fname2[0],"%s",BenchOptionForm->Edit2->Text);

     int stat;
     if(RadioButton30->Checked || RadioButton22->Checked || (RadioButton32->Checked && RadioButton27->Checked))
     {
        if (RadioButton30->Checked )
            net.algorithm_opt = 0;
        else if (RadioButton22->Checked )
             {
                 if(RadioButton20->Checked)
                    net.algorithm_opt = 1;
                 else
                    net.algorithm_opt = 2;
             }
        stat = MCF_NSA_Solve( 2,Fname,Fname2,&net,1,Number_Of_AGVs,0); // Static Fashion
     } else
        stat = 0;

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

           if( MCFAlgorithmForm->RadioButton5->Checked &&
             (( RadioButton30->Checked || RadioButton22->Checked ) ||
              ( RadioButton32->Checked && RadioButton27->Checked )) )
              Write_Solution_To_ListBox5();
           else
              MCFAlgorithmForm->ListBox5->Items->Clear();

           if(RadioButton30->Checked || RadioButton22->Checked || ( RadioButton32->Checked && RadioButton27->Checked))
           {
              Update_Cranes_And_Vehicles(); // Static Fashion
              Read_NSA_Solution(Fname2,Number_ARCs, Number_Of_AGVs, Number_Of_Nodes,1,Number_Of_Container_Jobs);
           }
     }
     Screen->Cursor = Save_Cursor; // always restore the cursor

     if(RadioButton30->Checked || RadioButton22->Checked || (RadioButton32->Checked && RadioButton27->Checked))
     {
         free(TimeAGVToC);
         free(TimeCToC);
         free(CostAGVToC);
         free(CostCToC);
         free(Route_Table);

         MCF_free( &net ); // Static Fashion
     }

  //   if(RadioButton32->Checked && RadioButton27->Checked ) // NSA + SImutaed Annealing
     if(RadioButton32->Checked) // NSA + SImutaed Annealing: 6/6/2004
        Handle_Multi_Load_AGVS();

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

void __fastcall TMCFAlgorithmForm::Button13Click(TObject *Sender)
{
       if(RadioButton12->Checked )
       {
          DBChart2->SaveToBitmapFile(Edit29->Text);
          DBChart3->SaveToBitmapFile(Edit19->Text);
       }
       if(RadioButton13->Checked )
       {
          DBChart2->SaveToMetafile (Edit29->Text);
          DBChart3->SaveToMetafile (Edit19->Text);
       }
       if(RadioButton14->Checked )
       {
         DBChart2->SaveToMetafileEnh (Edit29->Text);
         DBChart3->SaveToMetafileEnh (Edit19->Text);
       }

}
//---------------------------------------------------------------------------

void __fastcall TMCFAlgorithmForm::Timer4Timer(TObject *Sender)
{

       AnsiString PortName = MCFAlgorithmForm->ListBox3->Items->Strings[MCFAlgorithmForm->ListBox3->ItemIndex];

       TBookmark Bookmark =PortAGVForm->Table2->GetBookmark ()  ;

       int Number_Of_AGVs = PortAGVForm->Table2->RecordCount;

       PortAGVForm->Table2->First() ;
       Port_Buff.TotalVTTimes =  0;
       for(int m=1;m<=Number_Of_AGVs;m++)
       {
          int  TTime = PortAGVForm->Table2->FieldByName("CostAGV")->Value;
          Port_Buff.TotalVTTimes +=  TTime;
          PortAGVForm->Table2->Next() ;
       }
       MCFAlgorithmForm->Table2->Append ();
       MCFAlgorithmForm->Table2->FieldByName("PortName")->AsString = PortName   ;
       MCFAlgorithmForm->Table2->FieldValues["TimePeriod"]   = TimeValueInSecond ;
       MCFAlgorithmForm->Table2->FieldValues["EarlyTimes"]   = Port_Buff.TotalEarlyTimes;
       MCFAlgorithmForm->Table2->FieldValues["LateTimes"]    = Port_Buff.TotalLateTimes ;
       MCFAlgorithmForm->Table2->FieldValues["QCWTimes"]     = Port_Buff.TotalQCWTimes  ;
       MCFAlgorithmForm->Table2->FieldValues["AGVWTimes"]    = Port_Buff.TotalVWTimes   ;
       MCFAlgorithmForm->Table2->FieldValues["AGVTTimes"]    = Port_Buff.TotalVTTimes   ;
    //   if(Port_Buff.TotalDoneJobs)
          MCFAlgorithmForm->Table2->FieldValues["DAppActual"]   = Port_Buff.TotalDfApAc ;
    //   else
    //      MCFAlgorithmForm->Table2->FieldValues["DAppActual"]   = 0;
       MCFAlgorithmForm->Table2->FieldValues["NumOfDoneJobs"]= Port_Buff.TotalDoneJobs ;
       MCFAlgorithmForm->Table2->FieldValues["NumOfRemnJobs"]= PortContainerForm->Table2->RecordCount;
       MCFAlgorithmForm->Table2->FieldValues["NumOfEarlyJobs"]= Port_Buff.TotalEarlyJobs ;
       MCFAlgorithmForm->Table2->FieldValues["NumOflateJobs"] = Port_Buff.TotalLateJobs ;
       MCFAlgorithmForm->Table2->Post ();

       PortAGVForm->Table2->GotoBookmark(Bookmark);
}

//---------------------------------------------------------------------------

void __fastcall TMCFAlgorithmForm::Table2CalcFields(TDataSet *DataSet)
{
       int n1 = Table2->FieldByName("NumOfRemnJobs")->AsInteger;
       int n2 = Table2->FieldByName("NumOfDoneJobs")->AsInteger;
       Table2NumOfGenJobs->Value  = n1 + n2;
}
//---------------------------------------------------------------------------

void __fastcall TMCFAlgorithmForm::RadioButton32Click(TObject *Sender)
{
    if (RadioButton32->Checked )
    {
         GroupBox31->Enabled = true;
         GroupBox32->Enabled = true;
    } else
    {
         GroupBox31->Enabled = false;
         GroupBox32->Enabled = false;
    }
}
//---------------------------------------------------------------------------

void __fastcall TMCFAlgorithmForm::DigitalAnalog1Click(TObject *Sender)
{
          Button11Click(Sender);
}
//---------------------------------------------------------------------------



void __fastcall TMCFAlgorithmForm::Edit26Change(TObject *Sender)
{
       Edit18->Text = Edit26->Text;
}
//---------------------------------------------------------------------------


void __fastcall TMCFAlgorithmForm::Edit1Change(TObject *Sender)
{
       if(StrToInt(Edit1->Text) > Maximum_New_Jobs_For_Crane )
       {
          Edit1->Text  = IntToStr(Maximum_New_Jobs_For_Crane );
          Application->MessageBox("Maximum New Jobs for each crane is 100 ", "Warning", MB_OK);
       }
//       Edit9->Text = Edit1->Text;
}
//---------------------------------------------------------------------------

