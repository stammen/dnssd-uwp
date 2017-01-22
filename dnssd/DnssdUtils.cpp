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
#include <cvt/wstring>
#include <codecvt>



namespace dnssd_uwp
{
    Platform::String^ StringToPlatformString(const std::string& s)
    {
        std::wstring w(s.begin(), s.end());
        Platform::String^ p = ref new Platform::String(w.c_str());
        return p;
    }

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
}


