#include  <vcl.h>
#include  <iostream.h>
#include  <fstream.h>
#include  <stdio.h>
#include  <math.h>
#include  <iomanip.h>
#include  <stdlib.h>
#include  <search.h>
#include  <time.h>
#include  <algorithm>
#include  <functional>
#include  <string.h>
#include  "Global_ext.h"

struct Container
{
   int        JobNo      ;
   int        Demand     ;
   int        AppointmentTime;  // ReadyTime
   int        QCraneTime ;
   int        DueTime    ;
   int        ServiceTime;
   int        SourceDone ;        // 1 if the job has been pick up, otherwise 0
   int        DestDone   ;        // 1 if the job has been put down, otherwise 0
   String     SourceLocation;
   String     DestLocation  ;
   int        UseConstraintTime ; //  1 if the job concern with pick-up time constraint;
};                                  // -1 if the job concern with put-down time constraint;

struct  Vehicle
{
	int    Capacity;
        String StartLoc;
};


struct  VehileList
{
        int    JobNo;
        int    Action;
        int    ServTime;
};

struct  Trip
{
        String       LocToVisit;
        int          Load;
        int          VehicleTime;
        int          ActualTime;
        VehileList   Contents  [CAPACITY];
};

struct Tour
{
        Trip   *PortTrip;
	int     TotalDemand;
        int     TotalDist;
        int     TotalIdle;
        int     TotalLate;
        int     TotalWait;
        int     Length;
};

struct DistTimeInfo
{
	int    Dist;
	int    Index;
};


Container *CJob;
Vehicle  *AGV;
Tour     *TAGV,*TempT,*BestT;

//---------------------------------------------------------------------------
class SAAVRP
{
public:
	SAAVRP() ;
	~SAAVRP() {};
	void     DeleteMemory();
        void     ReadData();

	void     GenerateData(int TW, int C, int NJ, int MV);
	void     ReadParameters(ifstream &fIn, char *&filename);
        void     GetParameters();

	void     InitialTour1(ofstream &fOut);
	void     InitialTour2(ofstream &fOut);
	void     InitialTour3(ofstream &fOut);
	void     InitialTour11(ofstream &fOut);

        void     Update_Load_Vehicle(int Point,Tour *TR,int v);
        void     Update_Last_ServiceTimeVehicle(int Point,Tour *TR,int v);
        void     PickUp_Or_DeliverJobVehicle(int Job,int Action, Tour *TR, int v);
	void     PrintInfo(ofstream &fOut, Tour *TR);
	void     PrintContainerInfo(ofstream &fOut);
	void     PrintTripInfo(ofstream &fOut, Tour *TR);
	void     PrintTour(ofstream &fOut, Tour *TR);

	double   ObjectiveValue(Tour TT);
	double   ObjectiveFunction(Tour *TR);

	void     CopyTour(Tour *&T1, Tour *&T2);
	void     Deletion(Tour *&TR, int v, int start, int end);
	void     Insertion(Tour *&TR, int vI, int iSpot, int vD, int start, int end);
	int      Move(int &i1, int &i2,int M,int N);
	int      Feasible(Tour *&TR, int i1, int i2,int S,int E,int I);

	void     CalcValues(Tour *&TR, int i1, int i2);
	void     CalcValues(Tour *&TR);

	void     SimulatedAnnealing(ofstream &fOut, ofstream &fOutB, ofstream &fOutX);

	int      MaxTChange, MaxIterations,dLimit,sLimit;

