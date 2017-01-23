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
#include "DnssdClient.h"
#include "WindowsVersionHelper.h"
#include <iostream>
#include <string>
#include <conio.h>
#include <assert.h>
#include <memory>

#define USING_APP_MANIFEST
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

using namespace std;
using namespace dnssd_uwp;

CRITICAL_SECTION gCriticalSection;

static const std::string gServiceName = "_daap._tcp";
static const std::string gServicePort = "3689";

std::unique_ptr<DnssdClient> gDnssdClient;

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
        auto cp = GetConsoleOutputCP();
        SetConsoleOutputCP(CP_UTF8);
        wprintf(L"name: %S\n", info->instanceName);
        wprintf(L"host: %S\n", info->host);
        wprintf(L"port: %S\n", info->port);
        wprintf(L"  id: %S\n", info->id);
        SetConsoleOutputCP(cp);
    }
    cout << endl;
    LeaveCriticalSection(&gCriticalSection);
}

BOOL CtrlHandler(DWORD fdwCtrlType)
{
    switch (fdwCtrlType)
    {
    case CTRL_C_EVENT:
    case CTRL_CLOSE_EVENT:
    case CTRL_SHUTDOWN_EVENT:
    case CTRL_LOGOFF_EVENT:
        gDnssdClient.reset();
        gDnssdClient = nullptr;
        return(TRUE);

    case CTRL_BREAK_EVENT:
        return FALSE;

    default:
        return FALSE;
    }
}

int main()
{
    DnssdErrorType result = DNSSD_NO_ERROR;

    gDnssdClient = std::unique_ptr<DnssdClient>(new DnssdClient());

    // add a handler to clean up DnssdClient for various console exit scenarios
    SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlHandler, TRUE);
    cout << "Press any key to exit..." << endl << endl;

    // Initialize the dsssd api
#ifdef USING_APP_MANIFEST
    if(windows10orGreaterWithManifest())
    {
        gDnssdClient->InitializeDnssd();
    }
#else
    if (windows10orGreater())
    {
        gDnssdClient->InitializeDnssd();
    }
#endif

    if(result != DNSSD_NO_ERROR)
    {
        cout << "Unable to initialize dnssd" << endl;
        goto cleanup;
    }
  
    InitializeCriticalSection(&gCriticalSection);

    result = gDnssdClient->InitializeDnssdServiceWatcher(gServiceName, gServicePort, dnssdServiceChangedCallback);
    if (result != DNSSD_NO_ERROR)
    {
        cout << "Unable to initialize dnssd service watcher" << endl;
        goto cleanup;
    }

#if 1
    result = gDnssdClient->InitializeDnssdService(gServiceName, gServicePort);
    if (result != DNSSD_NO_ERROR)
    {
        cout << "Unable to initialize dnssd service" << endl;
        goto cleanup;
    }
#endif // 0


cleanup:
    // process dnssd callbacks until user presses a key on keyboard
    char c = _getch();

    gDnssdClient.reset();
    gDnssdClient = nullptr;
    DeleteCriticalSection(&gCriticalSection);

    return 0;
}