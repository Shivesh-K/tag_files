#include <bits/stdc++.h>
using namespace std;

struct FileInfo
{
    string name;
    string filePath;
    string hash;
};

map<string, set<string>> tagToFileHashes;
map<string, FileInfo> hashToFileInfo;

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

string getHash(string filePath)
{
    return filePath;
}

void addFile(string filePath, vector<string>tags)
{
    string fileHash = getHash(filePath);

    // TODO
    string fileName = "name";

    fstream fout;
    fout.open("db.csv", ios::out | ios::app);

    fout << fileHash << "," << fileName << "," << filePath << ",";
    for (int i = 0; i < tags.size(); i++) {
        fout << tags[i];
        if (i != tags.size() - 1)
            fout << ",";
    }
    fout << "\n";
    fout.close();
}

vector<string> readTags(string filePath)
{
    string fileHash = getHash(filePath);
    fstream fin;
    fin.open("db.csv", ios::in);

    bool flag = 0;
    vector<string> row;
    string line, word, temp, path;

    while (!fin.eof())
    {
        row.clear();
        getline(fin, line);
        stringstream s(line);

        while (getline(s, word, ','))
        {
            row.push_back(word);
        }

        path = row[1];
        if (path == fileHash) {
            flag = 1;
            return row;
        }
    }
    if (flag == 0)
        cout << "Record not found\n";

    return {};
}

void addTags(string filePath, vector<string>tags)
{
    string fileHash = getHash(filePath);

    fstream fin, fout;

    fin.open("db.csv", ios::in);
    fout.open("dbnew.csv", ios::out);

    string line, word, path;
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

        path = row[0];
        int row_size = row.size();


        if (path == fileHash) {
            flag = 1;
            for (string tag : tags)
                row.push_back(tag);

            if (!fin.eof())
            {
                for (int i = 0; i < row_size; i++)
                {
                    fout << row[i] << ',';
                }
                fout << row[row_size] << "\n";
            }
        }
        else
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
    if (flag == 0)
        cout << "File not added to application\n";

    fin.close();
    fout.close();

    remove("db.csv");
    rename("dbnew.csv", "db.csv");
}

void deleteTag(string filePath, string tag)
{
    string fileHash = getHash(filePath);

    fstream fin, fout;

    fin.open("db.csv", ios::in);
    fout.open("dbnew.csv", ios::out);


    string line, word, hash;
    vector<string> row;
    bool flag = 0;

    while (!fin.eof()) {

        row.clear();

        getline(fin, line);
        stringstream s(line);

        while (getline(s, word, ',')) {
            row.push_back(word);
        }

        hash = row[0];
        int row_size = row.size();

        if (hash == fileHash) {
            flag = 1;

            if (!fin.eof()) {
                for (int i = 0; i < row_size; i++) {
                    if (i <= 2 || row[i] != tag)
                        fout << row[i] << ',';
                }
                fout << row[row_size] << "\n";
            }
        }
        else {
            if (!fin.eof()) {
                for (int i = 0; i < row_size - 1; i++) {
                    fout << row[i] << ',';
                }

                fout << row[row_size - 1] << "\n";
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

        stringstream linestream(line);
        while (getline(linestream, token, ','))
            tokens.push_back(token);

        for (int i = 1; i < tokens.size(); ++i)
            tagToFileHashes[tokens[i]].insert(tokens[0]);
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
    string filePath = tokenized_input[1];
    if (std::strcmp(command, "addfile") == 0)
    {
        vector<string> tags;
        for (int i = 2; i < num_tokens; i++) {
            tags.push_back(tokenized_input[i]);
        }
        addFile(filePath, tags);
    }
    else if (std::strcmp(command, "delfile") == 0)
    {
        for (int index = 1; index < num_tokens; ++index)
            std::cout << tokenized_input[index] << "\n";
    }
    else if (std::strcmp(command, "addtags") == 0)
    {
        vector<string> tags;
        for (int i = 2; i < num_tokens; i++) {
            tags.push_back(tokenized_input[i]);
        }
        addTags(filePath, tags);
    }
    else if (std::strcmp(command, "readtags") == 0)
    {
        vector<string> tags = readTags(filePath);
        for (string tag : tags)
            cout << tag << " ";
    }
    else if (std::strcmp(command, "deletetag") == 0)
    {
        deleteTag(filePath, tokenized_input[2]);
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