#define UNICODE
#include <bits/stdc++.h>
#include <Windows.h>

using namespace std;

struct FileInfo
{
    string name;
    string filePath;
    string hash;
};

struct TrieNode
{
    char ele;
    bool isEndofWord;
    TrieNode *child[26];
};

TrieNode *getNewNode(char ch)
{
    TrieNode *node = new TrieNode;
    node->ele = ch;
    node->isEndofWord = false;
    for (int i = 0; i < 26; ++i)
        (node->child)[i] = nullptr;

    return node;
}

TrieNode *root = getNewNode('/');
map<string, set<string>> tagToFileHashes;
map<string, FileInfo> hashToFileInfo;

void insertInTrie(string &tag)
{
    int len = tag.length();

    TrieNode *current = root;

    for (int i = 0; i < len; i++)
    {

        int val = tag[i] - 'a';
        if ((current->child)[val] != nullptr)
            current = (current->child)[val];
        else
        {
            TrieNode *new_node = getNewNode(tag[i]);
            (current->child)[val] = new_node;
            current = new_node;
        }

        if (i == len - 1)
            current->isEndofWord = true;
    }
}

char *create_token(const char *input, int begin, int end)
{
    if (begin > end)
        return NULL;

    size_t length = end - begin + 1;
    char *res = (char *)malloc(sizeof(char) * (length + 1));
    if (res == NULL)
        return NULL;

    memcpy(res, input + begin, sizeof(char) * length);
    res[length] = 0;

    return res;
}

int tokenize_input(const char input[], const size_t length, char **tokenized_input, size_t &num_tokens)
{
    bool in_string = false;
    int start = 0;

    char *result_buffer[128];
    memset(result_buffer, 0, sizeof(result_buffer));
    // num_tokens = 0;

    for (int index = 0; index < length + 1; ++index)
    {
        if (!in_string && (input[index] == ' ' || input[index] == '\0'))
        {
            char *new_token = create_token(input, start, index - 1);
            if (new_token == NULL)
            {
                start = index + 1;
                continue;
            }

            result_buffer[num_tokens] = new_token;
            start = index + 1;
            ++num_tokens;
        }
        else if (input[index] == '"')
        {
            if (in_string)
            {
                char *new_token = create_token(input, start, index - 1);
                if (new_token == NULL)
                {
                    start = index + 1;
                    continue;
                }

                result_buffer[num_tokens] = new_token;
                ++num_tokens;
            }

            start = index + 1;
            in_string = !in_string;
        }
    }

    for (int index = 0; index < num_tokens; ++index)
    {
        *(tokenized_input + index) = result_buffer[index];
    }

    return 0;
}

string getHash(string fileName)
{
    HANDLE fileHandle = CreateFileA(fileName.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);
    LPFILETIME creation_time;
    bool file_found = GetFileTime(fileHandle, creation_time, NULL, NULL);
    CloseHandle(fileHandle);

    if (!file_found)
    {
        cout << "! File not found\n";
        return "";
    }

    size_t h1 = hash<DWORD>{}(creation_time->dwLowDateTime);
    size_t h2 = hash<DWORD>{}(creation_time->dwHighDateTime);

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

    return fileName;
}

bool checkFileExits(string filePath)
{
    LPWIN32_FIND_DATAA FindFileData;
    HANDLE hFindFile = INVALID_HANDLE_VALUE;
    wstring fileNameCon = wstring(filePath.begin(), filePath.end());
    LPCWSTR file = fileNameCon.c_str();

    hFindFile = FindFirstFileA(filePath.c_str(), FindFileData);

    if (INVALID_HANDLE_VALUE == hFindFile)
        return false;
    else
        return true;
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
        cout << "Record not found\n";

    return {};
}

