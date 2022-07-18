#include<bits/stdc++.h>
#include "utility.h"
using namespace std;

struct FileInfo
{
    string name;
    string filePath;
    string hash;
};

map<string, set<string>> tagToFileHashes;
map<string, FileInfo> hashToFileInfo;

string getFileName(string filePath){
    int beginIdx = filePath.size();
    for (int i = beginIdx - 1; i >= 0; i--)
    {
        if (filePath[i] == '\\' || filePath[i] == '/')
        {
            beginIdx = i + 1;
            break;
        }
    }
    return filePath.substr(beginIdx);
}

void addTags(string filePath, vector<string> &tags)
{
    if (!checkFileExits(filePath))
    {
        cout << "! File does not exist. Please check the file path\n";
        return;
    }
    string fileName = getFileName(filePath);

    for (string &tag : tags)
        insertInTrie(tag);

    string fileHash = getHash(filePath);
    if (fileHash == "")
        return;

    ifstream fin1;
    fin1.open("db.csv", ios::in);

    string line, word, hash;
    vector<string> row;
    bool flag = 0;

    // Check if file already exists
    while (!fin1.eof())
    {
        row.clear();
        getline(fin1, line);
        if (line == "")
            break;
        stringstream s(line);

        getline(s, hash, ',');
        if (hash == fileHash)
        {
            flag = 1;
            row.clear();
            word = "";
            line = "";
            hash = "";
            break;
        }
    }
    fin1.close();

    if (!flag)
    {
        FileInfo file1;
        file1.name = fileName;
        file1.filePath = filePath;
        file1.hash = fileHash;

        hashToFileInfo[fileHash] = file1;

        ofstream fout;
        fout.open("db.csv", ios::out | ios::app);
        row = {fileHash, fileName, filePath};
        for (string &tag : tags)
        {
            tagToFileHashes[tag].insert(fileHash);
            row.push_back(tag);
        }

        string res;
        for (int i = 0; i < row.size(); i++)
        {
            res += row[i] + ",";
        }
        res += "\n";
        fout << res;

        fout.close();
    }
    else
    {
        ifstream fin;
        ofstream fout;
        fin.open("db.csv", ios::in);
        fout.open("dbnew.csv", ios::out);

        while (!fin.eof())
        {
            row.clear();
            getline(fin, line, '\n');
            stringstream s(line);

            while (getline(s, word, ','))
            {
                row.push_back(word);
            }

            hash = row[0];

            if (hash == fileHash)
            {
                row[1] = fileName;
                row[2] = filePath;

                for (string tag : tags)
                {
                    if (tagToFileHashes[tag].count(fileHash) == 0)
                    {
                        row.push_back(tag);
                        tagToFileHashes[tag].insert(fileHash);
                    }
                }
            }

            int row_size = row.size();
            string res;
            for (int i = 0; i < row_size; i++)
            {
                if (row[i] == "")
                    continue;
                res += row[i] + ",";
            }
            res += "\n";
            if (res == "\n")
                continue;
            fout << res;

            if (fin.eof())
                break;
        }

        fin.close();
        fout.close();

        remove("db.csv");
        rename("dbnew.csv", "db.csv");
    }
}



vector<string> readTags(string filePath)
{
    string fileHash = getHash(filePath);
    fstream fin;
    fin.open("db.csv", ios::in);

    bool flag = 0;
    vector<string> row;
    string line, word, hash;

    while (!fin.eof())
    {
        row.clear();
        getline(fin, line);
        stringstream s(line);

        while (getline(s, word, ','))
        {
            row.push_back(word);
        }

        hash = row[0];
        if (hash == fileHash)
        {
            flag = 1;
            return row;
        }
    }

    if (flag == 0)
        cout << "! File not found\n";

    return {};
}


void deleteTag(string filePath, vector<string> tags)
{
    string fileHash = getHash(filePath);
    fstream fin, fout;

    fin.open("db.csv", ios::in);
    fout.open("dbnew.csv", ios::out);

    string line, word, hash;
    vector<string> row;
    bool flag = 0;
    sort(tags.begin(), tags.end());

    while (!fin.eof())
    {
        row.clear();
        getline(fin, line);
        stringstream s(line);

        while (getline(s, word, ','))
        {
            row.push_back(word);
        }

        hash = row[0];
        int row_size = row.size();

        if (hash == fileHash)
        {
            flag = 1;

            if (!fin.eof())
            {
                for (int i = 0; i < row_size; i++)
                {
                    if (i <= 2)
                    {
                        fout << row[i] << ",";
                        continue;
                    }
                    auto it = find(tags.begin(), tags.end(), row[i]);
                    if (it == tags.end())
                        fout << row[i] << ',';
                }
                fout << "\n";
            }
        }
        else
        {
            if (!fin.eof())
            {
                for (int i = 0; i < row_size; i++)
                {
                    fout << row[i] << ',';
                }
                // fout << row[row_size - 1] << "\n";
                fout << "\n";
            }
        }
        if (fin.eof())
            break;
    }
    if (flag == 0)
        cout << "File not added to application\n";

    fin.close();
    fout.close();

    remove("db.csv");
    rename("dbnew.csv", "db.csv");
}

