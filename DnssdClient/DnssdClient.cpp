// ******************************************************************
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THE CODE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH
// THE CODE OR THE USE OR OTHER DEALINGS IN THE CODE.
// ******************************************************************

#include "stdafx.h"
#include "dnssd.h"
#include "WindowsVersionHelper.h"
#include <iostream>
#include <string>
#include <conio.h>
#include <assert.h>

#define USING_APP_MANIFEST
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

using namespace std;
using namespace dnssd_uwp;

CRITICAL_SECTION gCriticalSection;

// Dnssd DLL function pointers
DnssdInitializeFunc     gDnssdInitFunc = nullptr;
DnssdFreeFunc           gDnssdFreeFunc = nullptr;
DnssdGetPortWatcherFunc gDnssdGetServiceWatcher = nullptr;

void dnssdServiceChangedCallback(const DnssdServiceWatcherPtr serviceWatcher, DnssdServiceUpdateType update, DnssdServiceInfoPtr info)
{
    EnterCriticalSection(&gCriticalSection);
    string portName = "In";

    switch (update)
    {
    case ServiceAdded:
        cout << "*** dnssd service added ***" << endl;
        break;

    case ServiceUpdated:
        cout << "*** dnssd service updated ***" << endl;
        break;

    case ServiceRemoved:
        cout << "*** dnssd service removed ***" << endl;
        break;

    case EnumerationComplete:
        cout << "*** dnssd service enumeration complete ***" << endl;
        break;
    }

    if (info != nullptr)
    {
        cout << "name: " << info->instanceName << endl;
        cout << "host: " << info->host << endl;
        cout << "port: " << info->port << endl;
    }
    cout << endl;
    LeaveCriticalSection(&gCriticalSection);
}


int main()
{
    HINSTANCE dllHandle = NULL;
    DnssdPtr dnssdPtr = nullptr;

    //Load the Dnssd dll
#ifdef USING_APP_MANIFEST
    if(windows10orGreaterWithManifest())
    {
        dllHandle = LoadLibrary(L"Dnssd.dll");
    }
#else
    if (windows10orGreater())
    {
        dllHandle = LoadLibrary(L"Dnssd.dll");
    }
#endif

    if(NULL == dllHandle)
    {
        cout << "Unable to load Dnssd.dll" << endl;
        goto cleanup;
    }
  
    InitializeCriticalSection(&gCriticalSection);

    // GetDnssd DLL function pointers. Error checking needs to be added!
    //Get pointer to the DnssdInitializeFunc function using GetProcAddress:  
    gDnssdInitFunc = reinterpret_cast<DnssdInitializeFunc>(::GetProcAddress(dllHandle, "dnssd_initialize"));

    //Get pointer to the DnssdFreeFunc function using GetProcAddress:  
    gDnssdFreeFunc = reinterpret_cast<DnssdFreeFunc>(::GetProcAddress(dllHandle, "dnssd_free"));

    //Get pointer to the DnssdGetPortWatcherFunc function using GetProcAddress:  
    gDnssdGetServiceWatcher = reinterpret_cast<DnssdGetPortWatcherFunc>(::GetProcAddress(dllHandle, "dnssd_get_servicewatcher"));


    // initialize dnssd interface
    DnssdErrorType result = gDnssdInitFunc(dnssdServiceChangedCallback, &dnssdPtr);
    if(result != DNSSD_NO_ERROR)
    {
        cout << "Unable to initialize Dnssd" << endl;
        goto cleanup;
    }

cleanup:
    // process until user presses key on keyboard
    cout << "Press any key to exit..." << endl << endl;
    char c = _getch();

    if(gDnssdFreeFunc && dnssdPtr)
    {
        gDnssdFreeFunc(dnssdPtr);
    }

    //Free the library:
    if(dllHandle)
    {
        FreeLibrary(dllHandle);
    }

    DeleteCriticalSection(&gCriticalSection);

    return 0;
}