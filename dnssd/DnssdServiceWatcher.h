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

#include <vector>
#include <memory>
#include <string>
#include <functional>
#include <map>
#include <string>

#include "dnssd.h"

namespace dnssd_uwp
{
    std::string PlatformStringToString(Platform::String^ s);
    ref class DnssdServiceWatcher;

    // C++ dsssd service changed callback
    typedef std::function<void(DnssdServiceWatcher^ watcher, DnssdServiceUpdateType update, DnssdServiceInfoPtr info)> DnssdServiceChangedCallbackType;

    // WinRT Delegate
    delegate void DnssdServiceUpdateHandler(DnssdServiceWatcher^ sender, DnssdServiceUpdateType update, DnssdServiceInfoPtr info);

    ref class DnssdServiceInstance sealed
    {
    public:
        DnssdServiceInstance()
        {
            mChanged = false;
            mType = DnssdServiceUpdateType::ServiceAdded;
        }

    internal:
        Platform::String^ mHost;
        Platform::String^ mPort;
        Platform::String^ mInstanceName;
        Platform::String^ mId;
        DnssdServiceUpdateType mType;
        bool mChanged;
    };

    ref class DnssdServiceWatcher
    {
    public:
        virtual ~DnssdServiceWatcher();

    internal:
        DnssdErrorType Initialize();

        void RemoveDnssdServiceChangedCallback() {
            mDnssdServiceChangedCallback = nullptr;
        };

        //event DnssdServiceUpdateHandler^ mPortUpdateEventHander;
        
        // needs to be internal as DnssdServiceChangedCallbackType is not a WinRT type
        void SetDnssdServiceChangedCallback(const DnssdServiceChangedCallback callback) {
            mDnssdServiceChangedCallback = callback;
        };
       
        // Constructor needs to be internal as this is an unsealed ref base class
        DnssdServiceWatcher(const char* serviceType, DnssdServiceChangedCallback callback = nullptr);

    private:
        void OnServiceAdded(Windows::Devices::Enumeration::DeviceWatcher^ sender, Windows::Devices::Enumeration::DeviceInformation^ args);
        void OnServiceRemoved(Windows::Devices::Enumeration::DeviceWatcher^ sender, Windows::Devices::Enumeration::DeviceInformationUpdate^ args);
        void OnServiceUpdated(Windows::Devices::Enumeration::DeviceWatcher^ sender, Windows::Devices::Enumeration::DeviceInformationUpdate^ args);
        void OnServiceEnumerationCompleted(Windows::Devices::Enumeration::DeviceWatcher^ sender, Platform::Object^ args);
        void OnServiceEnumerationStopped(Windows::Devices::Enumeration::DeviceWatcher^ sender, Platform::Object^ args);
        void UpdateDnssdService(DnssdServiceUpdateType type, Windows::Foundation::Collections::IMapView<Platform::String^, Platform::Object^>^ props, Platform::String^ serviceId);
        void OnDnssdServiceUpdated(DnssdServiceInstance^ info);

        Windows::Devices::Enumeration::DeviceWatcher^ mServiceWatcher;

        DnssdServiceChangedCallback mDnssdServiceChangedCallback;

        std::map<Platform::String^, DnssdServiceInstance^> mServices;
        Platform::String^ mServiceName;
        bool mRunning;
    };


    class DnssdServiceWatcherWrapper
    {
    public:
        DnssdServiceWatcherWrapper(DnssdServiceWatcher ^ watcher)
            : mWatcher(watcher)
        {
        }

        DnssdServiceWatcher^ GetWatcher() {
            return mWatcher;
        }

    private:
        DnssdServiceWatcher^ mWatcher;
    };
};




