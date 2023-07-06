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
                  +  Number_Of_Container_Jobs * (Number_Of_Container_Jobs-1);

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
            //int n = 0;

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
            //int n = 0;

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

     if( Number_Of_Fulfilled_Jobs > Number_Of_New_Jobs )
         int Found = 1;

     MCFAlgorithmForm->Edit28->Text = "Solving The Model";
     MCFAlgorithmForm->GroupBox16->Refresh();

     sprintf(&Fname[0],"%s",BenchOptionForm->Edit1->Text);
     char Fname2[100];
     sprintf(&Fname2[0],"%s",BenchOptionForm->Edit2->Text);

     int stat, balance;

     balance = ( Number_Of_New_Jobs == Number_Of_Fulfilled_Jobs ) ;

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

 