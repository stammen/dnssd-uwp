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

#include "DnssdServiceWatcher.h"
#include <algorithm>
#include <collection.h>
#include <cvt/wstring>
#include <codecvt>
#include <ppltasks.h>

using namespace Platform::Collections;
using namespace Windows::Networking::ServiceDiscovery::Dnssd;
using namespace Windows::Networking;
using namespace Windows::Networking::Sockets;
using namespace Windows::Networking::Connectivity;
using namespace Windows::Devices::Enumeration;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Platform;
using namespace concurrency;

namespace dnssd_uwp
{
    std::string PlatformStringToString(Platform::String^ s)
    {
        std::wstring w(s->Data());
        std::string result(w.begin(), w.end());
        return result;
    }

    std::string PlatformStringToString2(Platform::String^ s)
    {
        stdext::cvt::wstring_convert<std::codecvt_utf8<wchar_t>> convert;
        std::string stringUtf8 = convert.to_bytes(s->Data());
        return stringUtf8;
    }

    DnssdServiceWatcher::DnssdServiceWatcher(DnssdServiceChangedCallback callback)
        : mDnssdServiceChangedCallback(callback)
    {
 
    }

    DnssdServiceWatcher::~DnssdServiceWatcher()
    {
        if (mServiceWatcher)
        {
            mServiceWatcher->Stop();
            mServiceWatcher = nullptr;
        }
    }

    DnssdErrorType DnssdServiceWatcher::Initialize()
    {
        auto task = create_task(create_async([this]
        {
            /// <summary>
            /// All of the properties that will be returned when a DNS-SD instance has been found. 
            /// </summary>

            Vector<Platform::String^>^ propertyKeys = ref new Vector<Platform::String^>();
            propertyKeys->Append(L"System.Devices.Dnssd.HostName");
            propertyKeys->Append(L"System.Devices.Dnssd.ServiceName");
            propertyKeys->Append(L"System.Devices.Dnssd.InstanceName");
            propertyKeys->Append(L"System.Devices.IpAddress");
            propertyKeys->Append(L"System.Devices.Dnssd.PortNumber");

            Platform::String^ aqsQueryString;
            aqsQueryString = L"System.Devices.AepService.ProtocolId:={4526e8c1-8aac-4153-9b16-55e86ada0e54} AND " +
                "System.Devices.Dnssd.Domain:=\"local\" AND System.Devices.Dnssd.ServiceName:=\"_daap._tcp\"";

            mServiceWatcher = DeviceInformation::CreateWatcher(aqsQueryString, propertyKeys, DeviceInformationKind::AssociationEndpointService);

            // wire up event handlers
            mServiceWatcher->Added += ref new TypedEventHandler<DeviceWatcher ^, DeviceInformation ^>(this, &DnssdServiceWatcher::OnServiceAdded);
            mServiceWatcher->Removed += ref new TypedEventHandler<DeviceWatcher ^, DeviceInformationUpdate ^>(this, &DnssdServiceWatcher::OnServiceRemoved);
            mServiceWatcher->Updated += ref new TypedEventHandler<DeviceWatcher ^, DeviceInformationUpdate ^>(this, &DnssdServiceWatcher::OnServiceUpdated);
            mServiceWatcher->EnumerationCompleted += ref new Windows::Foundation::TypedEventHandler<DeviceWatcher ^, Platform::Object ^>(this, &DnssdServiceWatcher::OnServiceEnumerationCompleted);
            mServiceWatcher->Stopped += ref new Windows::Foundation::TypedEventHandler<DeviceWatcher ^, Platform::Object ^>(this, &DnssdServiceWatcher::OnServiceEnumerationStopped);

            // start watching for dnssd services
            mServiceWatcher->Start();
            auto status = mServiceWatcher->Status;
        }));

        try
        {
            // wait for port enumeration to complete
            task.get(); // will throw any exceptions from above task
            return DNSSD_NO_ERROR;
        }
        catch (Platform::Exception^ ex)
        {
            return DNSSD_PORTWATCHER_INITIALIZATION_ERROR;
        }
    }


    void DnssdServiceWatcher::SendDnssdServiceUpdate(DnssdServiceUpdateType type, Windows::Foundation::Collections::IMapView<Platform::String^, Platform::Object^>^ props, Platform::String^ serviceId)
    {
        DnssdServiceInfo info;

        auto box = safe_cast<Platform::IBoxArray<Platform::String^>^>(props->Lookup("System.Devices.IpAddress"));
        std::string host = PlatformStringToString(box->Value->get(0));
        std::string  port = PlatformStringToString(props->Lookup("System.Devices.Dnssd.PortNumber")->ToString());
        std::string instanceName = PlatformStringToString(props->Lookup("System.Devices.Dnssd.InstanceName")->ToString());
        std::string id = PlatformStringToString(serviceId);

        info.id = id.c_str();
        info.host = host.c_str();
        info.port = port.c_str();
        info.instanceName = instanceName.c_str();

        OnDnssdServiceUpdated(type, &info);
    }

    void DnssdServiceWatcher::OnServiceAdded(DeviceWatcher^ sender, DeviceInformation^ args)
    {
        SendDnssdServiceUpdate(DnssdServiceUpdateType::ServiceAdded, args->Properties, args->Id);
    }

    void DnssdServiceWatcher::OnServiceUpdated(DeviceWatcher^ sender, DeviceInformationUpdate^ args)
    {
        SendDnssdServiceUpdate(DnssdServiceUpdateType::ServiceUpdated, args->Properties, args->Id);
    }

    void DnssdServiceWatcher::OnServiceRemoved(DeviceWatcher^ sender, DeviceInformationUpdate^ args)
    {
        SendDnssdServiceUpdate(DnssdServiceUpdateType::ServiceUpdated, args->Properties, args->Id);
    }

    void DnssdServiceWatcher::OnServiceEnumerationCompleted(DeviceWatcher^ sender, Platform::Object^ args)
    {
        mServiceWatcher->Stop();
    }

    void DnssdServiceWatcher::OnServiceEnumerationStopped(Windows::Devices::Enumeration::DeviceWatcher^ sender, Platform::Object^ args)
    {
        mServiceWatcher->Start();
    }


    void DnssdServiceWatcher::OnDnssdServiceUpdated(DnssdServiceUpdateType update, DnssdServiceInfoPtr info)
    {
        DnssdServiceWatcherWrapper wrapper(this);

        if (mDnssdServiceChangedCallback != nullptr)
        {
            mDnssdServiceChangedCallback(&wrapper, update, info);
        }

        mPortUpdateEventHander(this, update, info);
    }
}


