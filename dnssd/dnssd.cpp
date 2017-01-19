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
#include "DnssdImpl.h"
#include "DnssdServiceWatcher.h"
#include <wrl\wrappers\corewrappers.h>

namespace dnssd_uwp
{
    DnssdErrorType dnssd_initialize(DnssdServiceChangedCallback callback, DnssdPtr* dnssdPtr)
    {
        *dnssdPtr = nullptr;

        // Initialize the Windows Runtime.
        static Microsoft::WRL::Wrappers::RoInitializeWrapper initialize(RO_INIT_MULTITHREADED);

        if (!SUCCEEDED(initialize.operator HRESULT()))
        {
            return DNSSD_WINDOWS_RUNTIME_ERROR;
        }

#if 0
        // Check if Windows 10 dnssd api is supported
        if (!Windows::Foundation::Metadata::ApiInformation::IsTypePresent("Windows.Networking.ServiceDiscovery"))
        {
            return DNSSD_WINDOWS_VERSION_ERROR;
        }
#endif

        // attempt to initialize the dnssd service watcher
        Dnssd* d = new Dnssd(callback);
        DnssdErrorType result = d->Initialize();
        if(result != DNSSD_NO_ERROR)
        {
            delete d;
            *dnssdPtr = nullptr;
        }
        else
        {
            *dnssdPtr = (DnssdPtr)d;
        }

        return result;
    }

    void dnssd_free(DnssdPtr dnssdPtr)
    {
        if (dnssdPtr)
        {
            delete dnssdPtr;
        }
    }

    const DnssdServiceWatcherPtr dnssd_get_servicewatcher(DnssdPtr dnssdPtr)
    {
        Dnssd* d = (Dnssd*)dnssdPtr;
        return d->GetServiceWatcherWrapper();
    }

}

