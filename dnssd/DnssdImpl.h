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
#include "DnssdServiceWatcher.h"
#include <memory>
#include <string>
#include <Windows.h>

namespace dnssd_uwp
{
    ref class DnssdService abstract
    {
    public:
        virtual ~DnssdService();
        virtual void ClosePort(void) = 0;

    internal:
        DnssdService();
        virtual DnssdErrorType OpenPort(Platform::String^ id) = 0;

        void SetError(DnssdErrorType error, const std::string& message);
        const std::string& GetErrorMessage();
        DnssdErrorType GetError();
 
    private:
        std::string mErrorMessage;
        DnssdErrorType mError;
    };



    class Dnssd
    {
    public:
        Dnssd(DnssdServiceChangedCallback callback);
        DnssdErrorType Initialize();

        DnssdServiceWatcher^ GetServiceWatcher();
        DnssdServiceWatcherPtr GetServiceWatcherWrapper();

    private:

        DnssdServiceWatcher^ mServiceWatcher;
        std::shared_ptr<DnssdServiceWatcherWrapper> mServiceWatcherWrapper;
    };
};

