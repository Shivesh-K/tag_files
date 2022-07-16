#include <iostream>
#include <cstring>

char *create_token(const char *input, int begin, int end)
{
    if (begin > end)
        return NULL;

    size_t length = end - begin + 1;
    char *res = (char *)std::malloc(sizeof(char) * (length + 1));
    if (res == NULL)
        return NULL;

    std::memcpy(res, input + begin, sizeof(char) * length);
    res[length] = 0;

    return res;
}

int tokenize_input(const char input[], const size_t length, char **tokenized_input, size_t &num_tokens)
{
    bool in_string = false;
    int start = 0;

    char *result_buffer[128];
    std::memset(result_buffer, 0, sizeof(result_buffer));
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

    // tokenized_input = (char **)std::malloc(num_tokens * sizeof(char *));
    for (int index = 0; index < num_tokens; ++index)
    {
        *(tokenized_input + index) = result_buffer[index];
    }

    return 0;
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
    if (std::strcmp(command, "addfile") == 0)
    {
        for (int index = 1; index < num_tokens; ++index)
            std::cout << tokenized_input[index] << "\n";
    }
    else if (std::strcmp(command, "delfile") == 0)
    {
        for (int index = 1; index < num_tokens; ++index)
            std::cout << tokenized_input[index] << "\n";
    }
    else if (std::strcmp(command, "addtags") == 0)
    {
        for (int index = 1; index < num_tokens; ++index)
            std::cout << tokenized_input[index] << "\n";
    }
    else if (std::strcmp(command, "updatetags") == 0)
    {
        for (int index = 1; index < num_tokens; ++index)
            std::cout << tokenized_input[index] << "\n";
    }
    else
    {
        std::cout << "! Command \"" << command << "\" not a valid command\n";
    }
}

int main()
{
    char input[128];
    std::cout << "\nWelcome to tag files\n";
    while (true)
    {
        std::cout << "\n> ";
        std::cin.getline(input, sizeof(char) * 128);
        handle_input(input, std::strlen(input));
    }

    free(input);
    return 0;
}