//---------------------------------------------------------------------------

// #include <vcl.h>
#include "Splash.h"
#pragma hdrstop
//---------------------------------------------------------------------------
USEFORM("PortLayout.cpp", PortLayoutForm);
USEFORM("OpenPort.cpp", OpenPortForm);
USEFORM("PortAbout.cpp", AboutForm);
USEFORM("PortContainer.cpp", PortContainerForm);
USEFORM("MCFModel1_3.cpp", MCFAlgorithmForm);
USEFORM("PortAGV.cpp", PortAGVForm);
USEFORM("Main.cpp", MainForm);
USEFORM("Splash.cpp", SplashForm);
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
                 Application->CreateForm(__classid(TPortLayoutForm), &PortLayoutForm);
                 Application->CreateForm(__classid(TOpenPortForm), &OpenPortForm);
                 Application->CreateForm(__classid(TPortContainerForm), &PortContainerForm);
                 Application->CreateForm(__classid(TMCFAlgorithmForm), &MCFAlgorithmForm);
                 Application->CreateForm(__classid(TPortAGVForm), &PortAGVForm);
                 Application->CreateForm(__classid(TAboutForm), &AboutForm);
                 SplashForm->Hide();
                 SplashForm->Close();
                 Application->Run();
        }
        catch (Exception &exception)
        {
                 Application->ShowException(&exception);
        }
        catch (...)
        {
                 try
                 {
                         throw Exception("");
                 }
                 catch (Exception &exception)
                 {
                         Application->ShowException(&exception);
                 }
        }
        return 0;
}
//---------------------------------------------------------------------------
