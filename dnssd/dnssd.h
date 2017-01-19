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

#if defined(DNSSD_EXPORT)
#define DNSSD_API extern "C" __declspec(dllexport)
#else
#define DNSSD_API extern "C" __declspec(dllimport)
#endif

namespace dnssd_uwp
{
    enum DnssdServiceUpdateType { ServiceAdded, ServiceUpdated, ServiceRemoved, EnumerationComplete };

    enum DnssdErrorType {
        DNSSD_NO_ERROR = 0,                         //no error
        DNSSD_WINDOWS_RUNTIME_ERROR,                // unable to initialize Windows Runtime
        DNSSD_WINDOWS_VERSION_ERROR,                // version of Windows does not support Windows::Networking::ServiceDiscovery::Dnssd api
        DNSSD_PORTWATCHER_INITIALIZATION_ERROR,     // error initializing dnssd service watcher
        DNSSD_INVALID_PARAMETER_ERROR,
        DNSSD_MEMORY_ERROR, 
        DNSSD_UNSPECIFIED_ERROR
    };

    typedef void* DnssdPtr;
    typedef void* DnssdServiceWatcherPtr;
    typedef void* DnssdInPortPtr;

    // dnssd service info
    typedef struct 
    {
        const char* id;
        const char* instanceName;
        const char* host;
        const char* port;
    } DnssdServiceInfo;

    typedef DnssdServiceInfo* DnssdServiceInfoPtr;

    // dnssd service changed callback
    typedef void(*DnssdServiceChangedCallback) (const DnssdServiceWatcherPtr portWatcher, DnssdServiceUpdateType update, DnssdServiceInfoPtr info);

    // dnssd service Functions
    typedef DnssdErrorType(__cdecl *DnssdInitializeFunc)(DnssdServiceChangedCallback callback, DnssdPtr* dnssdPtr);
    DNSSD_API DnssdErrorType __cdecl dnssd_initialize(DnssdServiceChangedCallback callback, DnssdPtr* dnssdPtr);
 
    typedef void(__cdecl *DnssdFreeFunc)(DnssdPtr dnssdPtr);
    DNSSD_API void __cdecl dnssd_free(DnssdPtr dnssdPtr);

    typedef const DnssdServiceWatcherPtr(__cdecl *DnssdGetPortWatcherFunc)(DnssdPtr dnssdPtr);
    DNSSD_API const DnssdServiceWatcherPtr __cdecl dnssd_get_servicewatcher(DnssdPtr dnssdPtr);

};
 