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
DnssdInitializeFunc             gDnssdInitFunc = nullptr;
DnssdCreateServiceWatcherFunc   gDnssdCreateServiceWatcherFunc = nullptr;
DnssdFreeServiceWatcherFunc     gDnssdFreeServiceWatcherFunc = nullptr;
DnssdCreateServiceFunc          gDnssdCreateServiceFunc = nullptr;
DnssdFreeServiceFunc            gDnssdFreeServiceFunc = nullptr;

static const std::string gServiceName = "_daap._tcp";
static const std::string gServicePort = "3689";

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
    DnssdServiceWatcherPtr dnssdServiceWatcherPtr = nullptr;
    DnssdServicePtr dnssdServicePtr = nullptr;

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

    //Get pointer to the DnssdFreeServiceWatcherFunc function using GetProcAddress:  
    gDnssdFreeServiceWatcherFunc = reinterpret_cast<DnssdFreeServiceWatcherFunc>(::GetProcAddress(dllHandle, "dnssd_free_service_watcher"));

    //Get pointer to the DnssdCreateServiceWatcherFunc function using GetProcAddress:  
    gDnssdCreateServiceWatcherFunc = reinterpret_cast<DnssdCreateServiceWatcherFunc>(::GetProcAddress(dllHandle, "dnssd_create_service_watcher"));
    
    //Get pointer to the DnssdFreeServiceFunc function using GetProcAddress:  
    gDnssdFreeServiceFunc = reinterpret_cast<DnssdFreeServiceFunc>(::GetProcAddress(dllHandle, "dnssd_free_service"));

    //Get pointer to the DnssdCreateServiceFunc function using GetProcAddress:  
    gDnssdCreateServiceFunc = reinterpret_cast<DnssdCreateServiceFunc>(::GetProcAddress(dllHandle, "dnssd_create_service"));

    // initialize dnssd interface
    DnssdErrorType result = gDnssdInitFunc();
    if(result != DNSSD_NO_ERROR)
    {
        cout << "Unable to initialize Dnssd SDK" << endl;
        goto cleanup;
    }

    // create a dns service watcher
    result = gDnssdCreateServiceWatcherFunc(gServiceName.c_str(), dnssdServiceChangedCallback, &dnssdServiceWatcherPtr);
    if (result != DNSSD_NO_ERROR)
    {
        cout << "Unable to initialize Dnssd service watcher" << endl;
        goto cleanup;
    }

    // create a dns service
    result = gDnssdCreateServiceFunc(gServiceName.c_str(), gServicePort.c_str(), &dnssdServicePtr);
    if (result != DNSSD_NO_ERROR)
    {
        cout << "Unable to initialize Dnssd service" << endl;
        goto cleanup;
    }

cleanup:
    // process until user presses key on keyboard
    cout << "Press any key to exit..." << endl << endl;
    char c = _getch();

    if (gDnssdFreeServiceFunc && dnssdServicePtr)
    {
        gDnssdFreeServiceFunc(dnssdServicePtr);
    }

    if(gDnssdFreeServiceWatcherFunc && dnssdServiceWatcherPtr)
    {
        gDnssdFreeServiceWatcherFunc(dnssdServiceWatcherPtr);
    }

    //Free the library:
    if(dllHandle)
    {
        FreeLibrary(dllHandle);
    }

    DeleteCriticalSection(&gCriticalSection);

    return 0;
}