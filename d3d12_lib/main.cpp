#include "pch.h"
#include "HelperFuncs.h"
#include "Context.h"
#include "Log.h"
#include "Win32Application.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    CreateConsole();
    Log::Init();
    Context* testContext = new Context();
    testContext->Test();
    delete testContext;
    return 0; // Win32Application::Run(&sample, hInstance, nCmdShow);
}