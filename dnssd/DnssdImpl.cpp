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
#include <ppltasks.h>
#include <robuffer.h> 

using namespace dnssd_uwp;
using namespace Windows::Storage::Streams;
using namespace std::placeholders;
using namespace concurrency;
using namespace Microsoft::WRL;

DnssdService::DnssdService()
    : mErrorMessage("")
    , mError(DNSSD_NO_ERROR)
{

}

DnssdService::~DnssdService()
{

}

void DnssdService::SetError(DnssdErrorType error, const std::string& message)
{
    mError = error;
    mErrorMessage = message;
}

const std::string& DnssdService::GetErrorMessage()
{
    return mErrorMessage;
}

DnssdErrorType DnssdService::GetError()
{
    return mError;
}

