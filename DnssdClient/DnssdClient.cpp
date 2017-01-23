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

#include "dnssd.h"
#include "DnssdClient.h"

using namespace dnssd_uwp;
using namespace std;

DnssdClient::DnssdClient()
{
    mDnssdInitFunc = nullptr;
    mDnssdCreateServiceWatcherFunc = nullptr;
    mDnssdFreeServiceWatcherFunc = nullptr;
    mDnssdCreateServiceFunc = nullptr;
    mDnssdFreeServiceFunc = nullptr;
    mDnssdServicePtr = nullptr;
    mDnssdServiceWatcherPtr = nullptr;
    mDllHandle = NULL;
}

DnssdClient::~DnssdClient()
{
    if (mDnssdFreeServiceFunc && mDnssdServicePtr)
    {
        mDnssdFreeServiceFunc(mDnssdServicePtr);
    }

    if (mDnssdFreeServiceWatcherFunc && mDnssdServiceWatcherPtr)
    {
        mDnssdFreeServiceWatcherFunc(mDnssdServiceWatcherPtr);
    }

    //Free the library:
    if (mDllHandle)
    {
        FreeLibrary(mDllHandle);
    }
}

DnssdErrorType DnssdClient::InitializeDnssd()
{
    DnssdErrorType result = DNSSD_NO_ERROR;

    if (mDllHandle == NULL)
    {
        mDllHandle = LoadLibrary(L"Dnssd.dll");
        if (NULL == mDllHandle)
        {
            result = DNSSD_DLL_MISSING_ERROR;
            goto cleanup;
        }
    }

    // GetDnssd DLL function pointers. Error checking needs to be added!
    //Get pointer to the DnssdInitializeFunc function using GetProcAddress:  
    mDnssdInitFunc = reinterpret_cast<DnssdInitializeFunc>(::GetProcAddress(mDllHandle, "dnssd_initialize"));

    //Get pointer to the DnssdFreeServiceWatcherFunc function using GetProcAddress:  
    mDnssdFreeServiceWatcherFunc = reinterpret_cast<DnssdFreeServiceWatcherFunc>(::GetProcAddress(mDllHandle, "dnssd_free_service_watcher"));

    //Get pointer to the DnssdCreateServiceWatcherFunc function using GetProcAddress:  
    mDnssdCreateServiceWatcherFunc = reinterpret_cast<DnssdCreateServiceWatcherFunc>(::GetProcAddress(mDllHandle, "dnssd_create_service_watcher"));

    //Get pointer to the DnssdFreeServiceFunc function using GetProcAddress:  
    mDnssdFreeServiceFunc = reinterpret_cast<DnssdFreeServiceFunc>(::GetProcAddress(mDllHandle, "dnssd_free_service"));

    //Get pointer to the DnssdCreateServiceFunc function using GetProcAddress:  
    mDnssdCreateServiceFunc = reinterpret_cast<DnssdCreateServiceFunc>(::GetProcAddress(mDllHandle, "dnssd_create_service"));

    // initialize dnssd interface
    result = mDnssdInitFunc();
    if (result != DNSSD_NO_ERROR)
    {
        goto cleanup;
    }

cleanup:
    return result;
}

DnssdErrorType DnssdClient::InitializeDnssdServiceWatcher(const std::string& serviceName, const std::string& port, DnssdServiceChangedCallback callback)
{
    // create a dns service watcher
    DnssdErrorType result = mDnssdCreateServiceWatcherFunc(serviceName.c_str(), callback, &mDnssdServiceWatcherPtr);
    return result;
}

DnssdErrorType DnssdClient::InitializeDnssdService(const std::string& serviceName, const std::string& port)
{
    // create a dns service 
    DnssdErrorType result = mDnssdCreateServiceFunc(serviceName.c_str(), port.c_str(), &mDnssdServicePtr);
    return result;
}



