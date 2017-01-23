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
#include "DnssdUtils.h"
#include <algorithm>
#include <vector>
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

    DnssdServiceWatcher::DnssdServiceWatcher(const char* serviceName, DnssdServiceChangedCallback callback)
        : mDnssdServiceChangedCallback(callback)
        , mRunning(false)
    {
        mServiceName = StringToPlatformString(serviceName);
    }

    DnssdServiceWatcher::~DnssdServiceWatcher()
    {
        if (mServiceWatcher)
        {
            mRunning = false;
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
                "System.Devices.Dnssd.Domain:=\"local\" AND System.Devices.Dnssd.ServiceName:=\"" + mServiceName + "\"";

            mServiceWatcher = DeviceInformation::CreateWatcher(aqsQueryString, propertyKeys, DeviceInformationKind::AssociationEndpointService);

            // wire up event handlers
            mServiceWatcher->Added += ref new TypedEventHandler<DeviceWatcher ^, DeviceInformation ^>(this, &DnssdServiceWatcher::OnServiceAdded);
            mServiceWatcher->Removed += ref new TypedEventHandler<DeviceWatcher ^, DeviceInformationUpdate ^>(this, &DnssdServiceWatcher::OnServiceRemoved);
            mServiceWatcher->Updated += ref new TypedEventHandler<DeviceWatcher ^, DeviceInformationUpdate ^>(this, &DnssdServiceWatcher::OnServiceUpdated);
            mServiceWatcher->EnumerationCompleted += ref new Windows::Foundation::TypedEventHandler<DeviceWatcher ^, Platform::Object ^>(this, &DnssdServiceWatcher::OnServiceEnumerationCompleted);
            mServiceWatcher->Stopped += ref new Windows::Foundation::TypedEventHandler<DeviceWatcher ^, Platform::Object ^>(this, &DnssdServiceWatcher::OnServiceEnumerationStopped);

            // start watching for dnssd services
            mServiceWatcher->Start();
            mRunning = true;
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
            return DNSSD_SERVICEWATCHER_INITIALIZATION_ERROR;
        }
    }

    void DnssdServiceWatcher::UpdateDnssdService(DnssdServiceUpdateType type, Windows::Foundation::Collections::IMapView<Platform::String^, Platform::Object^>^ props, Platform::String^ serviceId)
    {
        auto box = safe_cast<Platform::IBoxArray<Platform::String^>^>(props->Lookup("System.Devices.IpAddress"));
        Platform::String^ host = box->Value->get(0);
        Platform::String^ port = props->Lookup("System.Devices.Dnssd.PortNumber")->ToString();
        Platform::String^ name = props->Lookup("System.Devices.Dnssd.InstanceName")->ToString();

        auto it = mServices.find(serviceId);
        if (it != mServices.end()) // service was previously found. Update the info and report change if necessary
        {
            auto info = it->second;
            if (info->mHost != host)
            {
                info->mHost = host;
                info->mChanged = true;
            }
            if (info->mPort != port)
            {
                info->mPort = port;
                info->mChanged = true;
            }
            if (info->mInstanceName != name)
            {
                info->mInstanceName = name;
                info->mChanged = true;
            }
            info->mType = DnssdServiceUpdateType::ServiceUpdated;

            if (info->mChanged)
            {
                // report the updated service
                OnDnssdServiceUpdated(info);
            }
        }
        else // add it to the service map
        {
            DnssdServiceInstance^ info = ref new DnssdServiceInstance;
            info->mId = serviceId;
            info->mHost = host;
            info->mPort = port;
            info->mInstanceName = name;
            info->mType = DnssdServiceUpdateType::ServiceAdded;
            mServices[serviceId] = info;

            // report the new service
            OnDnssdServiceUpdated(info);
        }
    }

    void DnssdServiceWatcher::OnDnssdServiceUpdated(DnssdServiceInstance^ info)
    {
        DnssdServiceWatcherWrapper wrapper(this);
        DnssdServiceInfo serviceInfo;

        // convert Platform::Strings to std::strings 
        std::string host = PlatformStringToString(info->mHost);
        std::string  port = PlatformStringToString(info->mPort);
        std::string instanceName = PlatformStringToString(info->mInstanceName);
        std::string id = PlatformStringToString(info->mId);

        serviceInfo.host = host.c_str();
        serviceInfo.port = port.c_str();
        serviceInfo.id = id.c_str();
        //serviceInfo.instanceName = instanceName.c_str();
        serviceInfo.instanceName = instanceName.c_str();

        auto foo = info->mId->Data();

        if (mDnssdServiceChangedCallback != nullptr)
        {
            mDnssdServiceChangedCallback(&wrapper, info->mType, &serviceInfo);
        }
    }

    void DnssdServiceWatcher::OnServiceAdded(DeviceWatcher^ sender, DeviceInformation^ args)
    {
        UpdateDnssdService(DnssdServiceUpdateType::ServiceAdded, args->Properties, args->Id);
    }

    void DnssdServiceWatcher::OnServiceUpdated(DeviceWatcher^ sender, DeviceInformationUpdate^ args)
    {
        UpdateDnssdService(DnssdServiceUpdateType::ServiceUpdated, args->Properties, args->Id);
    }

    void DnssdServiceWatcher::OnServiceRemoved(DeviceWatcher^ sender, DeviceInformationUpdate^ args)
    {
        UpdateDnssdService(DnssdServiceUpdateType::ServiceUpdated, args->Properties, args->Id);
    }

    void DnssdServiceWatcher::OnServiceEnumerationCompleted(DeviceWatcher^ sender, Platform::Object^ args)
    {
        // stop the service scanning. Service scanning will be restarted when OnServiceEnumerationStopped event is received
        mServiceWatcher->Stop();
    }

    void DnssdServiceWatcher::OnServiceEnumerationStopped(Windows::Devices::Enumeration::DeviceWatcher^ sender, Platform::Object^ args)
    {
        // check if we are shutting down
        if (!mRunning)
        {
            return;
        }

        std::vector<Platform::String^> removedServices;

        // iterate through the services list and remove any service that is marked for removal
        for (auto it = mServices.begin(); it != mServices.end(); ++it)
        {
            auto service = it->second;
            if (service->mType == DnssdServiceUpdateType::ServiceRemoved)
            {
                // report to the client the removed service
                OnDnssdServiceUpdated(service);
                removedServices.push_back(it->first);
            }
            else // prepare the service for the next search
            {
                // for each scan we mark each service as removed. 
                // If the scan finds the service again we will update its state accordingly
                service->mType = DnssdServiceUpdateType::ServiceRemoved;
                service->mChanged = false;
            }
        }

        // remove stale services from the services map
        std::for_each(begin(removedServices), end(removedServices), [&](Platform::String^ id)
        {
            auto service = mServices.find(id);
            if (service != mServices.end())
            {
                mServices.erase(service);
            }
        });

        // restart the service scan
        mServiceWatcher->Start();
    }
}


