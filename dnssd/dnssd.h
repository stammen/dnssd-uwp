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
    enum DnssdServiceUpdateType { ServiceAdded, ServiceUpdated, ServiceRemoved };

    enum DnssdErrorType {
        DNSSD_NO_ERROR = 0,                         // no error
        DNSSD_WINDOWS_RUNTIME_INITIALIZATION_ERROR, // unable to initialize Windows Runtime
        DNSSD_WINDOWS_VERSION_ERROR,                // version of Windows does not support Windows::Networking::ServiceDiscovery::Dnssd api
        DNSSD_SERVICEWATCHER_INITIALIZATION_ERROR,  // error initializing dnssd service watcher
        DNSSD_LOCAL_HOSTNAME_NOT_FOUND_ERROR,       // dns service was not able to find a local hostname
        DNSSD_SERVICE_ALREADY_EXISTS_ERROR,         // dns service has already been started
        DNSSD_SERVICE_INITIALIZATION_ERROR,         // error starting dnssd service 
        DNSSD_INVALID_SERVICE_NAME_ERROR,           // Invalid service name during registration
        DNSSD_SERVICE_SERVER_ERROR,                 // Dnssd server error during registration
        DNSSD_SERVICE_SECURITY_ERROR,               // Dnssd security error during registration
        DNSSD_INVALID_PARAMETER_ERROR,
        DNSSD_MEMORY_ERROR,
        DNSSD_DLL_MISSING_ERROR,                    // dnssd dll not found
        DNSSD_UNSPECIFIED_ERROR
    };

    typedef void* DnssdServiceWatcherPtr;
    typedef void* DnssdServicePtr;

    // dnssd service info
    typedef struct 
    {
        const char* id;
        const char* instanceName;
        const char* host;
        const char* port;
    } DnssdServiceInfo;

    typedef DnssdServiceInfo* DnssdServiceInfoPtr;

    // dnssd functions
    typedef DnssdErrorType(__cdecl *DnssdInitializeFunc)();
    DNSSD_API DnssdErrorType __cdecl dnssd_initialize();

    // dnssd service watcher functions

    // dnssd service watcher changed callback
    typedef void(*DnssdServiceChangedCallback) (const DnssdServiceWatcherPtr portWatcher, DnssdServiceUpdateType update, DnssdServiceInfoPtr info);

    // dnssd service watcher create function
    typedef  DnssdErrorType(__cdecl *DnssdCreateServiceWatcherFunc)(const char* serviceName, DnssdServiceChangedCallback callback, DnssdServiceWatcherPtr *serviceWatcher);
    DNSSD_API DnssdErrorType __cdecl dnssd_create_service_watcher(const char* serviceName, DnssdServiceChangedCallback callback, DnssdServiceWatcherPtr * serviceWatcher);

    typedef void(__cdecl *DnssdFreeServiceWatcherFunc)(DnssdServiceWatcherPtr serviceWatcher);
    DNSSD_API void __cdecl dnssd_free_service_watcher(DnssdServiceWatcherPtr serviceWatcher);

    // dnssd service create function
    typedef  DnssdErrorType(__cdecl *DnssdCreateServiceFunc)(const char* serviceName, const char* port, DnssdServicePtr *service);
    DNSSD_API DnssdErrorType __cdecl dnssd_create_service(const char* serviceName, const char* port, DnssdServicePtr *service);

    typedef void(__cdecl *DnssdFreeServiceFunc)(DnssdServicePtr service);
    DNSSD_API void __cdecl dnssd_free_service(DnssdServicePtr service);


};
 