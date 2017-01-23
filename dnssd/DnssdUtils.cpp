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
#include <memory>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

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
        int bufferSize = WideCharToMultiByte(CP_UTF8, 0, s->Data(), -1, nullptr, 0, NULL, NULL);
        auto utf8 = std::make_unique<char[]>(bufferSize);
        if (0 == WideCharToMultiByte(CP_UTF8, 0, s->Data(), -1, utf8.get(), bufferSize, NULL, NULL))
            throw std::exception("Can't convert string to UTF8");

        return std::string(utf8.get());
    }

    std::string PlatformStringToString2(Platform::String^ s)
    {
        stdext::cvt::wstring_convert<std::codecvt_utf8<wchar_t>> convert;
        std::string stringUtf8 = convert.to_bytes(s->Data());
        return stringUtf8;
    }
}