vector<FileInfo> findFiles(vector<string> tags)
{
    unordered_map<string, int> m;
    int len = tags.size();

    vector<string> moreTags;

    for (int i = 0; i < len; i++)
    {
        TrieNode *cur = root;
        for (int j = 0; j < tags[i].length() && cur; j++)
        {
            cur = (cur->child)[tags[i][j]];
        }

        getMoreTags(tags[i], moreTags, cur);
    }

    for (auto i : moreTags)
    {
        tags.push_back(i);
    }

    for (string tag : tags)
    {
        for (string fileHash : tagToFileHashes[tag])
        {
            m[fileHash]++;
        }
    }
    vector<string> fileHashes;
    for (auto it : m)
    {
        if (it.second >= len)
        {
            fileHashes.push_back(it.first);
        }
    }

    vector<FileInfo> result;

    for (string fileHash : fileHashes)
    {
        result.push_back(hashToFileInfo[fileHash]);
    }
    return result;
}

void removeFiles(vector<string> paths)
{
    set<string> fileHashes;
    for (string path : paths)
        fileHashes.insert(getHash(path));

    fstream fin, fout;

    fin.open("db.csv", ios::in);
    fout.open("dbnew.csv", ios::out);

    string line, word, hash;
    vector<string> row;
    bool flag = 0;

    while (!fin.eof())
    {
        row.clear();
        getline(fin, line);
        stringstream s(line);

        while (getline(s, word, ','))
        {
            row.push_back(word);
        }

        hash = row[0];
        int row_size = row.size();

        auto it = fileHashes.find(hash);
        if (it == fileHashes.end())
        {
            if (!fin.eof())
            {
                for (int i = 0; i < row_size - 1; i++)
                {
                    fout << row[i] << ',';
                }

                fout << row[row_size - 1] << "\n";
            }
        }
        if (fin.eof())
            break;
    }

    fin.close();
    fout.close();

    remove("db.csv");
    rename("dbnew.csv", "db.csv");
}

void move(string path1, string path2)
{
    wstring fileNameCon1 = wstring(path1.begin(), path1.end());
    wstring fileNameCon2 = wstring(path2.begin(), path2.end());
    LPCWSTR file1 = fileNameCon1.c_str();
    LPCWSTR file2 = fileNameCon2.c_str();

    BOOL bFile;
    bFile = MoveFile(file1, file2);

    if (bFile == FALSE)
        cout << "! File move failed\n";
    else
    {
        vector<string> tags;
        addTags(path2, tags);

        string fileName = getFileName(path2);
        string fileHash = getHash(path2);

        FileInfo file1;
        file1.name = fileName;
        file1.filePath = path2;
        file1.hash = fileHash;

        hashToFileInfo[fileHash] = file1;
    }
}

void copy(string path1, string path2)
{
    wstring fileNameCon1 = wstring(path1.begin(), path1.end());
    wstring fileNameCon2 = wstring(path2.begin(), path2.end());
    LPCWSTR file1 = fileNameCon1.c_str();
    LPCWSTR file2 = fileNameCon2.c_str();

    BOOL bFile;
    bFile = CopyFile(file1, file2, TRUE);

    if (bFile == FALSE)
    {
        cout << "! File copy failed\n";
    }
    else
    {
        SYSTEMTIME currentTime;
        GetSystemTime(&currentTime);

        FILETIME fileTime;
        SystemTimeToFileTime(&currentTime, &fileTime);

        HANDLE filename = CreateFile(file2,
                                     FILE_WRITE_ATTRIBUTES, FILE_SHARE_READ | FILE_SHARE_WRITE,
                                     NULL, OPEN_EXISTING,
                                     FILE_ATTRIBUTE_NORMAL, NULL);

        SetFileTime(filename, &fileTime, &fileTime, &fileTime);
        CloseHandle(filename);

        vector<string> row = readTags(path1), tags;
        for (int i = 3; i < row.size(); i++)
            tags.push_back(row[i]);
        addTags(path2, tags);

        cout << "^ CopyFile success\n";
    }
}

vector<string> getFileMetadataTags(const string &filePath)
{
    return {};
}

void trackFolder(const string folderPath, vector<string> tags = {})
{
    WIN32_FIND_DATAA findData;
    HANDLE hFind = INVALID_HANDLE_VALUE;
    string full_path = folderPath + "\\*";
    vector<string> dir_list;

    hFind = FindFirstFileA(full_path.c_str(), &findData);

    if (hFind == INVALID_HANDLE_VALUE)
    {
        FindClose(hFind);
        cout << "! Directory not found (" << full_path << ")\n";
        return;
    }

    while (FindNextFileA(hFind, &findData) != 0)
    {
        string filePath = folderPath + "\\" + findData.cFileName;
        auto attributes = GetFileAttributesA(filePath.c_str());
        if (attributes & FILE_ATTRIBUTE_DIRECTORY)
            continue;
        addTags(filePath, tags);
    }

    FindClose(hFind);
}


void createFile(string filePath, vector<string> tags = {})
{
    wstring fileName = wstring(filePath.begin(), filePath.end());

    HANDLE fileHandle = CreateFileW(fileName.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (fileHandle == INVALID_HANDLE_VALUE)
    {
        CloseHandle(fileHandle);
        cout << "! There was an error creating the file\n";
        return;
    }
    CloseHandle(fileHandle);

    addTags(filePath, tags);
}