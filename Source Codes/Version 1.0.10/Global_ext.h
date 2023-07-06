#include "Defines.h"

int   __fastcall Get_Time_Between_Two_Points(AnsiString PortName,
                                             AnsiString SourcePoint,
                                             AnsiString DestPoint);


extern struct {
   String     Port ;
   int        NumberOfBlockYard;
   int        NumberOfWorkingPosition;
   int        NumberOfAGVs  ;
   int        NumberOfJobs  ;
   int        NumberOfJunctions ;
   int        NumberOfLanes ;
   int        NumberOfZones ;
   long       TotalEarlyTimes;
   long       TotalLateTimes;
   long       TotalQCWTimes;
   long       TotalVWTimes;
   long       TotalVTTimes;
   long       TotalDoneJobs;
   long       TotalEarlyJobs;
   long       TotalLateJobs;
   double     TotalDfApAc;
   FILE      *Fout3;
}  Port_Buff;

extern struct {
   String     IDStr ;
   String     SourcePointStr;
   String     DestPointStr  ;
   long int   ReadyTime     ;
   long int   QCraneTime    ;
   int        Vehicle       ;
   
   unsigned char Empty      ;
   unsigned char Done       ;
}  Container_Buff[Maximum_Container_Jobs];

extern struct {
   int  Last_Completed_Temp;
   int  Number_Of_Jobs;
   int  Next_Generation_Time;
   int  Last_Completed_Time;
   int  Number_Of_Done_Jobs;
   int  QPrev_ActualTime;
   int  QCurr_ActualTime;
}  Crane_Buff[Maximum_Number_Cranes];

extern struct {
   int  Number_Of_Fulfilled;
   int  Number_Of_New_Jobs;
   int  Deleted_Jobs[Maximum_New_Jobs_For_Crane];
   int  New_Jobs    [Maximum_New_Jobs_For_Crane];
   int  Flag        [Maximum_New_Jobs_For_Crane];  // Flag to use for constructing the new solution
}  Job_Crane_Buff[Maximum_Number_Cranes];

extern struct {
   String StartLoc;
   int    Last_Completed_Temp;
   int    Number_Of_Jobs;
   int    Last_Completed_Time;
   int    Number_Of_Done_Jobs;
   int    Lane_No            ; // <=Number_of_Lanes
   int    Time_in_Lane       ;
   int    Number_of_Lanes    ;
   int    Lane     [Maximum_Number_Lanes];
   int    Cost_Temp;
   int    PreJob_Temp;
   int    No_Job_Temp;
}  Vehicle_Buff[Maximum_Number_Vehicles];


extern struct {
   int    Junction ;
   String Location ;
   int    NextJunc1;
   int    NextLane1;
   int    DurationLane1;
   int    NextJunc2;
   int    NextLane2;
   int    DurationLane2;
   int    NextJunc3;
   int    NextLane3;
   int    DurationLane3;
}  Route_Buff[Maximum_Number_Junctions];


extern struct {
   int    Duration;
   int    Busy    ;
   int    Vehicle ; // [Maximum_NAGV_In_Lan];
}  Route_Buff2[Maximum_Number_Lanes];