	double   tInitial,tEnd;
	double   rCooling,criticalRatio; //not using criticalRatio
	double   weight1,weight2,weight3;
	int      NumVehicles,NumContainers,vCapacity,TWindowVehicle;
	double   objective;
        String   PortName;
private:

};
//---------------------------------------------------------------------------
void SAAVRP::DeleteMemory()
{
   delete [ ] CJob;
   delete [ ] AGV;
   delete [ ] TAGV;
   delete [ ] TempT;
   delete [ ] BestT;
   CJob  =  0;
   AGV   =  0;
   TAGV  =  0;
   TempT =  0;
   BestT =  0;
   return;
}
//---------------------------------------------------------------------------
void SAAVRP::ReadData()
{
     for(int VN=1;VN<=Port_Buff.NumberOfAGVs ;VN++)
         AGV[VN-1].StartLoc = Vehicle_Buff[VN-1].StartLoc;

     for(int i=0;i<Port_Buff.NumberOfJobs;i++)
     {
         CJob[i].JobNo           = i+1;
         CJob[i].Demand          = 1  ;
         CJob[i].AppointmentTime = Container_Buff[i].ReadyTime;

         CJob[i].SourceLocation  = Container_Buff[i].SourcePointStr;
         CJob[i].DestLocation    = Container_Buff[i].DestPointStr  ;

         CJob[i].SourceDone      = 0;
         CJob[i].DestDone        = 0;
         String SourcePointContainerStr = Container_Buff[i].SourcePointStr ;
         if(SourcePointContainerStr.SubString(1,3) == "W/P")
            CJob[i].UseConstraintTime = 1;
         else
            CJob[i].UseConstraintTime = 2;
     }
     return;
}
//---------------------------------------------------------------------------
void SAAVRP::GenerateData(int TW, int Capacity, int NumJ, int NumV)
{
}
//---------------------------------------------------------------------------
void SAAVRP::ReadParameters(ifstream &fIn, char *&filename)
{
     char q,c;
     filename = new char[MAXChar];

     fIn>>q>>c;
     while(c!='\"')
     	fIn>>c;
     fIn>>q>>q;
     fIn>>q>>c;
     fIn.getline(filename,MAXChar,'\'');
     fIn>>q>>q>>q;
     fIn>>q>>tInitial>>q>>q>>tEnd>>q>>q>>rCooling>>q>>q>>MaxTChange>>q>>q>>MaxIterations>>q;
     fIn>>q>>dLimit>>q>>q>>sLimit>>q>>q>>criticalRatio>>q;
     fIn>>q>>weight1>>q>>q>>weight2>>q>>q>>weight3>>q;
     return;

}
//---------------------------------------------------------------------------
void SAAVRP::PrintContainerInfo(ofstream &fOut)
{
     int i;
     char ss[100];


     fOut<<"\nNumber of Vehicles:";
     fOut<<"\n"<<NumVehicles;
     fOut<<"\nVehicle Information:";
     fOut<<"\nAGV\tCapacity\tStart";
     fOut<<"\nNo\t\t\tLoc";
     fOut<<"\n--------------------------------------------------------------";
     for(i=0;i<NumVehicles;i++)
     {
     	fOut<<"\n"<<i+1<<"\t"<<AGV[i].Capacity<<"\t";
                 sprintf(ss,"%s",AGV[i].StartLoc);
        fOut<<"\t"<<ss;
     }
     fOut<<"\n\nNumber of Jobs:";
     fOut<<"\n"<<NumContainers; // MAXJOB;
     fOut<<"\nContainer Jobs Information:";
     fOut<<"\nJob\tDemand\tAppointment\tSource\t\tDest\t\tSource\tDest\tUseConstarint";
     fOut<<"\nNo\tAmount\tTime\t\tLoc\t\tLoc\t\tDone\tDone\tTime";
     fOut<<"\n--------------------------------------------------------------------------------------------";
     for(i=0;i<NumContainers;i++)
     {
     	fOut<<"\n"<<CJob[i].JobNo            \
            <<"\t"<<CJob[i].Demand           \
            <<"\t"<<CJob[i].AppointmentTime; // QCraneTime;
        sprintf(ss,"%s",CJob[i].SourceLocation);
        fOut<<"\t\t"<<ss;
        sprintf(ss,"%s", CJob[i].DestLocation);
        fOut<<"\t\t"<<ss<<"\t";
        fOut<<"\t"<<CJob[i].SourceDone       \
            <<"\t"<<CJob[i].DestDone         \
            <<"\t"<<CJob[i].UseConstraintTime;
     }
}
//---------------------------------------------------------------------------
void SAAVRP::PrintTripInfo(ofstream &fOut, Tour *TR)
{
     fOut<<setiosflags(ios::showpoint | ios::fixed);
     fOut<<setprecision(0);
     int i,j;
     char ss[100];

     fOut<<"\n\nTrip Information:";
     fOut<<"\nVehicle\tTrip Information";
     for(int m=0;m<NumVehicles;m++)
     {
     	fOut<<"\nVehicle\tTrip  \tLoad\tVehicle\tActual\t";
        for(int l=0;l<CAPACITY;l++)
           fOut<<"\tContent\tAction ";

     	fOut<<"\n"<<m+1<<"\tAtLoc \t    \tTime   \t Time  \t";
        for(int l=0;l<CAPACITY;l++)
           fOut<<"\t["<<l<<"]\t["<<l<<"]";
     	for(j=0;j<TR[m].Length;j++)
        {
       	    fOut<<"\n";
            sprintf(ss,"%s",TR[m].PortTrip[j].LocToVisit);
            fOut<<"\t"<<ss;
            fOut<<"\t'"<<TR[m].PortTrip[j].Load<<"'"    ;
            fOut<<"\t"<< TR[m].PortTrip[j].VehicleTime     ;
            fOut<<"\t"<< TR[m].PortTrip[j].ActualTime<<"\t\t";
            for(int l=0;l<CAPACITY;l++)
            {
                if(!TR[m].PortTrip[j].Contents[l].JobNo) break;
                fOut <<TR[m].PortTrip[j].Contents[l].JobNo;
                switch(TR[m].PortTrip[j].Contents[l].Action )
                {
                     case 0:
                        fOut<<"\t"<<TR[m].PortTrip [j].Contents[l].Action<<"\t";
                        break;
                     case PICKUP:
                        fOut<<"\tP\t";
                        break;
                     case DELIVER:
                        fOut<<"\tD\t";
                }
            }
        }
        fOut<<"\nTotal Dist="<<TR[m].TotalDist;
        fOut<<"  Total Wait="<<TR[m].TotalWait;
        fOut<<"  Total Late="<<TR[m].TotalLate;
        fOut<<"\n------------------------------------------------------------------------------------------------------";
     }
     return;
}
//---------------------------------------------------------------------------
void SAAVRP::PrintInfo(ofstream &fOut, Tour *TR)
{
     fOut<<"\n";
     PrintContainerInfo(fOut);
     PrintTripInfo(fOut,TR);
     return;
}
//---------------------------------------------------------------------------
void SAAVRP::PrintTour(ofstream &fOut, Tour *TR)
{
     int i,j;
     fOut<<"\n";
     for(i=1;i<=NumVehicles;i++)
     {
         fOut<<"\n"<<i<<" ";
      //   for(j=0;j<=20;j++)
      //       fOut<<TR[i].TripSource[j]<<" ";
     }
     return;
}
//---------------------------------------------------------------------------
double SAAVRP::ObjectiveValue(Tour TT)
{
     return (weight1*TT.TotalDist)+(weight2*TT.TotalWait)+(weight3*TT.TotalLate);
}
//---------------------------------------------------------------------------
double SAAVRP::ObjectiveFunction(Tour *TR)
{
     int i;
     objective=0;
     for(i=0;i<NumVehicles;i++)
         objective = objective + ObjectiveValue(TR[i]);
     return objective;
}
//---------------------------------------------------------------------------
typedef int (*CompFunction)(const void*, const void*);
int CompareDT(struct DistTimeInfo *elem1, struct DistTimeInfo *elem2)
{
    if ( elem1->Dist < elem2->Dist)
         return -1;
    else if (elem1->Dist > elem2->Dist)
             return 1;
         else
             return 0;
}

