#include<bits/stdc++.h>
using namespace std;


struct TrieNode
{
    char ele;
    bool isEndofWord;
    unordered_map<char, TrieNode *> child;
};



TrieNode *getNewNode(char ch)
{
    TrieNode *node = new TrieNode;
    node->ele = ch;
    node->isEndofWord = false;
    return node;
}

TrieNode *root = getNewNode('/');


void getMoreTags(string tag, vector<string> &moreTags, TrieNode *current)
{
    if (current == nullptr)
        return;

    if (current->isEndofWord)
        moreTags.push_back(tag);

    for (auto el : current->child)
        getMoreTags(tag + el.first, moreTags, el.second);

}

void insertInTrie(string &tag)
{
    int len = tag.length();

    TrieNode *current = root;

    for (int i = 0; i < len; i++)
    {

        char val = tag[i];
        if ((current->child)[val])
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
