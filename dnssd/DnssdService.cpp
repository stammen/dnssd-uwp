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
#include "DnssdService.h"
#include "Dnssdutils.h"
#include <ppltasks.h>
#include <stdlib.h>

using namespace dnssd_uwp;
using namespace concurrency;
using namespace Windows::Foundation;
using namespace Windows::Networking;
using namespace Windows::Networking::Connectivity;
using namespace Windows::Networking::Sockets;
using namespace Windows::Networking::ServiceDiscovery::Dnssd;

DnssdService::DnssdService(const std::string& name, const std::string& port)
{
    mServiceName = StringToPlatformString(name);
    mPort = StringToPlatformString(port);
}

DnssdService::~DnssdService()
{
    DnssdService::Stop();
}

DnssdErrorType DnssdService::Start()
{
    DnssdErrorType result = DNSSD_NO_ERROR;

    if (mService != nullptr)
    {
        return DNSSD_SERVICE_ALREADY_EXISTS_ERROR;
    }

    auto hostNames = NetworkInformation::GetHostNames();
    HostName^ hostName = nullptr;

    // find first HostName of Type == HostNameType.DomainName && RawName contains "local"
    for (unsigned int i = 0; i < hostNames->Size; ++i)
    {
        HostName^ n = hostNames->GetAt(i);
        if (n->Type == HostNameType::DomainName)
        {
            std::wstring temp(n->RawName->Data());
            auto found = temp.find(L"local");
            if (found != std::string::npos)
            {
                hostName = n;
                break;
            }
        }
    }

    if (hostName == nullptr)
    {
        return DNSSD_LOCAL_HOSTNAME_NOT_FOUND_ERROR;
    }

    auto task = create_task(create_async([this, hostName]
    {
        mSocket = ref new StreamSocketListener();
        mSocketToken = mSocket->ConnectionReceived += ref new TypedEventHandler<StreamSocketListener^, StreamSocketListenerConnectionReceivedEventArgs ^>(this, &DnssdService::OnConnect);
        create_task(mSocket->BindServiceNameAsync(mPort)).get();
        unsigned short port = static_cast<unsigned short>(_wtoi(mSocket->Information->LocalPort->Data()));
        mService = ref new DnssdServiceInstance(L"dnssd." + mServiceName + L".local", hostName, port);
        return create_task(mService->RegisterStreamSocketListenerAsync(mSocket));
    }));

    try
    {
        // wait for dnssd service to start
        DnssdRegistrationResult^ reg = task.get(); // will also rethrow any exceptions from above task
        auto ip = reg->IPAddress; // this always seems to be NULL
        auto status = reg->Status;
        bool hasInstanceChanged = reg->HasInstanceNameChanged;

        if (status != DnssdRegistrationStatus::Success)
        {
            switch (status)
            {
                case DnssdRegistrationStatus::InvalidServiceName:
                    result = DNSSD_INVALID_SERVICE_NAME_ERROR;
                    break;
                case DnssdRegistrationStatus::SecurityError:
                    result = DNSSD_SERVICE_SECURITY_ERROR;
                    break;
                case DnssdRegistrationStatus::ServerError:
                    result = DNSSD_SERVICE_SERVER_ERROR;
                    break;
                default:
                    result = DNSSD_SERVICE_INITIALIZATION_ERROR;
                    break;
            }

        }
        return result;
    }
    catch (Platform::Exception^ ex)
    {
        result =  DNSSD_SERVICE_INITIALIZATION_ERROR;
    }

    return result;
}

void DnssdService::Stop()
{
    if (mSocket != nullptr)
    {
        mSocket->ConnectionReceived -= mSocketToken;
        delete mSocket;
        mSocket = nullptr;
    }

    mService = nullptr;
}

void DnssdService::OnConnect(StreamSocketListener^ sender, StreamSocketListenerConnectionReceivedEventArgs ^ args)
{


}


