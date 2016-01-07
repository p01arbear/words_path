#include <iostream>
#include <fstream>
#include <unordered_map>
#include <exception>
#include <list>
#include <vector>
#include <memory>
#include <cstring>

using namespace std;

struct WordNode;

typedef struct WordLink
{
    WordLink(WordNode &node, short dsym_num) :
          node(node),
          dsym_num(dsym_num)
    {}

    WordNode   &node;
    short       dsym_num;

} WordLink;

typedef struct WordNode
{
    WordNode(const wstring& word, int chk_flag_idx) :
          word(word),
          chk_rcs_flag_idx(chk_flag_idx)
    {}

    const wstring &word;
    list<WordLink>   links;

    int chk_rcs_flag_idx;

} WordNode;

unordered_map<wstring, WordNode*>   words;
list<WordNode>      nodes;
vector<char>        nodes_chk_flags;

void reset_chk_flags()
{
    memset(nodes_chk_flags.data(), 0, nodes_chk_flags.size());
}

short is_one_diff(const wstring &a, const wstring &b)
{
    const wchar_t *pa = &a[0];
    const wchar_t *pa_end = pa + a.length();
    const wchar_t *pb = &b[0];
    char cnt = 1;
    short dsym_num = 0;
    while(pa < pa_end)
    {
        if (*pa != *pb)
        {
            if(--cnt < 0)
                return 0;
            dsym_num = pa - &a[0] + 1;
        }

        pa++;
        pb++;
    }

    return cnt == 0 ? dsym_num : 0;
}

ifstream& getwline(ifstream& stream, wstring& wstr)
{
    string getwline_str;
    getline(stream, getwline_str);
    wstr.resize(getwline_str.size());
    wstr.resize(mbstowcs(&wstr[0], getwline_str.c_str(), getwline_str.size()));
    return stream;
}

void link_recoursively(WordNode &node, WordNode &snode,
                     int dsym_num, int dsym_num_link)
{
    if (nodes_chk_flags[snode.chk_rcs_flag_idx])
        return;
    else
        nodes_chk_flags[snode.chk_rcs_flag_idx] = true;

    if (dsym_num == 0)
    {
        dsym_num = dsym_num_link = is_one_diff(snode.word, node.word);
        if (dsym_num == 0)
            return;
    }

    if (dsym_num == dsym_num_link)
    {
        for(auto& subn : snode.links)
            link_recoursively(node, subn.node, dsym_num, subn.dsym_num);

        node.links.push_back(WordLink(snode, dsym_num));
        snode.links.push_back(WordLink(node, dsym_num));
    }
}

typedef struct PathNode
{
    PathNode   *prev_path;
    WordNode   *word_node;

} PathNode;
typedef list<PathNode> Paths;

//return true on completion
bool find_paths(Paths *paths, WordNode* end, int *paths_cnt)
{
    bool path_finded = false;
    bool search_completed = true;

    auto paths_it = --paths->end();
    int old_paths_cnt = *paths_cnt;
    *paths_cnt = 0;
    for(; old_paths_cnt; --paths_it, --old_paths_cnt)
    {
        WordNode *word_node = paths_it->word_node;

        if (nodes_chk_flags[word_node->chk_rcs_flag_idx])
            continue;
        else
        {
            nodes_chk_flags[word_node->chk_rcs_flag_idx] = true;
            for (auto& link : word_node->links)
            {
                if (&link.node == end)
                    path_finded = true;
                else
                    search_completed = false;

                paths->push_back({ &*paths_it, &link.node});
                ++(*paths_cnt);
            }
        }
    }

    return path_finded || search_completed;
}

void print_path(PathNode *path, WordNode* beg)
{
    bool not_beg = path->word_node != beg;
    if (not_beg)
        print_path(path->prev_path, beg);

    wcout << path->word_node->word << endl;
}

void print_paths(Paths *paths, WordNode* beg, WordNode* end, int paths_cnt)
{
    auto paths_it = --paths->end();
    int cnt = 0;
    for(; cnt < paths_cnt; --paths_it, cnt++)
        if ((*paths_it).word_node == end)
        {
            print_path(&*paths_it, beg);
            wcout << endl;
        }
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        cout << "ERROR: Invalid arguments\n";
        return 0;
    }

    locale::global(locale(""));
    ios_base::sync_with_stdio(false);

    wstring firstw, lastw;

    std::ifstream infile;
    try
    {
        infile.open (argv[1]);
        if (!infile.good())
            throw runtime_error("ERROR in words file");

        getwline(infile, firstw);
        getwline(infile, lastw);
        infile.close();
        if (
                firstw.empty() || lastw.empty()
                || (firstw.length() != lastw.length())
                )
            throw runtime_error("Wrong input words file");

        uint word_length = firstw.length();

        infile.open (argv[2]);
        if (!infile.good())
            throw runtime_error("ERROR in vocabulary file");

        wstring line;
        while (getwline(infile, line))
        {
            if (line.length() != word_length)
                continue;

            auto res_pair = words.emplace(line, nullptr);
            if (res_pair.second)
            {
                nodes_chk_flags.push_back(false);

                nodes.push_back( WordNode(
                                     (*res_pair.first).first,
                                      nodes_chk_flags.size()-1)
                                 );
                WordNode &new_node = nodes.back();
                (*res_pair.first).second = &nodes.back();

                auto n = nodes.begin();
                auto n_end = --nodes.end();
                for(; n != n_end; n++)
                {
                    reset_chk_flags();
                    link_recoursively(new_node, *n, 0, 0);
                }
            }
        }

        auto first_it = words.find(firstw);
        auto last_it  = words.find(lastw);
        if (first_it == words.end() || last_it == words.end())
            throw runtime_error("No input word pair in vocabulary");

        WordNode *beg = first_it->second;
        WordNode *end = last_it->second;

        Paths paths;
        paths.push_back({nullptr, beg});

        int paths_cnt = 1;

        reset_chk_flags();
        while(!find_paths(&paths, end, &paths_cnt));
        if (paths_cnt == 0)
            cout << "No paths finded" << endl;
        else
            print_paths(&paths, beg, end, paths_cnt);

    }
    catch (exception &e)
    {
      std::cerr << e.what() << endl;
    }

    return 0;
}

