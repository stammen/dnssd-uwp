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

#pragma once

#include "dnssd.h"
#include <string>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace dnssd_uwp
{
    class DnssdClient 
    {
    public:
        DnssdClient();
        ~DnssdClient();

        DnssdErrorType InitializeDnssd();
        DnssdErrorType InitializeDnssdServiceWatcher(const std::string& serviceName, const std::string& port, DnssdServiceChangedCallback callback);
        DnssdErrorType InitializeDnssdService(const std::string& serviceName, const std::string& port);

    private:
        // Dnssd DLL function pointers
        DnssdInitializeFunc             mDnssdInitFunc;
        DnssdCreateServiceWatcherFunc   mDnssdCreateServiceWatcherFunc;
        DnssdFreeServiceWatcherFunc     mDnssdFreeServiceWatcherFunc;
        DnssdCreateServiceFunc          mDnssdCreateServiceFunc;
        DnssdFreeServiceFunc            mDnssdFreeServiceFunc;

        // dnssd service
        DnssdServicePtr mDnssdServicePtr;

        // dnssd service watcher
        DnssdServiceWatcherPtr mDnssdServiceWatcherPtr;

        // dnssd DLL Handle
        HINSTANCE mDllHandle;
    };
};

