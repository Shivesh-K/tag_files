#define UNICODE
#include <bits/stdc++.h>
#include <Windows.h>
#include "trie.h"
#include "cli_input.h"
#include "tag_functions.h"
using namespace std;

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

    if (strcmp(command, "create") == 0)
    {
        vector<string> filePaths, tags;
        int i;
        for (i = 1; i < num_tokens; ++i)
        {
            if (strcmp(tokenized_input[i], "-tags") == 0)
            {
                ++i;
                break;
            }
            filePaths.push_back(string(tokenized_input[i]));
        }

        while (i < num_tokens)
            tags.push_back(tokenized_input[i++]);

        for (string &tag : tags)
            transform(tag.begin(), tag.end(), tag.begin(), ::tolower);

        for (string &filePath : filePaths)
            createFile(filePath, tags);
    }
    else if (strcmp(command, "addtags") == 0)
    {
        vector<string> filePaths, tags;
        int i;
        for (i = 1; i < num_tokens; ++i)
        {
            if (strcmp(tokenized_input[i], "-tags") == 0)
            {
                ++i;
                break;
            }
            filePaths.push_back(string(tokenized_input[i]));
        }

        while (i < num_tokens)
            tags.push_back(tokenized_input[i++]);

        for (string &tag : tags)
            transform(tag.begin(), tag.end(), tag.begin(), ::tolower);

        for (string &filePath : filePaths)
            addTags(filePath, tags);
    }
    else if (strcmp(command, "readtags") == 0)
    {
        vector<string> tags = readTags(filePath);
        for (int i = 3; i < tags.size(); ++i)
            cout << tags[i] << " ";
        cout << "\n";
    }
    else if (strcmp(command, "deletetags") == 0)
    {
        vector<string> filePaths, tags;
        int i;
        for (i = 1; i < num_tokens; ++i)
        {
            if (strcmp(tokenized_input[i], "-tags") == 0)
            {
                ++i;
                break;
            }
            filePaths.push_back(string(tokenized_input[i]));
        }

        while (i < num_tokens)
            tags.push_back(tokenized_input[i++]);

        for (string &tag : tags)
            transform(tag.begin(), tag.end(), tag.begin(), ::tolower);

        for (string &filePath : filePaths)
            deleteTags(filePath, tags);
    }
    else if (strcmp(command, "find") == 0)
    {
        vector<string> tags;
        for (int i = 1; i < num_tokens; i++)
            tags.push_back(tokenized_input[i]);

        for (string &tag : tags)
            transform(tag.begin(), tag.end(), tag.begin(), ::tolower);

        vector<FileInfo> result = findFiles(tags);
        for (int i = 0; i < result.size(); i++)
        {
            cout << i + 1 << ". " << result[i].name << " (" << result[i].filePath << ") ";
            if (!checkFileExits(result[i].filePath))
                cout << " ~Moved to other location~";
            cout << "\n";
        }

        cout << "Enter the S.No. of file to open it. Enter 0 to exit: ";
        string sno;
        cin >> sno;
        if (sno.size() > 8)
        {
            sno = "0";
        }
        for (int i = 0; i < sno.size(); i++)
        {
            if (!('0' <= sno[i] && sno[i] <= '9'))
            {
                sno = "0";
                break;
            }
        }
        int sInt = stoi(sno);
        if (sInt != 0)
        {
            string filePath = result[sInt - 1].filePath;
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
            folderPaths.push_back(tokenized_input[i]);
        }

        while (i < num_tokens)
            tags.push_back(tokenized_input[i++]);

        for (string &tag : tags)
            transform(tag.begin(), tag.end(), tag.begin(), ::tolower);

        for (string &folderPath : folderPaths)
            trackFolder(folderPath, tags);
    }
    else if (strcmp(command, "exit") == 0)
    {
        exit(0);
    }
    else
    {
        std::cout << "! Command \"" << command << "\" not a valid command\n";
    }
}

int main()
{
    setup();
    char input[1024];
    cout << "\nWelcome to tag files\n";

    while (true)
    {
        cout << "\n> ";
        cin.getline(input, sizeof(char) * 1024);
        handle_input(input, strlen(input));
    }

    free(input);
    return 0;
}