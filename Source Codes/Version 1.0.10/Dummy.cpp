        if(deleted_node->right_sibling)
        {  // node 3
           father->child = deleted_node->right_sibling;
           deleted_node->right_sibling->pred = father;
           deleted_node->right_sibling->left_sibling = NULL;
        } else
        {   // node 7
           father->child = deleted_node->child;
           deleted_node->child->left_sibling = deleted_node->left_sibling;
           deleted_node->right_sibling->left_sibling = NULL;

        }
            temp->right_sibling->left_sibling = temp->child->child;


        if( temp->left_sibling )
            temp->left_sibling->right_sibling = temp->right_sibling;
        else
            father->child = temp->right_sibling;

        temp = father;
        father = temp->pred;
    }

    /* Update orientation, flow, subtreesize and basic_arc of
       the child of deleted node     */
    if(deleted_node->child )
    {
       basic_arc_temp = deleted_node->child->basic_arc;
       if(basic_arc_temp->head == deleted_node ||
          basic_arc_temp->tail == deleted_node  )
       {
          for(basic_arc_temp = net->arcs; basic_arc_temp != net->stop_arcs; basic_arc_temp++)
          {
            if(basic_arc_temp->head == deleted_node->pred &&
               basic_arc_temp->tail == deleted_node->child)
            {
               new_basic_arc = basic_arc_temp;
               new_orientation = MCF_UP;
               break;
            }
            if(basic_arc_temp->tail == deleted_node->pred &&
               basic_arc_temp->head == deleted_node->child)
            {
               new_basic_arc = basic_arc_temp;
               new_orientation = MCF_DOWN;
               break;
            }
          }
       }
       temp = deleted_node->child;

       temp->orientation = new_orientation;
       temp->basic_arc = new_basic_arc;
       temp->flow = new_basic_arc->lower  ;

    }



     for(int RecTable3=1;RecTable3<=NoRecTable3;RecTable3++)
     {

          MCFAlgorithmForm->ProgressBar1->Position = 100*RecTable3/NoRecTable3;
          MCFAlgorithmForm->Refresh();

          if(Container_Buff[RecTable3-1].Empty == 1)
          {
             arc++;
             continue;
          }


            for( arc = net.dummy_arcs; n < net.n; arc++, n++ )
            {
               if(arc->tail->number == (j2  * 2  + 1 + Number_Of_AGVs)  ||
                  arc->head->number == (j2  * 2  + 1 + Number_Of_AGVs)  ||
                  arc->tail->number == (j2  * 2  + 2 + Number_Of_AGVs)  ||
                  arc->head->number == (j2  * 2  + 2 + Number_Of_AGVs)   )
                  arc->ident = MCF_DELETED;
            }
            for( arc = net.arcs; arc < net.stop_arcs; arc++ )
            {
              if((arc->tail->number == (j2  * 2  + 1 + Number_Of_AGVs)  ||
                  arc->head->number == (j2  * 2  + 1 + Number_Of_AGVs)  ||
                  arc->tail->number == (j2  * 2  + 2 + Number_Of_AGVs)  ||
                  arc->head->number == (j2  * 2  + 2 + Number_Of_AGVs)) )
                 /* &&
                 (arc->tail->number != (j2  * 2  + 1 + Number_Of_AGVs)  &&
                  arc->head->number != (j2  * 2  + 2 + Number_Of_AGVs)) )
                 */
                  arc->ident = MCF_DELETED;
            }


            int n = 0;
            for( arc = net.dummy_arcs; n < net.n; arc++, n++ )
               if(arc->tail->number == (j2  * 2  + 1 + Number_Of_AGVs)  ||
                  arc->head->number == (j2  * 2  + 1 + Number_Of_AGVs)  ||
                  arc->tail->number == (j2  * 2  + 2 + Number_Of_AGVs)  ||
                  arc->head->number == (j2  * 2  + 2 + Number_Of_AGVs)    )
                  arc->ident = MCF_INSERTED;

            for( arc = net.arcs; arc < net.stop_arcs; arc++ )
               if(arc->tail->number == (j2  * 2  + 1 + Number_Of_AGVs)  ||
                  arc->head->number == (j2  * 2  + 1 + Number_Of_AGVs)  ||
                  arc->tail->number == (j2  * 2  + 2 + Number_Of_AGVs)  ||
                  arc->head->number == (j2  * 2  + 2 + Number_Of_AGVs)    )
               {
                  arc->ident = MCF_INSERTED;
                  if(arc->tail->number == (j2  * 2  + 2 + Number_Of_AGVs) )
                     Update_Arcs_From_CPNodes_To_Any_Other_CNodes(Number_Of_AGVs,Number_Of_Container_Jobs,
                              &Number_Of_ARCs,PortNameStr,fp,0,&net,arc,j2 + 1);
               }
         }
            


     Max_Number_Of_Container_Jobs = StrToInt(Edit1->Text)*StrToInt(Edit6->Text);
     Max_Number_Of_ARCs  =  ( Number_Of_AGVs * Max_Number_Of_Container_Jobs)
                         +  ( Max_Number_Of_Container_Jobs * 2)
                         +    Number_Of_AGVs
                         +  Max_Number_Of_Container_Jobs * (Max_Number_Of_Container_Jobs-1);

     Max_Number_Of_Nodes = Number_Of_AGVs + Max_Number_Of_Container_Jobs*2 + 1;


     
        h2 = Max_Number_Of_ARCs;
        t2 = Max_Number_Of_Nodes;


            if(!AllocateMemoryForNSA)
            {
                Timer1Timer(Sender); // Making the model, solving it and Generating new schedule:
                AllocateMemoryForNSA = true;
            }else




    i =0; j = 0;
    int Number_Of_Dummy_Arcs = net->n + 1;
    stop = net->stop_nodes;
    for( arc = net->dummy_arcs; node < (MCF_node_p)stop; arc++, node++ )
    {
        if(arc->tail->number != removing_node &&
           arc->head->number != removing_node   )
        {
           memcpy(arc2, arc , sizeof(MCF_arc_p));

           memcpy(arc, arc + 1, (Number_Of_Dummy_Arcs - (i + 1 ) )*sizeof(MCF_arc_p));
           arc++;
           j++;
        }
        if(arc->tail->number == removing_node + 1  ||
           arc->head->number == removing_node + 1  )
        {
           memcpy(arc, arc + 1, (Number_Of_Dummy_Arcs - (i + 1 ) )*sizeof(MCF_arc_p));
           arc++;
           j++;
        }
    }
    net->dummy_arcs -= j;


    // Removing the arcs
    i = 0; j = 0;
    for( arc = net->arcs; arc < net->stop_arcs; arc++, i++ )
    {
        if(arc->tail->number == removing_node ||
           arc->head->number == removing_node   )
        {
           memcpy(arc, arc + 1, (net->m - (i + 1 ) )*sizeof(MCF_arc_p));
           arc++;
           j++;
        }
        if(arc->tail->number == removing_node + 1 ||
                arc->head->number == removing_node + 1   )
        {
           memcpy(arc, arc + 1, (net->m - (i + 1 ) )*sizeof(MCF_arc_p));
           arc++;
           j++;
        }
    }
    net->m -= j;
    net->stop_arcs -= j;
