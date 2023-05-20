#ifndef TEST1_HPP
#define TEST1_HPP
#include <iostream>
#include <vector>
#include <unordered_map>
#include <queue>
#include <string>
#include <algorithm>
#include <chrono>
typedef std::pair<int, std::string> element;
typedef std::priority_queue<element, std::vector<element>, std::greater<element>> statePQ;

std::ostream &operator<<(std::ostream &out, std::string &str)
{
    int count = 0;
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            out << str[count++] << " ";
        }
        out << "\n";
    }
    out << "===================== \n";
    return out;
}

class solve
{
private:
    std::string start, goal;

public:
    solve(std::string &&i_start, std::string &&i_goal) : start(i_start), goal(i_goal)
    {
        int sum1 = 0, sum2 = 0;
        for (int i = 0; i < 9; ++i)
        {
            for (int j = 0; j < i; ++j)
            {
                if (start[i] > start[j])
                    ++sum1;
                if (goal[i] > goal[j])
                    ++sum2;
            }
        }

        if ((sum1 % 2 == 0 && sum2 % 2 != 0) || (sum1 % 2 != 0 && sum2 % 2 == 0))
        {
            std::cerr << "input error that is no anwser" << std::endl;
            exit(1);
        }
    }

private:
    std::vector<std::string> getSuccess(const std::string &stat);
    int heuristic(const std::string &start, const std::string goal);
    void printResult(std::unordered_map<std::string, int> &dist, std::unordered_map<std::string, std::string> &pred);
    std::vector<std::string> path;
    int cnt = 0;
    std::unordered_map<std::string, bool> ifVisit;
    bool dfs(std::string state);

public:
    void sloveByA();
    void sloveByBfs();
    void sloveByDfs();
};

void solve::printResult(std::unordered_map<std::string, int> &dist, std::unordered_map<std::string, std::string> &pred)
{
    std::cout << "Solution found!" << std::endl;
    std::cout << "Steps: " << dist[goal] << std::endl;
    std::vector<std::string> temp;
    temp.push_back(goal);
    std::string state = goal;
    while (state != start)
    {
        state = pred[state];
        temp.push_back(state);
    }
    pred.clear();
    dist.clear();
    for (auto pos = temp.crbegin(); pos != temp.crend(); ++pos)
    {
        std::string str = *pos;
        std::cout << str;
    }
    temp.clear();
}

std::vector<std::string> solve::getSuccess(const std::string &stat)
{
    std::vector<std::string> result;
    size_t pos = stat.find('0');
    if (pos > 2) // up
    {
        std::string newStat = stat;
        std::swap(newStat[pos - 3], newStat[pos]);
        result.push_back(newStat);
    }

    if (pos < 6) // down
    {
        std::string newStat = stat;
        std::swap(newStat[pos + 3], newStat[pos]);
        result.push_back(newStat);
    }

    if (pos % 3 < 2) // right
    {
        std::string newStat = stat;
        std::swap(newStat[pos + 1], newStat[pos]);
        result.push_back(newStat);
    }

    if (pos % 3 > 0) // left
    {
        std::string newStat = stat;
        std::swap(newStat[pos - 1], newStat[pos]);
        result.push_back(newStat);
    }

    return result;
}

int solve::heuristic(const std::string &start, const std::string goal)
{
    int dist = 0;
    for (int i = 0; i < 9; ++i)
    {
        if (start[i] != goal[i])
        {
            int x1 = i % 3;
            int y1 = i / 3;
            int x2 = goal.find(start[i]) % 3;
            int y2 = goal.find(start[i]) / 3;
            dist += std::abs(x1 - x2) + std::abs(y1 - y2);
        }
    }
    return dist;
}

void solve::sloveByA()
{
    statePQ pq;
    pq.emplace(std::make_pair(heuristic(start, goal), start));
    std::unordered_map<std::string, int> dist;
    std::unordered_map<std::string, std::string> pred;

    dist[start] = 0;
    int count = 0;
    while (!pq.empty())
    {
        ++count;
        std::string state = pq.top().second;
        int f = pq.top().first;
        pq.pop();
        if (state == goal)
        {
            std::cout << "seach " << count << " times" << std::endl;
            // 找到了解决方案
            printResult(dist, pred);
            return;
        }
        for (auto &succ : getSuccess(state))
        {
            int new_dist = dist[state] + 1;
            if (!dist.count(succ) || new_dist < dist[succ])
            {
                dist[succ] = new_dist;
                pred[succ] = state;
                pq.emplace(new_dist + heuristic(succ, goal), succ);
            }
        }
    }
    std::cout << "seach " << count << " times" << std::endl;
    std::cout << "No solution found." << std::endl;
}

void solve::sloveByBfs()
{
    std::unordered_map<std::string, int> dist;
    std::unordered_map<std::string, std::string> pred;
    std::queue<element> q;
    q.push(std::make_pair(0, start));
    dist[start] = 0;
    int count = 0;
    while (!q.empty())
    {
        std::string state = q.front().second;
        ++count;
        int f = q.front().first;
        q.pop();
        if (state == goal)
        {
            std::cout << "seach " << count << " times" << std::endl;
            printResult(dist, pred);
            return;
        }
        for (auto &succ : getSuccess(state))
        {
            int newdist = dist[state] + 1;
            if (!dist.count(succ) || dist[succ] > newdist)
            {
                dist[succ] = newdist;
                pred[succ] = state;
                q.emplace(std::make_pair(newdist, succ));
            }
        }
    }
    std::cout << "seach " << count << " times" << std::endl;
    std::cout << "No solution found." << std::endl;
}

bool solve::dfs(std::string state)
{
    ifVisit.emplace(std::make_pair(state, true));
    path.push_back(state);
    ++this->cnt;
    if (state == goal)
    {
        return true;
    }

    for (const auto succ : getSuccess(state))
    {

        if (dfs(succ))
        {
            return true;
        }
    }

    path.pop_back();
    return false;
}

void solve::sloveByDfs()
{

    if (dfs(start))
    {
        std::cout << "Solution found!" << std::endl;
        std::cout << "need " << cnt << " times" << std::endl;
        for (const auto &p : path)
        {
            std::string temp = p;
            std::cout << temp;
        }
    }
    else
    {
        std::cout << "No solution found." << std::endl;
    }
}

void test()
{
    // std::string start("012345678"); // 012345678 283104765
    // std::string goal("876543210");  // 876543210 283140765
    std::string start; // 012345678 283104765
    std::string goal;  // 876543210 283140765
    std::cout << "please input start state : ";
    std::cin >> start;
    std::cout << "please input goal state : ";
    std::cin >> goal;
    solve s(std::move(start), std::move(goal));
    clock_t begin = clock();
    s.sloveByBfs();
    clock_t end = clock();
    std::cout << "bfs need " << end - begin << "ms" << std::endl;
    std::cout << "====================================" << std::endl;
    s.sloveByA();
    begin = clock();
    std::cout << "A* need " << begin - end << "ms" << std::endl;
    std::cout << "====================================" << std::endl;
    // s.sloveByDfs();
    // end = clock();
    // std::cout << "dfs need " << end - begin << "ms" << std::endl;
}
#endif