//---------------------------------------------------------------------------
SAAVRP::SAAVRP()
{
     int MAXVEHICLES ;
     int MAXJOB      ;

     MAXVEHICLES= NumVehicles   = Port_Buff.NumberOfAGVs;
     MAXJOB     = NumContainers = Port_Buff.NumberOfJobs;

     TAGV  = new Tour[MAXVEHICLES];	//start index at 1
     TempT = new Tour[MAXVEHICLES];
     BestT = new Tour[MAXVEHICLES];

     AGV   = new Vehicle   [MAXVEHICLES];
     CJob  = new Container [MAXJOB];

     for(int i=0;i<MAXVEHICLES;i++)	//initialize Trips
     {
         TAGV [i].PortTrip  = new struct Trip [2*MAXJOB];
         TempT[i].PortTrip  = new struct Trip [2*MAXJOB];
         BestT[i].PortTrip  = new struct Trip [2*MAXJOB];

         TAGV [i].TotalDist = TAGV[i].TotalWait =  TAGV[i].TotalLate =  TAGV[i].Length =  0;

         for(int j=0;j<MAXJOB*2;j++)
         {
           TAGV[i].PortTrip [j].LocToVisit    ='0';
           TAGV[i].PortTrip [j].Load          = 0;
           TAGV[i].PortTrip [j].VehicleTime   = 0;
           TAGV[i].PortTrip [j].ActualTime    = 0;
           for(int l=0;l<CAPACITY;l++)
           {
                TAGV[i].PortTrip [j].Contents[l].JobNo   = 0;
                TAGV[i].PortTrip [j].Contents[l].Action  = 0;
                TAGV[i].PortTrip [j].Contents[l].ServTime= 0;
           }
         }
         for(int j=0;j<MAXJOB*2;j++)
         {
           TempT[i].PortTrip [j].LocToVisit    = '0';
           TempT[i].PortTrip [j].Load          =  0;
           TempT[i].PortTrip [j].VehicleTime   =  0;
           TempT[i].PortTrip [j].ActualTime    =  0;
           for(int l=0;l<CAPACITY;l++)
           {
                TempT[i].PortTrip [j].Contents[l].JobNo   = 0;
                TempT[i].PortTrip [j].Contents[l].Action  = 0;
                TempT[i].PortTrip [j].Contents[l].ServTime= 0;
           }
         }
         for(int j=0;j<MAXJOB*2;j++)
         {
           BestT[i].PortTrip [j].LocToVisit    = '0';
           BestT[i].PortTrip [j].Load          =  0;
           BestT[i].PortTrip [j].VehicleTime   =  0;
           BestT[i].PortTrip [j].ActualTime    =  0;
           for(int l=0;l<CAPACITY;l++)
           {
                BestT[i].PortTrip [j].Contents[l].JobNo    = 0;
                BestT[i].PortTrip [j].Contents[l].Action   = 0;
                BestT[i].PortTrip [j].Contents[l].ServTime = 0;
           }
         }
     }
}
//---------------------------------------------------------------------------
void  SAAVRP::Update_Load_Vehicle(int Point,Tour *TR,int m)
{
      for(int j=Point;j<=TR[m].Length;j++)
      {
          for(int l=0;l<CAPACITY;l++)
          {
              switch(TR[m].PortTrip[j].Contents[l].Action)
              {
                 case PICKUP :
                      if(!j)
                         TR[m].PortTrip[j].Load = ONE;
                      else
                      {
                         if (!l)
                            TR[m].PortTrip[j].Load = TR[m].PortTrip[j-1].Load + ONE;
                         else
                            TR[m].PortTrip[j].Load = TR[m].PortTrip[j  ].Load + ONE;
                      }
                      break;
                 case DELIVER:
                         if (!l)
                            TR[m].PortTrip[j].Load = TR[m].PortTrip[j-1].Load - ONE;
                         else
                            TR[m].PortTrip[j].Load = TR[m].PortTrip[j  ].Load - ONE;
              }
          }
          if(TR[m].PortTrip[j].Load > ONE )
          {
               memcpy( &TR[m].PortTrip[j].Contents[1],
                       &TR[m].PortTrip[j].Contents[0], (CAPACITY-1)*sizeof(struct  VehileList));
               memcpy( &TR[m].PortTrip[j  ].Contents[0],
                       &TR[m].PortTrip[j-1].Contents[0], sizeof(struct  VehileList));
          }
          if(j > 1 && TR[m].PortTrip[j].Load < TR[m].PortTrip[j-1].Load && TR[m].PortTrip[j].Load > 0 )
          {
               memcpy( &TR[m].PortTrip[j].Contents[1],
                       &TR[m].PortTrip[j].Contents[0], (CAPACITY-1)*sizeof(struct  VehileList));

               memcpy( &TR[m].PortTrip[j  ].Contents[0],
                       &TR[m].PortTrip[j-1].Contents[0], sizeof(struct  VehileList));
          }
      }

}
//---------------------------------------------------------------------------
void  SAAVRP::Update_Last_ServiceTimeVehicle(int Point,Tour *TR, int v)
{
      int j,l;
      for(j=Point;j<TR[v].Length;j++)
      {
         if (!j)
             TR[v].PortTrip[j].VehicleTime =
                Get_Time_Between_Two_Points(PortName,AGV[v].StartLoc,TR[v].PortTrip[j].LocToVisit);
         else
             TR[v].PortTrip[j].VehicleTime = TR[v].PortTrip[j-1].ActualTime
                  + Get_Time_Between_Two_Points(PortName,TR[v].PortTrip[j-1].LocToVisit,
                                                         TR[v].PortTrip[j].LocToVisit);

         int Max_S = 0;
         for(l=0;l<CAPACITY;l++)
         {
             int i = TR[v].PortTrip[j].Contents[l].JobNo-1;
          //   if(!i)
          //      break;
             switch( TR[v].PortTrip[j].Contents[l].Action )
             {
                case PICKUP:
                     if(CJob[i].UseConstraintTime == PICKUPTIME )
                        Max_S = max ( Max_S , max(TR[v].PortTrip [j].VehicleTime ,
                                                  CJob[i].AppointmentTime));
                     else
                        Max_S = max ( Max_S , TR[v].PortTrip [j].VehicleTime );
                     break;
                case DELIVER:
                     if(CJob[i].UseConstraintTime == DELIVRTIME)
                        Max_S = max ( Max_S , max(TR[v].PortTrip [j].VehicleTime ,
                                                   CJob[i].AppointmentTime));
                     else
                        Max_S = max ( Max_S , TR[v].PortTrip [j].VehicleTime );
             }
         }
         TR[v].PortTrip[j].ActualTime = Max_S;

      }
}
//---------------------------------------------------------------------------
void  SAAVRP::PickUp_Or_DeliverJobVehicle(int i,int Action, Tour *TR, int m)
{
      int Len = TR[m].Length;
      int L   = TR[m].PortTrip [Len].Load;
      switch(Action)
      {
        case PICKUP:
             TR[m].PortTrip   [Len].LocToVisit = CJob[i].SourceLocation ;
             TR[m].PortTrip   [Len].Contents[L].JobNo = CJob[i].JobNo ;
             TR[m].PortTrip   [Len].Contents[L].Action= PICKUP;
             if(Len)
             {
                TR[m].PortTrip [Len].VehicleTime  = TR[m].PortTrip [Len-1].ActualTime
                   + Get_Time_Between_Two_Points(PortName,CJob[i].SourceLocation,TR[m].PortTrip [Len-1].LocToVisit);

                if(CJob[i].UseConstraintTime == PICKUPTIME)
                   TR[m].PortTrip [Len].ActualTime= max(TR[m].PortTrip [Len].VehicleTime ,
                                                           CJob[i].AppointmentTime ) ;
                else
                   TR[m].PortTrip [Len].ActualTime= TR[m].PortTrip [Len].VehicleTime       ;

                TR[m].PortTrip [Len].Load = TR[m].PortTrip [Len-1].Load
                                          + TR[m].PortTrip [Len  ].Contents[L].Action;
             } else
             {
                TR[m].PortTrip [Len].VehicleTime  =
                 Get_Time_Between_Two_Points(PortName,AGV[m].StartLoc,CJob[i].SourceLocation);

                if(CJob[i].UseConstraintTime == PICKUPTIME)
                  TR[m].PortTrip [Len].ActualTime= max(TR[m].PortTrip [Len].VehicleTime ,
                                                       CJob[i].AppointmentTime ) ;
                else
                  TR[m].PortTrip [Len].ActualTime= TR[m].PortTrip [Len].VehicleTime       ;

                TR[m].PortTrip [Len].Load = ONE;
             }
             CJob[i].SourceDone =  DONE;
             TR[m].Length++;
             break;
        case DELIVER:
             Len = TR[m].Length;
             L   = TR[m].PortTrip [Len].Load;
             TR[m].PortTrip   [Len].LocToVisit = CJob[i].DestLocation ;
             TR[m].PortTrip   [Len].VehicleTime  = TR[m].PortTrip[Len-1].ActualTime
                  + Get_Time_Between_Two_Points(PortName,TR[m].PortTrip[Len-1].LocToVisit,
                                                         CJob[i].DestLocation);
             if(CJob[i].UseConstraintTime == DELIVRTIME)
                TR[m].PortTrip[Len].ActualTime= max(TR[m].PortTrip [Len].VehicleTime ,
                                                     CJob[i].AppointmentTime) ;
             else
                TR[m].PortTrip[Len].ActualTime= TR[m].PortTrip [Len].VehicleTime       ;

             TR[m].PortTrip   [Len].Contents[L].JobNo = CJob[i].JobNo ;
             TR[m].PortTrip   [Len].Contents[L].Action= DELIVER;
             TR[m].PortTrip   [Len].Load       = TR[m].PortTrip [Len-1].Load
                                               + TR[m].PortTrip [Len  ].Contents[L].Action;

             CJob[i].DestDone   =  DONE;
             TR[m].Length++;
       }
}
//---------------------------------------------------------------------------
void SAAVRP::InitialTour1(ofstream &fOut)
{    // Constructive Random Initial Solution
     // Finding which job has a lower cost for every vehilce, then allocate the job for the vehicle
     int    i,j,iteration,count,loops,extra,NumContainersLeft;
     double distTemp;

     NumContainersLeft = NumContainers;
     for(int m=0;m<NumVehicles;m++)
     {
         double BestObj = MAXCOST;
         double D1, ObjFun  ;
         int    FindJob = -1;
         for(i=0;i<NumContainers;i++)
         {
             if(CJob[i].SourceDone == REMAIN)
             {
                switch(CJob[i].UseConstraintTime)
                {
                   case PICKUPTIME: // constarint time at the source
                        D1 = Get_Time_Between_Two_Points(PortName,AGV[m].StartLoc,
                                                         CJob[i].SourceLocation);
                        if(CJob[i].AppointmentTime >   D1)
                           ObjFun= weight2 *(CJob[i].AppointmentTime - D1)
                                 + weight1 * D1;
                        else
                           ObjFun= -weight3 *(CJob[i].AppointmentTime - D1)
                                 +  weight1 * D1;

                        if(ObjFun < BestObj)
                        {
                           FindJob = i;
                           BestObj = ObjFun ;
                        }
                        break;
                   case DELIVRTIME:   // constarint time at the destination
                        D1 = Get_Time_Between_Two_Points(PortName,
                                                         AGV[m].StartLoc,
                                                         CJob[i].SourceLocation)
                           + Get_Time_Between_Two_Points(PortName,
                                                         CJob[i].SourceLocation,
                                                         CJob[i].DestLocation);
                        if(CJob[i].AppointmentTime >  D1)
                           ObjFun= weight2 *(CJob[i].AppointmentTime  - D1)
                                 + weight1 * D1;
                        else
                           ObjFun= -weight3 *(CJob[i].AppointmentTime - D1)
                                 +  weight1 * D1;

                        if(ObjFun < BestObj)
                        {
                           FindJob = i;
                           BestObj = ObjFun ;
                        }
                 }
             }
         }
         PickUp_Or_DeliverJobVehicle(FindJob,PICKUP ,TAGV,m);
         PickUp_Or_DeliverJobVehicle(FindJob,DELIVER,TAGV,m);
         NumContainersLeft--;
     }


     loops = NumContainersLeft  / NumVehicles;
     extra = NumContainersLeft  % NumVehicles;

     for(int m=0;m<NumVehicles;m++)
     {
       for(int j=0;j<loops;j++)
       {
         for(i=0;i<NumContainers;i++)
         {
           if(CJob[i].SourceDone == REMAIN)
           {
              PickUp_Or_DeliverJobVehicle(i,PICKUP ,TAGV,m);
              PickUp_Or_DeliverJobVehicle(i,DELIVER,TAGV,m);
              break;
           }
         }
       }
     }
     for(int m=0;m<extra;m++)
     {
         for(i=0;i<NumContainers;i++)
         {
           if(CJob[i].SourceDone == REMAIN)
           {
              PickUp_Or_DeliverJobVehicle(i,PICKUP ,TAGV,m);
              PickUp_Or_DeliverJobVehicle(i,DELIVER,TAGV,m);
           }
         }
     }
     CalcValues(TAGV);
     PrintInfo(fOut,TAGV);
     fOut<<"\nObjective Value = "<< ObjectiveFunction(TAGV);
     return;
}
//---------------------------------------------------------------------------
void SAAVRP::InitialTour2(ofstream &fOut)
{    // Random initial feasible solution
     int i,m, j, NumContainersDone = 0;
     do
     {
     	do
     	{
   	    m   =  rand()  %  NumVehicles   ;
     	    i   =  rand()  %  NumContainers ;
     	} while(CJob[i].SourceDone == DONE);

        PickUp_Or_DeliverJobVehicle(i,PICKUP ,TAGV,m);
        PickUp_Or_DeliverJobVehicle(i,DELIVER,TAGV,m);

        NumContainersDone++;

     } while(NumContainersDone < NumContainers);

     CalcValues(TAGV);
     PrintInfo(fOut,TAGV);

     fOut<<"\nObjective Value = "<<ObjectiveFunction(TAGV);
     return;
}
//---------------------------------------------------------------------------
void SAAVRP::InitialTour11(ofstream &fOut)
{    // Deterministic Initial Feasible Solution
     int    i,j,iteration,count,loops,extra,NumContainersLeft;
     double distTemp;

     NumContainersLeft = NumContainers;

     loops = NumContainersLeft  / NumVehicles;
     extra = NumContainersLeft  % NumVehicles;

     for(int m=0;m<NumVehicles;m++)
     {
       for(int j=0;j<loops;j++)
       {
         for(i=0;i<NumContainers;i++)
         {
           if(CJob[i].SourceDone == REMAIN)
           {
              PickUp_Or_DeliverJobVehicle(i,PICKUP ,TAGV,m);
              PickUp_Or_DeliverJobVehicle(i,DELIVER,TAGV,m);
              break;
           }
         }
       }
     }

     for(int m=0;m<extra;m++)
     {
         for(i=0;i<NumContainers;i++)
         {
           if(CJob[i].SourceDone == REMAIN)
           {
              PickUp_Or_DeliverJobVehicle(i,PICKUP ,TAGV,m);
              PickUp_Or_DeliverJobVehicle(i,DELIVER,TAGV,m);
           }
         }
     }
     CalcValues(TAGV);
     PrintInfo(fOut,TAGV);
     fOut<<"\nObjective Value = "<< ObjectiveFunction(TAGV);
     fOut.flush();
     return;
}
//---------------------------------------------------------------------------
void SAAVRP::InitialTour3(ofstream &fOut)
//capacity is a hard constraint
//time windows is a hard constraint
{
 /*    srand( (unsigned)time( NULL ) );
     int i,j,loops,extra,NumContainersLeft,rLen,sum;

   //  DistTimeInfo DT[MAXJOB];	//cannot declare as a pointer because of qsort function

     TAGV  = new Tour[MAXVEHICLES];	//start index at 1
     TempT = new Tour[MAXVEHICLES];
     BestT = new Tour[MAXVEHICLES];

     loops = NumContainers/NumVehicles;

     extra = NumContainers % NumVehicles;

     CJob[0].SourceDone =  -1;
     for(i=1;i<=NumVehicles;i++)	//initialize Trips
     {
     	TAGV[i].Trip               = new int [NumContainers+2];	//0-a-b-c-...-0
     	TAGV[i].StartServiceTime   = new int [NumContainers+2];
     	TAGV[i].VehicleTimeAtSource  = new int [NumContainers+2];
     	TAGV[i].VehicleTimeAtDest    = new int [NumContainers+2];

     	TempT[i].Trip              = new int [NumContainers+2];	//0-a-b-c-...-0
     	TempT[i].StartServiceTime  = new int [NumContainers+2];
     	TempT[i].VehicleTimeAtSource = new int [NumContainers+2];
     	TempT[i].VehicleTimeAtDest   = new int [NumContainers+2];

     	BestT[i].Trip              = new int [NumContainers+2];	//0-a-b-c-...-0
     	BestT[i].StartServiceTime  = new int [NumContainers+2];
     	BestT[i].VehicleTimeAtLoc    = new int [NumContainers+2];
     	BestT[i].VehicleTimeAtDest   = new int [NumContainers+2];

     	TAGV[i].TotalDemand =  TAGV[i].TotalDist =  TAGV[i].TotalIdle =  TAGV[i].TotalLate =  TAGV[i].Length =  0;

     	for(j=0;j<NumContainers+1;j++)
     	{
     	    TAGV[i].TripSource[j] =  0;
     	    TAGV[i].StartServiceTime[j] =  0;
     	    TAGV[i].VehicleTimeAtLoc[j] =  0;
     	}
     }

     qsort((void *) &DT,NumContainers,sizeof(struct DistTimeInfo),(CompFunction)CompareDT);

     for(i=1;i<=NumVehicles;i++)
     {
     	TAGV[i].TripSource[1]     =  DT[i-1].Index;	//Each vehicle takes the Trip that is the closest
     	TAGV[i].TotalDemand =  TAGV[i].TotalDemand+CJob[DT[i-1].Index].Demand;
     	CJob[DT[i-1].Index].SourceDone =  1;
     	TAGV[i].Length++;
     }
     // At the first iteration, if capacity exceeds, then the data set is infeasible

     do
     {
        for(i=1;i<=NumVehicles;i++)	//now for each vehicle, assign the closest for loops-1
        {
            rLen =  rand() % (2*(NumContainers/NumVehicles)) + 1;
            do
            {
                //should be ratio of NumVehicles and NumContainers
                NumContainersLeft =  0;
                for(j=1;j<=NumContainers;j++)
                {
                    if(CJob[j].SourceDone ==  0)
                    {
                       int CNo = TAGV[i].TripSource[TAGV[i].Length];
                       DT[NumContainersLeft].Index = j;
                       NumContainersLeft++;
                    }
                }
                qsort((void *) &DT ,NumContainersLeft,sizeof(struct DistTimeInfo),
                         (CompFunction)CompareDT);

                //ordered distances from last element of Trip
                j =  1;
                while(( (CJob[DT[j-1].Index].SourceDone !=  0)||
                        (TAGV[i].TotalDemand + CJob[DT[j-1].Index].Demand > AGV[i].Capacity))&&
                        (j < NumContainersLeft))
                	j++;

                if((CJob[DT[j-1].Index].SourceDone == 0)&&
                   (TAGV[i].TotalDemand+CJob[DT[j-1].Index].Demand<= AGV[i].Capacity))
                   //then accept and enter into Trip else nothing (check)
                {
                  TAGV[i].Length++;
                  TAGV[i].TripSource[TAGV[i].Length] =  DT[j-1].Index;
                  TAGV[i].TotalDemand       =  TAGV[i].TotalDemand + CJob[DT[j-1].Index].Demand;
                  CJob[DT[j-1].Index].SourceDone  =  1;
                }
            } while((TAGV[i].Length <= rLen) && (NumContainersLeft > 0));
        }
        sum =  0;
        for(i=1;i<=NumContainers;i++)
            sum =  sum + CJob[i].SourceDone;
        cout<<sum;

     } while (sum < NumContainers);
     //if the loop does not take care of all.... go back around till it does
     CalcValues(TAGV);
     PrintInfo(fOut,TAGV);
     fOut<<"\nObjective Value = "<<ObjectiveFunction(TAGV);
  */
 //    fOut.close() ;
  //   exit(0);

     return;
}
//---------------------------------------------------------------------------
void SAAVRP::CopyTour(Tour *&T1, Tour *&T2)
{
     int MAXJOB = Port_Buff.NumberOfJobs;
     int i,j;
     for(i=0;i<NumVehicles;i++)
     {

         for(j=0;j<2*MAXJOB;j++)
            memcpy(&T1[i].PortTrip[j]  , &T2[i].PortTrip[j] , sizeof(struct Trip) ) ; // *T2[i].Length);

         /*
         for(j=0;j<T2[i].Length;j++)
         {
            T1[i].PortTrip[j].LocToVisit  = T2[i].PortTrip[j].LocToVisit ;
            T1[i].PortTrip[j].Load        = T2[i].PortTrip[j].Load       ;
            T1[i].PortTrip[j].VehicleTime = T2[i].PortTrip[j].VehicleTime;
            T1[i].PortTrip[j].ActualTime  = T2[i].PortTrip[j].ActualTime ;
            for(int l=0;l<CAPACITY;l++)
            {
                T1[i].PortTrip[j].Contents[l].JobNo    = T2[i].PortTrip[j].Contents[l].JobNo;
                T1[i].PortTrip[j].Contents[l].Action   = T2[i].PortTrip[j].Contents[l].Action  ;
                T1[i].PortTrip[j].Contents[l].ServTime = T2[i].PortTrip[j].Contents[l].ServTime;
            }
         }
        */
         T1[i].TotalDemand= T2[i].TotalDemand;
         T1[i].TotalWait  = T2[i].TotalWait;
         T1[i].TotalDist  = T2[i].TotalDist;
         T1[i].TotalIdle  = T2[i].TotalIdle;
         T1[i].TotalLate  = T2[i].TotalLate;
         T1[i].Length     = T2[i].Length;
     }
     return;
}
//---------------------------------------------------------------------------
void SAAVRP::Deletion(Tour *&TR, int v, int start, int end)
{
     int i;
     for(i=start;i<=TR[v].Length-(end-start+1);i++)
         memcpy( &TR[v].PortTrip[i]            ,  \
                 &TR[v].PortTrip[i+end-start+1], sizeof(struct Trip));

     TR[v].Length = TR[v].Length-(end-start+1);
     Update_Last_ServiceTimeVehicle(start,TR,v);
     return;
}
//---------------------------------------------------------------------------
void SAAVRP::Insertion(Tour *&TR, int vI, int iSpot, int vD, int start, int end)
{
     int i;
     for(i=(TR[vI].Length - iSpot+1);i>=0;i--)
         memcpy(&TR[vI].PortTrip[iSpot+(end-start+1)+i] ,                 \
                &TR[vI].PortTrip[iSpot+i]               , sizeof(struct Trip));
                                                  // (end-start+1)*sizeof(struct Trip));

     memcpy(&TR[vI].PortTrip[iSpot] ,                     \
            &TR[vD].PortTrip[start] , (end-start+1)*sizeof(struct Trip));

     TR[vI].Length = TR[vI].Length+(end-start+1);

     if(TR[vI].PortTrip[iSpot].Load )
        Update_Load_Vehicle(iSpot,TR,vI);

     Update_Last_ServiceTimeVehicle(iSpot,TR,vI);
}
//---------------------------------------------------------------------------
int SAAVRP::Move(int &i1, int &i2,int MaxIterations,int NIterations)
{	//STRING RELOCATION
     //randomly select two vehicles i and j from the list
     srand( (unsigned)time( NULL ) );
     int    InsP, StringLength, StrP, EndP,nIterations;
     double Dist;

     nIterations = NIterations;
     CopyTour(TempT,TAGV);

     do //  Choose a Cluster of any vehicle for deletion - must be consistent
     {
       nIterations++;
       i1 =  rand() % NumVehicles;

       do
         StringLength =  rand() % (TAGV[i1].Length+1);  // length of cluster
       while(StringLength >= sLimit);

       StrP =  rand() % (TAGV[i1].Length - StringLength+ 1); // Starting Point
       EndP =  StrP   + StringLength - 1;                       // End      Point
       i2   =  rand() % NumVehicles;//chose for insertion
       InsP =  rand() % (TAGV[i2].Length+1);
    //   Dist =  abs (TAGV[i1].PortTrip[StrP].LocToVisit-TAGV[i2].PortTrip[InsP].LocToVisit);
     } while( (Feasible(TempT,i1,i2,StrP,EndP,InsP) == 0) &&  (nIterations != MaxIterations) );

     //         ( Dist > dLimit);
     //i2 = 1;  InsP = 1;
     //i1 = 0;  StrP = 0;  EndP = 1;
     if ( nIterations < MaxIterations)
     {
          Insertion(TempT, i2, InsP , i1, StrP, EndP);
          Deletion (TempT, i1, StrP , EndP);
     }
     return(nIterations);
}
//---------------------------------------------------------------------------
int SAAVRP::Feasible(Tour *&TR, int i1, int i2,int S,int E,int I)
{
    //check capacity constraint
    int i,j;

    if(i1 == i2 || S >= E)
       return (0);

    if(TR[i1].PortTrip[S  ].Load != ONE  ||
       TR[i1].PortTrip[E  ].Load != ZERO ||
      (TR[i1].PortTrip[S-1].Load != ZERO && S >= 1))
       return(0);

    if(TR[i2].PortTrip[I-1].Load >= CAPACITY && I >= 1)
       return(0);

    int Max_L1 = 0;
    for(j=S;j<=E;j++)
        Max_L1 = max (Max_L1, TR[i1].PortTrip[j].Load);

    int Max_L2 = 0;
    for(j=I;j < TR[i2].Length;j++)
    {
        if(TR[i2].PortTrip[j].Load == 0 ) break;
        Max_L2 = max (Max_L2, TR[i2].PortTrip[j].Load);
    }

  //  if(Max_L1 + TR[i2].PortTrip[I].Load > CAPACITY)

    if(Max_L1 + Max_L2 > CAPACITY)
        return(0);
    if(max (Max_L1 , Max_L2) > 2)
       int FFF = 1;
    return (1);	// Every thing is OK, so it is possible
}
//---------------------------------------------------------------------------
void SAAVRP::CalcValues(Tour *&TR)
{
   int TWV, j,v,l,Max_S;
   for(v=0;v<NumVehicles;v++)
   {
       TR[v].TotalWait = TR[v].TotalDist = TR[v].TotalIdle = TR[v].TotalLate  =  0;

       if( TR[v].Length == 0 ) continue;

       TR[v].TotalDist += Get_Time_Between_Two_Points(PortName,AGV[v].StartLoc,
                                                                TR[v].PortTrip[0].LocToVisit);
       if(TR[v].Length == 2)
          TWV =  0;
       else
          TWV =  TWindowVehicle;
       String FirstLoc = TR[v].PortTrip[0].LocToVisit;
       if(FirstLoc.SubString(1,3) == "W/P" )
       {
          if(CJob[TR[v].PortTrip[0].Contents[0].JobNo-1].AppointmentTime
               >  TR[v].PortTrip[0].VehicleTime )
                  TR[v].TotalWait +=
             CJob[TR[v].PortTrip[0].Contents[0].JobNo-1].AppointmentTime
                - TR[v].PortTrip[0].VehicleTime + TWV;
          else
                  TR[v].TotalLate +=
                  TR[v].PortTrip[0].VehicleTime -
             CJob[TR[v].PortTrip[0].Contents[0].JobNo-1].AppointmentTime;
       }


       for(j=1;j<TR[v].Length;j++)
       {
           if(j >= TR[v].Length - 2 )
              TWV =  0;
           else
              TWV =  TWindowVehicle;
           for(l=0;l<CAPACITY;l++)
           {
               if(TR[v].PortTrip[j].Contents[l].JobNo == 0 ) break;  // 30/04/2004
               if(     TR[v].PortTrip[j].LocToVisit ==
                  CJob[TR[v].PortTrip[j].Contents[l].JobNo-1].SourceLocation &&
                  CJob[TR[v].PortTrip[j].Contents[l].JobNo-1].UseConstraintTime == PICKUPTIME)
               {
                  if(CJob[TR[v].PortTrip[j].Contents[l].JobNo-1].AppointmentTime
                       >  TR[v].PortTrip[j].VehicleTime )
                          TR[v].TotalWait +=
                     CJob[TR[v].PortTrip[j].Contents[l].JobNo-1].AppointmentTime
                        - TR[v].PortTrip[j].VehicleTime + TWV;
                  else
                          TR[v].TotalLate +=
                          TR[v].PortTrip[j].VehicleTime -
                     CJob[TR[v].PortTrip[j].Contents[l].JobNo-1].AppointmentTime;

               }

               if(     TR[v].PortTrip[j].LocToVisit ==
                  CJob[TR[v].PortTrip[j].Contents[l].JobNo-1].DestLocation &&
                  CJob[TR[v].PortTrip[j].Contents[l].JobNo-1].UseConstraintTime == DELIVRTIME)
               {
                  if(CJob[TR[v].PortTrip[j].Contents[l].JobNo-1].AppointmentTime
                       >  TR[v].PortTrip[j].VehicleTime )
                          TR[v].TotalWait +=
                     CJob[TR[v].PortTrip[j].Contents[l].JobNo-1].AppointmentTime
                        - TR[v].PortTrip[j].VehicleTime +  TWV;
                  else
                          TR[v].TotalLate +=
                          TR[v].PortTrip[j].VehicleTime -
                     CJob[TR[v].PortTrip[j].Contents[l].JobNo-1].AppointmentTime;

               }
           }
           String LastLoc = TR[v].PortTrip[j].LocToVisit;
           if(LastLoc.SubString(1,3) == "W/P"  || (j < TR[v].Length-1) )
              TR[v].TotalDist += Get_Time_Between_Two_Points(PortName,TR[v].PortTrip[j-1].LocToVisit,
                                                                      TR[v].PortTrip[j  ].LocToVisit);

       }
   }
}
//---------------------------------------------------------------------------
void SAAVRP::CalcValues(Tour *&TR, int i1, int i2)
{
   int TWV, i,j,v,l;
   for(i=1;i<=2;i++)
   {
       if(i==1) v =  i1;
       if(i==2) v =  i2;
       TR[v].TotalWait = TR[v].TotalDist = TR[v].TotalIdle = TR[v].TotalLate  =  0;

       if( TR[v].Length == 0 ) continue;

       TR[v].TotalDist += Get_Time_Between_Two_Points(PortName,AGV[v].StartLoc,
                                                                TR[v].PortTrip[0].LocToVisit);

       if(TR[v].Length == 2 )
          TWV =  0;
       else
          TWV =  TWindowVehicle;
       String FirstLoc = TR[v].PortTrip[0].LocToVisit;
       if(FirstLoc.SubString(1,3) == "W/P" )
       {
          if(CJob[TR[v].PortTrip[0].Contents[0].JobNo-1].AppointmentTime
               >  TR[v].PortTrip[0].VehicleTime )
                  TR[v].TotalWait +=
             CJob[TR[v].PortTrip[0].Contents[0].JobNo-1].AppointmentTime
                - TR[v].PortTrip[0].VehicleTime + TWV;
          else
                  TR[v].TotalLate +=
                  TR[v].PortTrip[0].VehicleTime -
             CJob[TR[v].PortTrip[0].Contents[0].JobNo-1].AppointmentTime;
       }


       for(j=1;j<TR[v].Length;j++)
       {
           if(j >= TR[v].Length - 2)
              TWV =  0;
           else
              TWV =  TWindowVehicle;
           for(l=0;l<CAPACITY;l++)
           {
               if(TR[v].PortTrip[j].Contents[l].JobNo == 0 ) break;  // 26/04/2004

               if(     TR[v].PortTrip[j].LocToVisit ==
                  CJob[TR[v].PortTrip[j].Contents[l].JobNo-1].SourceLocation &&
                  CJob[TR[v].PortTrip[j].Contents[l].JobNo-1].UseConstraintTime == PICKUPTIME)
               {
                  if(CJob[TR[v].PortTrip[j].Contents[l].JobNo-1].AppointmentTime
                       >  TR[v].PortTrip[j].VehicleTime )
                          TR[v].TotalWait +=
                     CJob[TR[v].PortTrip[j].Contents[l].JobNo-1].AppointmentTime
                        - TR[v].PortTrip[j].VehicleTime + TWV;
                  else
                          TR[v].TotalLate +=
                          TR[v].PortTrip[j].VehicleTime -
                     CJob[TR[v].PortTrip[j].Contents[l].JobNo-1].AppointmentTime;

               }

               if(     TR[v].PortTrip[j].LocToVisit ==
                  CJob[TR[v].PortTrip[j].Contents[l].JobNo-1].DestLocation &&
                  CJob[TR[v].PortTrip[j].Contents[l].JobNo-1].UseConstraintTime == DELIVRTIME )
               {
                  if(CJob[TR[v].PortTrip[j].Contents[l].JobNo-1].AppointmentTime
                       >  TR[v].PortTrip[j].VehicleTime )
                          TR[v].TotalWait +=
                     CJob[TR[v].PortTrip[j].Contents[l].JobNo-1].AppointmentTime
                        - TR[v].PortTrip[j].VehicleTime + TWV;
                  else
                          TR[v].TotalLate +=
                          TR[v].PortTrip[j].VehicleTime -
                     CJob[TR[v].PortTrip[j].Contents[l].JobNo-1].AppointmentTime;

               }
           }
           String LastLoc = TR[v].PortTrip[j].LocToVisit;
           if(LastLoc.SubString(1,3) == "W/P"  || (j < TR[v].Length-1) )
              TR[v].TotalDist += Get_Time_Between_Two_Points(PortName,TR[v].PortTrip[j-1].LocToVisit
                                                                     ,TR[v].PortTrip[j  ].LocToVisit);
       }
   }
}
//---------------------------------------------------------------------------
void SAAVRP::SimulatedAnnealing(ofstream &fOut, ofstream &fOutB, ofstream &fOutX)
{
     srand( (unsigned)time( NULL ) );
     int     NIterations =  0,NTempIterations,NFeasible,NFeasibleA, v1, v2;
     int     ImproveAccept,NonImproveAccept, BestI=0;
     double  r,Temp,Diff, ObjT;

     CopyTour(BestT,TAGV);
  /*
     fOut<<"\nCopy to Best:";
     PrintInfo(fOut,BestT);
     fOut.flush ();
   */

     Temp  = tInitial;
     do
     {
     	NTempIterations =  NFeasible        =  NFeasibleA =  0;
     	ImproveAccept   =  NonImproveAccept =  0;
     	do
     	{
     	   NIterations = Move(v1,v2,MaxIterations,NIterations);

           fOut<<"\nAfter Move:"<<NIterations;
           CalcValues(TempT,v1,v2);
           PrintTripInfo(fOut,TempT);
           fOut.flush ();

     	   NFeasible++;
     	   if(NIterations < MaxIterations)
     	   {
     	      CalcValues(TempT,v1,v2);
     	      ObjT =   ObjectiveFunction(TAGV);
     	      Diff =   ObjectiveFunction(TempT) - ObjT;
     	      r    =   double(rand())/RAND_MAX;	//U[0,1]
     	      if(Diff < 0)
     	      {
     	      	CopyTour(TAGV,TempT);
     	      	NFeasibleA++;
     	      	ImproveAccept++;
     	      	if(ObjT < ObjectiveFunction(BestT))
     	      	   CopyTour(BestT,TAGV);
     	      }else
              if(r < exp(-Diff/Temp))
     	      {
     	          CopyTour(TAGV,TempT);
	          NFeasibleA++; //keep count of how many non-improving moves its allowing vs improving moves
                  NonImproveAccept++;
	      }
      	      NTempIterations++;
           }
	} while((NTempIterations != MaxTChange) && (NIterations != MaxIterations));

	Temp =  rCooling*Temp;
        BestI++;
        PrintTripInfo(fOutB,BestT);
       	// PrintInfo(fOut,TAGV ) ; // BestT);
	fOut<<"\nTemp ="<<Temp<<" Objective Value of BestT = "<< ObjectiveFunction(BestT);
	fOut<<" Accept="<<ImproveAccept<<" NonImproveAccept="<<NonImproveAccept;
	fOutX<<"\n"<< ObjectiveFunction(BestT);
     } while( (Temp > tEnd)&& (NIterations != MaxIterations));

      PrintTripInfo(fOutB,BestT);
      // PrintInfo(fOut,TAGV ) ; // BestT);
      fOutB<<"\n"<<NumVehicles;
      PrintTour(fOutB,BestT);
      fOutB<<"\n"<< ObjectiveFunction(BestT)<<"\t BestI="<<BestI;
}
//---------------------------------------------------------------------------

