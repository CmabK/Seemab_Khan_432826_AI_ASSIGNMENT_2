//Seemab Khan 
//432826
//8-Puzzle problem Using A-Star
//h1 distance from the original position 
#include <iostream>
#include <stdexcept>
#include <map>
#include <set>
#include <functional>
#include <string>
#include <vector>
#include <unordered_map>

using namespace std;

#define ASSERT_MSG(cond, msg)                                                                                                                   \
    {                                                                                                                                           \
        if (!(cond))                                                                                                                            \
            throw std::runtime_error("Assertion (" #cond ") failed at line " + std::to_string(__LINE__) + "! Msg '" + std::string(msg) + "'."); \
    }
#define ASSERT(cond) ASSERT_MSG(cond, "")

using u8 = uint8_t;

size_t constexpr n = 3, m = 3;



class Puzzle_Board
{
public:
    Puzzle_Board() : n_(n), m_(m), puzzle_board_(n_ * m_) {}
    Puzzle_Board(string const &s) : n_(n), m_(m), puzzle_board_(n_ * m_)
    {
        for (size_t i = 0; i < n_; ++i)
            for (size_t j = 0; j < m_; ++j)
                (*this)(i, j) = s.at(i * m_ + j) - '0';
    }
    u8 &operator()(size_t i, size_t j) { return puzzle_board_[i * m_ + j]; }
    u8 const &operator()(size_t i, size_t j) const { return const_cast<Puzzle_Board &>(*this)(i, j); }
    bool operator==(Puzzle_Board const &o) const { return puzzle_board_ == o.puzzle_board_; }
    vector<Puzzle_Board> Nearest() const
    {
        vector<Puzzle_Board> r;
        for (ptrdiff_t i = 0; i < n_; ++i)
            for (ptrdiff_t j = 0; j < m_; ++j)
                if ((*this)(i, j) == 0)
                {
                    for (pair<int, int> p : vector<pair<int, int>>{{0, -1}, {0, 1}, {-1, 0}, {1, 0}})
                    {
                        ptrdiff_t const ni = i + p.first, nj = j + p.second;
                        if (ni < 0 || ni >= n_ || nj < 0 || nj >= m_)
                            continue;
                        Puzzle_Board o = *this;
                        swap(o(i, j), o(ni, nj));
                        r.push_back(move(o));
                    }
                    break;
                }
        return move(r);
    }
    string Str(bool newline = false) const
    {
        string r;
        for (size_t i = 0; i < n_; ++i)
        {
            for (size_t j = 0; j < m_; ++j)
                r.append(1, (*this)(i, j) + '0');
            if (newline && i + 1 < n_)
                r.append(1, '\n');
        }
        return r;
    }
    size_t Minimun_Distance(Puzzle_Board const &to) const
    {
        size_t r = 0;
        for (ptrdiff_t i = 0; i < n_; ++i)
            for (ptrdiff_t j = 0; j < m_; ++j)
            {
                auto const v = (*this)(i, j);
                if (v == 0)
                    continue;
                size_t dist = size_t(-1);
                for (ptrdiff_t i2 = 0; i2 < n_; ++i2)
                {
                    for (ptrdiff_t j2 = 0; j2 < m_; ++j2)
                        if (to(i2, j2) == v)
                        {
                            dist = abs(i - i2) + abs(j - j2);
                            break;
                        }
                    if (dist != size_t(-1))
                        break;
                }
                ASSERT(dist != -1);
                r += dist;
            }
        return r;
    }

private:
    size_t n_ = 0, m_ = 0;
    vector<u8> puzzle_board_;
};

vector<Puzzle_Board> AStar_Sol_Algo(Puzzle_Board const &start, Puzzle_Board const &goal)
{
    using IdT = string;
    struct Entry
    {
        Puzzle_Board board;
        size_t g_cost_of_travilling_score = size_t(-1), function_score = size_t(-1);
        IdT originated{};
    };
    unordered_map<IdT, Entry> appear;
    map<size_t, set<IdT>> expose_set;

    auto H = [&](Entry const &e)
    {
        return e.board.Minimun_Distance(goal);
    };

    {
        Entry first{.board = start, .g_cost_of_travilling_score = 0};
        first.function_score = H(first);
        appear[first.board.Str()] = first;
        expose_set[first.function_score].insert(first.board.Str());
    }

    function<vector<Puzzle_Board>(IdT const &, size_t)> Rebuild_Route =
        [&](IdT const &id, size_t depth)
    {
        thread_local vector<Puzzle_Board> path;
        if (id == IdT{})
            return path;
        if (depth == 0)
            path.clear();
        auto const &e = appear.at(id);
        path.insert(path.begin(), e.board);
        return Rebuild_Route(e.originated, depth + 1);
        
    };

    while (!expose_set.empty())
    {
        auto const minimum_function_score = expose_set.begin()->first;
        auto const minimun_appear = expose_set.begin()->second;
        for (auto const &id : minimun_appear)
            if (appear.at(id).board == goal)
                return Rebuild_Route(id, 0);
        expose_set.erase(minimum_function_score);
        for (auto const &cid : minimun_appear)
        {
            auto const &cure = appear.at(cid);
            for (auto const &nbid : cure.board.Nearest())
            {
                size_t const tentative_g_cost_of_travilling_score = cure.g_cost_of_travilling_score + 1;
                auto const nid = nbid.Str();
                auto it = appear.find(nid);
                bool is_new = it == appear.end();
                if (is_new || tentative_g_cost_of_travilling_score < it->second.g_cost_of_travilling_score)
                {
                    if (is_new)
                        it = appear.insert({nid, Entry{.board = nbid}}).first;
                    it->second.originated = cid;
                    it->second.g_cost_of_travilling_score = tentative_g_cost_of_travilling_score;
                    if (!is_new)
                    {
                        auto it2 = expose_set.find(it->second.function_score);
                        if (it2 != expose_set.end() && it2->second.count(nid))
                        {
                            it2->second.erase(nid);
                            if (it2->second.empty())
                                expose_set.erase(it2);
                        }
                    }
                    it->second.function_score = tentative_g_cost_of_travilling_score + H(it->second);
                    expose_set[it->second.function_score].insert(nid);
                }
            }
        }
    }
    ASSERT_MSG(false, "Not accessible!");
}

void Sol(string const &start, string const &goal)
{
    auto const v = AStar_Sol_Algo(start, goal);
    size_t constexpr each_line = 5;
    bool finaL = false;
    for (size_t i = 0; !finaL; ++i)
    {
        for (size_t j = 0; j < n; ++j)
        {
            for (size_t i2 = 0; i2 < each_line; ++i2)
            {
                size_t const k = i * each_line + i2;
                if (k >= v.size())
                {
                    finaL = true;
                    for (size_t l = 0; l < (m + 5); ++l)
                        cout << " ";
                }
                else
                {
                    auto const &e = v.at(k);
                    auto const s = e.Str(true);
                    size_t pos = 0;
                    for (size_t ip = 0; ip < j; ++ip)
                        pos = s.find('\n', pos) + 1;
                    size_t position2 = min<size_t>(s.size(), s.find('\n', pos));
                    cout << s.substr(pos, position2 - pos) << (j == (n / 2) && k + 1 < v.size() ? " ==> " : "     ");
                }
                cout << (i2 + 1 >= each_line ? "\n" : "");
            }
        }
        cout << endl;
    }
}

int main()
{
	cout <<"A star : Informed Search technique" << endl;
    cout<<"8-Puzzle Game\nEnter 9 number like \n8 0 6 \n5 4 7 \n2 3 1 \n *Enter all digits in a single row "<<endl;
    
    int a;
    cin>>a;
    cout<<endl;

    try
    {
        Sol(to_string(a), "012345678");


        return 0;
    }
    catch (exception const &ex)
    {
        cout << "Exception: " << ex.what() << endl;
        return -1;
    }
}