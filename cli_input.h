#include <bits/stdc++.h>
using namespace std;

char *create_token(const char *input, int begin, int end)
{
    if (begin > end || (begin == end && *input == ' '))
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
        cout << input[index];
        if (!in_string && (input[index] == ' ' || input[index] == 0))
        {
            char *new_token = create_token(input, start, index - 1);
            start = index + 1;
            if (new_token == NULL)
                continue;

            result_buffer[num_tokens++] = new_token;
        }
        else if (input[index] == '"')
        {
            if (in_string)
            {
                char *new_token = create_token(input, start, index - 1);
                in_string = false;
                start = index + 1;
                if (new_token == NULL)
                    continue;

                result_buffer[num_tokens] = new_token;
                ++num_tokens;
            }
            else
            {
                start = index + 1;
                in_string = true;
            }

            // start = index + 1;
            // in_string = !in_string;
        }
    }

    for (int index = 0; index < num_tokens; ++index)
    {
        *(tokenized_input + index) = result_buffer[index];
    }

    return 0;
}