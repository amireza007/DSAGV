//---------------------------------------------------------------------------

#include <vcl.h>
#include "Splash.h"
#pragma hdrstop
USERES("PortApp1_0_10.res");
USEFORM("Main1_0_5.cpp", MainForm);
USEFORM("MCFModel1_3.cpp", MCFAlgorithmForm);
USEFORM("OpenPort.cpp", OpenPortForm);
USEFORM("PortAbout.cpp", AboutForm);
USEFORM("PortAGV.cpp", PortAGVForm);
USEFORM("PortContainer.cpp", PortContainerForm);
USEFORM("PortLayout.cpp", PortLayoutForm);
USEFORM("Splash.cpp", SplashForm);
USEFORM("PortBenchmark.cpp", BenchOptionForm);
USEUNIT("PBEAMPP4.cpp");
USEUNIT("PBEAMPP1.cpp");
USEUNIT("PBEAMPP2.cpp");
USEUNIT("PBEAMPP3.cpp");
USEUNIT("Dijkstra.cpp");
USEUNIT("PFLOWUP.cpp");
USEUNIT("TREEUP.cpp");
USEUNIT("READMIN.Cpp");
USEUNIT("PREPAIR.cpp");
USEUNIT("OUTPUT.cpp");
USEUNIT("PSIMPLEX1_3.cpp");
USEUNIT("Mcfutil.cpp");
USEUNIT("PBLA1_3.cpp");
USEUNIT("MCFLIGHT1_0_6.Cpp");
USEUNIT("PSTART.cpp");
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
        try
        {
                 SplashForm = new TSplashForm(Application);
                 SplashForm->Show();
                 SplashForm->Update();
                 Sleep(2000);

                 Application->Initialize();
                 Application->Title = "Static and Dynamic Scheduling of Automated Guided Vehicles";
                 Application->CreateForm(__classid(TMainForm), &MainForm);
                 Application->CreateForm(__classid(TMCFAlgorithmForm), &MCFAlgorithmForm);
                 Application->CreateForm(__classid(TOpenPortForm), &OpenPortForm);
                 Application->CreateForm(__classid(TAboutForm), &AboutForm);
                 Application->CreateForm(__classid(TPortAGVForm), &PortAGVForm);
                 Application->CreateForm(__classid(TPortContainerForm), &PortContainerForm);
                 Application->CreateForm(__classid(TPortLayoutForm), &PortLayoutForm);
                 Application->CreateForm(__classid(TSplashForm), &SplashForm);
                 Application->CreateForm(__classid(TBenchOptionForm), &BenchOptionForm);
                 Application->Run();
        }
        catch (Exception &exception)
        {
                 Application->ShowException(&exception);
        }
        return 0;
}
//---------------------------------------------------------------------------
