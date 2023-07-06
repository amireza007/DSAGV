//---------------------------------------------------------------------------

#include <vcl.h>
#include "Splash.h"
#pragma hdrstop
USERES("PortApp1_0_5.res");
USEFORM("Main1_0_5.cpp", MainForm);
USEFORM("MCFModel1_3.cpp", MCFAlgorithmForm);
USEFORM("OpenPort.cpp", OpenPortForm);
USEFORM("PortAbout.cpp", AboutForm);
USEFORM("PortAGV.cpp", PortAGVForm);
USEFORM("PortContainer.cpp", PortContainerForm);
USEFORM("PortLayout.cpp", PortLayoutForm);
USEFORM("Splash.cpp", SplashForm);
USEUNIT("PBEAMPP1.C");
USEUNIT("PBEAMPP2.C");
USEUNIT("PBEAMPP3.C");
USEUNIT("PBEAMPP4.C");
USEUNIT("PBLA1_3.C");
USEUNIT("PFLOWUP.C");
USEUNIT("MCFLIGHT1_3.C");
USEUNIT("TREEUP.C");
USEUNIT("PSTART.C");
USEUNIT("PSIMPLEX1_3.C");
USEUNIT("Mcfutil.c");
USEUNIT("OUTPUT.C");
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
                 Application->Title = "Dynamic Scheduling of Automated Guided Vehicles";
                 Application->CreateForm(__classid(TMainForm), &MainForm);
                 Application->CreateForm(__classid(TMCFAlgorithmForm), &MCFAlgorithmForm);
                 Application->CreateForm(__classid(TOpenPortForm), &OpenPortForm);
                 Application->CreateForm(__classid(TAboutForm), &AboutForm);
                 Application->CreateForm(__classid(TPortAGVForm), &PortAGVForm);
                 Application->CreateForm(__classid(TPortContainerForm), &PortContainerForm);
                 Application->CreateForm(__classid(TPortLayoutForm), &PortLayoutForm);
                 Application->CreateForm(__classid(TSplashForm), &SplashForm);
                 Application->Run();
        }
        catch (Exception &exception)
        {
                 Application->ShowException(&exception);
        }
        return 0;
}
//---------------------------------------------------------------------------
