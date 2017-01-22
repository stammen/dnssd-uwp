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
    static bool mInitialized = false;

    DnssdErrorType dnssd_initialize()
    {
        DnssdErrorType result = DNSSD_NO_ERROR;
        HRESULT hr = S_OK;

        // Initialize the Windows Runtime.
        if (!mInitialized)
        {
            HRESULT hr = ::Windows::Foundation::Initialize(RO_INIT_MULTITHREADED);
            if (!SUCCEEDED(hr))
            {
                result = DNSSD_WINDOWS_RUNTIME_INITIALIZATION_ERROR;
            }
            else
            {
                mInitialized = true;
            }
        }
        
        return result;
    }


    DNSSD_API DnssdErrorType dnssd_create_service_watcher(const char* serviceName, DnssdServiceChangedCallback callback, DnssdServiceWatcherPtr *serviceWatcher)
    {
        DnssdErrorType result = DNSSD_NO_ERROR;

        *serviceWatcher = nullptr;

        auto watcher = ref new DnssdServiceWatcher(serviceName, callback);
        result = watcher->Initialize();

        if (result != DNSSD_NO_ERROR)
        {
            *serviceWatcher = nullptr;
            watcher = nullptr;
        }
        else
        {
            auto wrapper = new DnssdServiceWatcherWrapper(watcher);
            *serviceWatcher = (DnssdServiceWatcherPtr)wrapper;
        }

        return result;
    }

    void dnssd_free_service_watcher(DnssdServiceWatcherPtr serviceWatcher)
    {
        if (serviceWatcher)
        {
            DnssdServiceWatcherWrapper* watcher = (DnssdServiceWatcherWrapper*)serviceWatcher;
            delete watcher;
        }
    }
}