#if defined MCF_DEBUG_3
    MCF_write_intermediate2(net);
    return (0);
#endif

    Number_Of_Dummy_Arcs = net->n + 1;
    // we have to considred that given node numbers start from zero (0) to n. the Zero for root or artifical node
    stop = net->stop_nodes; i = 0;
    for( node = net->nodes, node++; node < stop; node++, i++ )
    {
         if(node->number == removing_node )
         {
            // remove_node and its conjucate from the graph
            memcpy(node, node +  2, (net->n - (i + 2 ) )*sizeof(MCF_node_t));
            net->n            -= 2;
            net->stop_nodes   -= 2;
            break;
         }
    }


/******************************************************************************************
    else if( arc_base != arcs || arc_end > stop_arcs )
    {
        putch(7); // This block can be deleted beacuse it will not have any entry.
        for( i=1; i < K+B+1; i++ )
            perm[i]->a = (MCF_arc_p)NULL;
        nr_group = ( (m-1) / K ) + 1;
        group_pos = 0;
        basket_size = 0;
    }

******************************************************************************************/
     FILE *fp2 = fopen(FileName,"r+b");
     while(1)
     {
          if(feof(fp2)) break;
          fgets(buff,500,fp2); buff2 =  buff; Row++;
          if(Row >= 4)
          {
             switch(Row)
             {
                 case 4:b3= buff2.SubString(buff2.AnsiPos("needed")+6,buff2.AnsiPos("Iteration")-buff2.AnsiPos("needed")-7);
                        // Num_Iteration = StrToInt(b3); //
                        Num_Iteration = net.iterations;
                        // b3= buff2.SubString(buff2.AnsiPos("in")+2,buff2.AnsiPos("Second")-buff2.AnsiPos("in")-3);
                        // CPU_Time = StrToInt(b3);
                        CPU_Time = net.cpu_time ;
                        break;
                 case 5://Objective_Function = StrToInt(buff2.SubString(2,buff2.Length()-3));
                        Objective_Function = net.optcost ;
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
//------------------------------------------------------------------------------
void __fastcall  Read_NSA_Solution_0(char *FileName   ,int Num_Arcs   , int M_AGVs,
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
     unsigned long  Objective_Function;

     int   Quay_Crane_Time_Window,  CN , Number_Of_Cranes;

     int Number_Of_Jobs ; // For Container Jobs
     int Number_Of_Remaining_Jobs;

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
                 case 5:Objective_Function = StrToInt(buff2.SubString(2,buff2.Length()-3));
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
       PortName = MCFAlgorithmForm->ListBox1->Items->Strings[MCFAlgorithmForm->ListBox1->ItemIndex];
       Time_Window_Vehicle = StrToInt(MCFAlgorithmForm->Edit5->Text);
     }
     else
     {
       PortName = MCFAlgorithmForm->ListBox3->Items->Strings[MCFAlgorithmForm->ListBox3->ItemIndex];
       Time_Window_Vehicle = StrToInt(MCFAlgorithmForm->Edit30->Text);
     }

     if(!Route_Table || !AGV_Con_Table )
     {
         Application->MessageBox("Not Enough Memory !! ", "Error", MB_OK);
         return;
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

     PortAGVForm->Table2->First() ;
     for(int VN=1;VN<=M_AGVs;VN++)
     {
        int NumContainers = PortAGVForm->Table2->FieldValues["NumberOfContainers"] ;
       // int ReadyTime     = PortAGVForm->Table2->FieldValues["ReadyTime"] ;

        int next = *(Route_Table+VN);

        AGVConRec = 0;
        int n=0;

        while(next != N_Nodes)
        {
          int c_node = (next - M_AGVs)/2 + 1;

          next = *(Route_Table+next);
          if( next == N_Nodes) break;
          if( (next - M_AGVs) % 2 == 0 )
              next = *(Route_Table+next);

              AnsiString AGVNameStrS = "AGV-"; AGVNameStrS = AGVNameStrS + StrToInt(VN);

              MCFAlgorithmForm->Table5->Append();

              MCFAlgorithmForm->Table5->FieldValues["Portname"]= PortName;
              MCFAlgorithmForm->Table5->FieldValues["AGVName"] = AGVNameStrS;
              MCFAlgorithmForm->Table5->FieldValues["AGVNumber"]= VN;
              MCFAlgorithmForm->Table5->FieldValues["ContainerID"]  = Container_Buff[c_node-1].IDStr;
              MCFAlgorithmForm->Table5->FieldValues["JobNo"]  = c_node ;// -1

              String SourcePointStr = Container_Buff[c_node-1].SourcePointStr;
              String DestinPointStr = Container_Buff[c_node-1].DestPointStr  ;
              if (SourcePointStr.SubString(1,3)== "W/P")
                  CN = StrToInt(SourcePointStr.SubString(5,SourcePointStr.Length()-4 ));
              else if (DestinPointStr.SubString(1,3)== "W/P")
                  CN = StrToInt(DestinPointStr.SubString(5,DestinPointStr.Length()-4 ));

              MCFAlgorithmForm->Table5->FieldValues["ContainerReadyTime"]= Container_Buff[c_node-1].ReadyTime;
              MCFAlgorithmForm->Table5->FieldValues["SourcePoint"]       = Container_Buff[c_node-1].SourcePointStr;
              MCFAlgorithmForm->Table5->FieldValues["DestPoint"]         = Container_Buff[c_node-1].DestPointStr;

           //   VehicleTime = Vehicle_Buff[VN-1].Last_Completed_Temp ; 28/04/04

              Vehicle_Buff[VN-1].Number_Of_Jobs++; // NSA

              *(AGV_Con_Table + AGVConRec) = c_node;
               AGVConRec ++;

              if( AGVConRec <= 1)
              {
                 int First_Job_For_Vehicle = (*(Route_Table + VN)-M_AGVs) /2 + 1;
                 VehicleTime  = 0;// 28/04/04
                 VehicleTime += *(TimeAGVToC + (VN-1)*Number_Of_Container_Jobs  + First_Job_For_Vehicle - 1 ); // + c_node - 1) ;
              }else
              {
                 int n1 = *(AGV_Con_Table + AGVConRec - 2);
                 int n2 = *(AGV_Con_Table + AGVConRec - 1);
                 VehicleTime = Vehicle_Buff[VN-1].Last_Completed_Temp ; // 28/04/04
                 VehicleTime += * (TimeCToC + (n1-1)*Number_Of_Container_Jobs + n2-1);
              }
              NumContainers ++;

              int QuayCraneTime, ActualTime;

              QuayCraneTime = Crane_Buff[CN-1].Last_Completed_Temp ;
              ActualTime    = max (QuayCraneTime , VehicleTime );

              MCFAlgorithmForm->Table5->FieldValues["VehicleTime"]= VehicleTime;
              MCFAlgorithmForm->Table5->FieldValues["QuayCraneTime"]= QuayCraneTime;
              MCFAlgorithmForm->Table5->FieldValues["ActualTime"]= ActualTime;
              MCFAlgorithmForm->Table5->Post();

              Crane_Buff[CN-1].Last_Completed_Temp = ActualTime + Quay_Crane_Time_Window;
                // NSA
              Vehicle_Buff[VN-1].Last_Completed_Temp = ActualTime + Time_Window_Vehicle;


              if(n++ > Num_Arcs ) break;
        }
        PortAGVForm->Table2->Edit();
        if(staticflag)
        {
             int Cost;
             if(AGVConRec > 0 )
             {
                Cost  = *(CostAGVToC + (VN-1)*Number_Of_Container_Jobs + *(AGV_Con_Table + 0 )-1 ) ;
                for(int k=0;k<AGVConRec-1;k++)
                    Cost += * (CostCToC + (*(AGV_Con_Table + k ) - 1)*Number_Of_Container_Jobs + *(AGV_Con_Table + k + 1 )-1);
             } else
                    Cost = 0;

             PortAGVForm->Table2->FieldValues["CostAGV"] = Cost;
             Number_Of_Remaining_Jobs -= AGVConRec;
             MCFAlgorithmForm->Edit21->Text = Number_Of_Remaining_Jobs;
        }
        PortAGVForm->Table2->FieldValues["NumberOfContainers"] = NumContainers ;
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

     free(Route_Table);
     free(AGV_Con_Table);
}
//------------------------------------------------------------------------------


       if(     TR[v].PortTrip[0].LocToVisit ==
          CJob[TR[v].PortTrip[0].Contents[0].JobNo-1].SourceLocation   &&
          CJob[TR[v].PortTrip[0].Contents[0].JobNo-1].UseConstraintTime == PICKUPTIME)
       {
               if(CJob[TR[v].PortTrip[0].Contents[0].JobNo-1].AppointmentTime >        \
                       TR[v].PortTrip[0].LastServTime                            )
                  TR[v].TotalWait +=  CJob[TR[v].PortTrip[0].Contents[0].JobNo-1].AppointmentTime -  \
                                           TR[v].PortTrip[0].LastServTime;
               else
                  TR[v].TotalLate +=-(CJob[TR[v].PortTrip[0].Contents[0].JobNo-1].AppointmentTime -  \
                                           TR[v].PortTrip[0].LastServTime);

       }


       if(     TR[v].PortTrip[0].LocToVisit ==
          CJob[TR[v].PortTrip[0].Contents[0].JobNo-1].SourceLocation   &&
          CJob[TR[v].PortTrip[0].Contents[0].JobNo-1].UseConstraintTime == PICKUPTIME)

               TR[v].PortTrip[0].LastServTime = max (                 \
          CJob[TR[v].PortTrip[0].Contents[0].JobNo-1].AppointmentTime,\
       	       TR[v].PortTrip[0].ReadyTime                            );

     for(int CN=1;CN<=Maximum_Number_Cranes;CN++)
     {
         Crane_Buff[CN-1].Number_Of_Jobs  = 0;
         Crane_Buff[CN-1].Number_Of_Done_Jobs = 0;
         Crane_Buff[CN-1].Next_Generation_Time= StrToInt(Edit23->Text);
         Crane_Buff[CN-1].Last_Completed_Time = StrToInt(Edit23->Text);
     }


         Vehicle_Buff[VN-1].Number_Of_Jobs  = 0;
         Vehicle_Buff[VN-1].Last_Completed_Time = 0;

         Vehicle_Buff[VN-1].Number_of_Lanes = 0;

         while(MCFAlgorithmForm->Table4->RecordCount > 0) // Delete others
         {
             MCFAlgorithmForm->Table4->First();
             MCFAlgorithmForm->Table4->Delete();
         }

         PortAGVForm->Table2->Edit() ;

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





              locvalues[0] = PortName;
              locvalues[1] = Container_Buff[c_node-1].IDStr;
              locvalues[2] = AGVNameStrS;
              Opts << loCaseInsensitive ;
              b = MCFAlgorithmForm->Table5->Locate("Portname;AGVName;ContainerID", VarArrayOf(locvalues, 2) , Opts);


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




                     //  PortAGVForm->Table2->FieldValues["Status"] = "Collision" ;
             //  int WaitedTime = PortAGVForm->Table2->FieldValues["WaitedTime"];
             //  WaitedTime++;
             //  PortAGVForm->Table2->FieldValues["WaitedTime"] = WaitedTime ;




                if(MCFAlgorithmForm->Table4->RecordCount > 0)
                {
                   MCFAlgorithmForm->Table4->First();
                   AnsiString AGVLocation = PortAGVForm->Table2->FieldValues["StartLocation"];
                   int Junc_D ;
                   int Junc_S      =  Get_Junction(PortNameStr,AGVLocation);
                   String SourceStr= MCFAlgorithmForm->Table4->FieldByName("SourcePoint")->AsString;
                   String DestStr  = MCFAlgorithmForm->Table4->FieldByName("DestPoint")->AsString;
                   if(AGVLocation != SourceStr)
                      Junc_D = Get_Junction(PortNameStr,SourceStr);
                   else
                      Junc_D = Get_Junction(PortNameStr,DestStr  );

                   int Distance = Shortest_Path_Dijkstra(PortNameStr,Junc_S,Junc_D,m );
                   if (Distance < Infinity )
                   {
                     PortAGVForm->Table2->Edit();
                     PortAGVForm->Table2->FieldValues["Lane"] = Vehicle_Buff[m-1].Lane[0];
                     PortAGVForm->Table2->FieldValues["TimeInLane"] = 0;
                     PortAGVForm->Table2->FieldValues["NextLane"] = Vehicle_Buff[m-1].Lane[1];
                     PortAGVForm->Table2->Post();
                  }


//------------------------------------------------------------------
        if(ReadyTimeAGV > 0)
        {
           if(MCFAlgorithmForm->Table4->RecordCount > 0)
           {
              MCFAlgorithmForm->Table4->First();
              AnsiString AGVLocation = PortAGVForm->Table2->FieldValues["StartLocation"];
              int Junc_D ;
              int Junc_S      =  Get_Junction(PortNameStr,AGVLocation);
              String SourceStr= MCFAlgorithmForm->Table4->FieldByName("SourcePoint")->AsString;
              String DestStr  = MCFAlgorithmForm->Table4->FieldByName("DestPoint")->AsString;
              if(AGVLocation != SourceStr)
                 Junc_D = Get_Junction(PortNameStr,SourceStr);
              else
                 Junc_D = Get_Junction(PortNameStr,DestStr  );

              int Distance = Shortest_Path_Dijkstra(PortNameStr,Junc_S,Junc_D,m );
              if (Distance < Infinity )
              {
                PortAGVForm->Table2->Edit();
                PortAGVForm->Table2->FieldValues["Lane"] = Vehicle_Buff[m-1].Lane[0];
                PortAGVForm->Table2->FieldValues["TimeInLane"] = 0;
                PortAGVForm->Table2->FieldValues["NextLane"] = Vehicle_Buff[m-1].Lane[1];
                PortAGVForm->Table2->Post();
             }
           }
            locvalues[0]= AGVLocation;
            locvalues[1]= ReadyTimeAGV;
            bool b = PortAGVForm->Table1->Locate("StartLocation;Readytime", VarArrayOf(locvalues, 1), Opts);

            AnsiString AGVName  = PortAGVForm->Table1->FieldValues["AGVName"];
            AnsiString PortName = PortAGVForm->Table1->FieldValues["PortName"];
            AnsiString AGVNameCurr = "AGV-";  AGVNameCurr = AGVNameCurr + Vehicle_No;

            PortAGVForm->Table2->Edit() ;
            if (b &&  AGVNameCurr != AGVName && PortName == PortNameStr)
            {
                // Update Vehicle time and Actual time: Increase Vehicle Time one second
               if(MCFAlgorithmForm->Table4->RecordCount > 0)
               {
                /* This block now is commented; it should be investigated later to consider collision

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
               */
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

        // Otherwise
        PortAGVForm->Table2->Edit() ;

        // ReadyTimeAGV = 0;



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

     // Graph G;
          // int nodenum, edgenum;

      //G.setsize(nodenum, edgenum);
      int s = 1;
      for (int s=1 ; s<=nodenum ; s++)
      {
        //   G.dijkstra(s);
#if defined DIJKSTRA_DEBUG
           temp_out = fopen( "dij.tmp", "a+t" );
           //                1234567|1234567|1234567
           fprintf(temp_out,"source | dest  |Distance\n");
           fprintf(temp_out,"-------------------------\n");
           for(int d=1;d<=nodenum ;d++)
           {
          //     Distance[s][d] = G.R_Distance(d);
               fprintf(temp_out,"%7d|%7d|%7.0f\n",s,d,Distance[s][d]);
           }
           fclose(temp_out);
#endif





   ///   int q= Myqueue->size;

      j1 = Route_Buff [Vehicle_Buff[m-1].Current_Lane].NextLane1;
      for(i1=1;i1<=Len;i1++)
      {
          Vehicle_Buff[m-1].Zone    [Vehicle_Buff[m-1].Number_of_Zones ] = j1;
          Vehicle_Buff[m-1].Duration[Vehicle_Buff[m-1].Number_of_Zones ] = Route_Buff[j1].Duration;
          Vehicle_Buff[m-1].Number_of_Zones++;
          if ( Route_Buff [j1].Location == Destination )
               return(1);
          else
              j1 = Route_Buff [j1].NextZone1;
      }
      for(i2=i1;i2<=(Port_Buff.NumberOfZones )-1;i2++)
      {
          j1 = Route_Buff [j1].NextZone2;
          Vehicle_Buff[m-1].Zone[Vehicle_Buff[m-1].Number_of_Zones] = j1;
          Vehicle_Buff[m-1].Duration[Vehicle_Buff[m-1].Number_of_Zones] = Route_Buff[j1].Duration;
          Vehicle_Buff[m-1].Number_of_Zones++;

          if ( Route_Buff [j1].Location == Destination )
               return(1);
      }

 