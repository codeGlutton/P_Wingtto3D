#pragma once

constexpr unsigned char UTF8_BOM[] = { 0xEF, 0xBB, 0xBF };

inline bool HasUtf8Bom(const std::string& str)
{
    return str.size() >= 3 &&
        (unsigned char)str[0] == UTF8_BOM[0] &&
        (unsigned char)str[1] == UTF8_BOM[1] &&
        (unsigned char)str[2] == UTF8_BOM[2];
}

inline std::wstring ConvertUtf8ToWString(const std::string& utf8Str)
{
    const char* data = utf8Str.data();
    int32 ut8StrSize = static_cast<int32>(utf8Str.size());

    // BOM 제거
    if (HasUtf8Bom(utf8Str) == true)
    {
        data += 3;
        ut8StrSize -= 3;
    }
    if (ut8StrSize <= 0)
    {
        return std::wstring();
    }

    int32 wstrSize = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, data, ut8StrSize, nullptr, 0);
    if (wstrSize <= 0)
    {
        return std::wstring();
    }
    std::wstring wstr(wstrSize, L'\0');
    MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, data, ut8StrSize, wstr.data(), wstrSize);

    return wstr;
}

inline std::string ConvertWStringToUtf8(const std::wstring& wstr, bool withBom = false)
{
    std::string utf8Str;

    if (wstr.empty() == true)
    {
        return utf8Str;
    }

    int utf8Size = WideCharToMultiByte(CP_UTF8, 0, wstr.data(), static_cast<int32>(wstr.size()), nullptr, 0, nullptr, nullptr);
    if (utf8Size <= 0)
    {
        return utf8Str;
    }

    // BOM 삽입
    if (withBom == true)
    {
        utf8Str.append(reinterpret_cast<const char*>(UTF8_BOM), 3);
    }
    size_t offset = utf8Str.size();
    utf8Str.resize(offset + utf8Size);

    WideCharToMultiByte(CP_UTF8, 0, wstr.data(), static_cast<int32>(wstr.size()), utf8Str.data() + offset, utf8Size, nullptr, nullptr);

    return utf8Str;
}

