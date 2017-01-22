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

namespace dnssd_uwp
{
    ref class DnssdService sealed
    {
    public:
        virtual ~DnssdService();

    internal:
        DnssdService(const std::string& name, const std::string& port);
        DnssdErrorType Start();
        void Stop();

    private:
        Platform::String^ mServiceName;
        Platform::String^ mPort;
    };

    class DnssdServiceWrapper
    {
    public:
        DnssdServiceWrapper(DnssdService ^ service)
            : mService(service)
        {
        }

        DnssdService^ GetService() {
            return mService;
        }

    private:
        DnssdService^ mService;
    };
};

