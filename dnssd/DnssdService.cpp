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

using namespace dnssd_uwp;
using namespace concurrency;
using namespace Microsoft::WRL;

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

    return result;
}

void DnssdService::Stop()
{

}

