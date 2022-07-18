#include<bits/stdc++.h>
#include<Windows.h>
using namespace std;

string getHash(string filePath)
{
    wstring fileName = wstring(filePath.begin(), filePath.end());

    HANDLE fileHandle = CreateFile(fileName.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);
    FILETIME creation_time;
    bool file_found = GetFileTime(fileHandle, &creation_time, NULL, NULL);
    CloseHandle(fileHandle);

    if (!file_found)
        return "";

    size_t h1 = hash<DWORD>{}(creation_time.dwLowDateTime);
    size_t h2 = hash<DWORD>{}(creation_time.dwHighDateTime);

    size_t final_hash = (h1 ^ (h2 << 1));
    string res = "";
    for (int i = 0; i < 8; ++i)
    {
        int temp = (final_hash & 15);
        char ch;
        if (temp < 10)
            ch = '0' + temp;
        else
            ch = 'a' + temp - 10;
        res += ch;
        final_hash >>= 4;
    }

    return res;
}

bool checkFileExits(string filePath)
{
    wstring newFilePath = wstring(filePath.begin(), filePath.end());
    HANDLE fileHandle = CreateFile(newFilePath.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);
    bool file_found = GetFileTime(fileHandle, NULL, NULL, NULL);
    CloseHandle(fileHandle);

    return file_found;
}