void addTags(string filePath, vector<string> &tags)
{
    int beginIdx = filePath.size();
    for (int i = beginIdx - 1; i >= 0; i--)
    {
        if (filePath[i] == '\\' || filePath[i] == '/')
        {
            beginIdx = i + 1;
            break;
        }
    }
    string fileName = filePath.substr(beginIdx);
    if (!checkFileExits(filePath))
    {
        cout << "! File does not exist. Please check the file path\n";
        return;
    }

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

void getMoreTags(string tag, vector<string> &moreTags, TrieNode *current)
{
    if (current == nullptr)
    {
        return;
    }

    if (current->isEndofWord)
        moreTags.push_back(tag);

    for (int i = 0; i < 26; i++)
    {
        char ch = ('a' + i);
        getMoreTags(tag + ch, moreTags, (current->child)[i]);
    }

    return;
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
                // auto it = find(tags.begin(), tags.end(), row[row_size - 1]);
                // if (it != tags.end())
                //     fout << row[row_size - 1];
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
            cur = (cur->child)[tags[i][j] - 'a'];
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
    {
        cout << "MoveFile failed - " << GetLastError() << endl;
    }
    else
    {
        cout << "MoveFile success!" << endl;
        vector<string> row = readTags(path1), tags;
        for (int i = 3; i < row.size(); i++)
        {
            tags.push_back(row[i]);
        }

        vector<string> paths = {path1};
        removeFiles(paths);
        addTags(path2, tags);
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
        cout << "CopyFile failed - " << GetLastError() << endl;
    }
    else
    {
        vector<string> row = readTags(path1), tags;
        for (int i = 3; i < row.size(); i++)
        {
            tags.push_back(row[i]);
        }
        addTags(path2, tags);

        cout << "CopyFile success!" << endl;
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
        // metadataTags = getFileMetadataTags(findData.)
        if (strcmp(findData.cFileName, "..") == 0)
            continue;
        addTags(folderPath + "\\" + string(findData.cFileName), tags);
    }

    FindClose(hFind);
}

void setup()
{
    tagToFileHashes.clear();

    ifstream fin;
    fin.open("db.csv", ios::app);

    string line, token;
    vector<string> tokens;

    while (!fin.eof())
    {
        getline(fin, line);
        if (line != "")
        {
            stringstream linestream(line);
            while (getline(linestream, token, ','))
                tokens.push_back(token);

            for (int i = 1; i < tokens.size(); ++i)
                tagToFileHashes[tokens[i]].insert(tokens[0]);

            for (int i = 3; i < tokens.size(); ++i)
            {
                insertInTrie(tokens[i]);
            }

            FileInfo file;
            file.name = tokens[1];
            file.filePath = tokens[2];
            file.hash = tokens[0];

            string fileHash = tokens[0];
            hashToFileInfo[fileHash] = file;
            tokens.clear();
        }
    }

    fin.close();
}

void handle_input(const char input[], const size_t length)
{
    if (length == 0)
        return;

    char *tokenized_input[128];
    size_t num_tokens = 0;
    int res = tokenize_input(input, length, tokenized_input, num_tokens);

    if (res == 1 || num_tokens < 1)
    {
        std::cout << "! Invalid input provided\n";
        return;
    }

    char *command = tokenized_input[0];
    string filePath(tokenized_input[1]);

    if (strcmp(command, "delfile") == 0)
    {
        for (int index = 1; index < num_tokens; ++index)
            std::cout << tokenized_input[index] << "\n";
    }
    else if (strcmp(command, "addtags") == 0)
    {
        vector<string> tags;
        for (int i = 2; i < num_tokens; i++)
        {
            tags.push_back(tokenized_input[i]);
        }
        addTags(filePath, tags);
    }
    else if (strcmp(command, "readtags") == 0)
    {
        vector<string> tags = readTags(filePath);
        for (string tag : tags)
            cout << tag << " ";
    }
    else if (strcmp(command, "deletetags") == 0)
    {
        vector<string> tags;
        for (int i = 2; i < num_tokens; i++)
        {
            tags.push_back(tokenized_input[i]);
        }
        deleteTag(filePath, tags);
    }
    else if (strcmp(command, "find") == 0)
    {
        vector<string> tags;
        for (int i = 1; i < num_tokens; i++)
        {
            tags.push_back(tokenized_input[i]);
        }
        vector<FileInfo> result = findFiles(tags);
        for (int i = 0; i < result.size(); i++)
        {
            cout << "s no. - " << i + 1 << ", name - " << result[i].name << ", path - " << result[i].filePath;
            if (!checkFileExits(result[i].filePath))
            {
                cout << " ---Moved to other location---";
            }
            cout << endl;
        }

        cout << "Enter the s no. of file to open it else enter 0: ";
        int sno;
        cin >> sno;
        if (sno != 0)
        {
            string filePath = result[sno - 1].filePath;
            ShellExecuteA(NULL, "open", filePath.c_str(), NULL, NULL, SW_SHOWDEFAULT);
        }
    }
    else if (strcmp(command, "removefiles") == 0)
    {
        vector<string> paths;
        for (int i = 1; i < num_tokens; i++)
        {
            string filePath(tokenized_input[i]);
            paths.push_back(filePath);
        }
        removeFiles(paths);
    }
    else if (strcmp(command, "move") == 0)
    {
        string filePath2(tokenized_input[2]);
        move(filePath, filePath2);
    }
    else if (strcmp(command, "copy") == 0)
    {
        string filePath2(tokenized_input[2]);
        copy(filePath, filePath2);
    }
    else if (strcmp(command, "track") == 0)
    {
        vector<string> folderPaths, tags;
        int i;
        for (i = 1; i < num_tokens; ++i)
        {
            if (strcmp(tokenized_input[i], "-tags") == 0)
            {
                ++i;
                break;
            }
            folderPaths.push_back(string(tokenized_input[i]));
        }

        while (i < num_tokens)
            tags.push_back(tokenized_input[i++]);

        for (string &folderPath : folderPaths)
            trackFolder(folderPath, tags);
    }
    else
    {
        std::cout << "! Command \"" << command << "\" not a valid command\n";
    }
}

int main()
{
    setup();
    char input[128];
    cout << "\nWelcome to tag files\n";

    while (true)
    {
        cout << "\n> ";
        cin.getline(input, sizeof(char) * 128);
        handle_input(input, strlen(input));
    }

    free(input);
    return 0;
}