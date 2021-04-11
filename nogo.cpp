#include "jsoncpp/json.h"
#include <cstdio>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>
#include <cmath>

using namespace std;

#define TIME_OUT_SET 0.96
#define INF 0x3f3f3f3f

int board[9][9];

bool dfs_air_visit[9][9];

const int cx[] = {-1, 0, 1, 0};
const int cy[] = {0, -1, 0, 1};

const int demo_x[] = {1,1,7,7,0,8,0,8};
const int demo_y[] = {1,7,1,7,0,0,8,8};
const int demo0_x[] = {0,0,8,8,1,7,1,7};
const int demo0_y[] = {0,8,0,8,1,1,7,7};

inline bool inBorder(int x, int y) { return x >= 0 && y >= 0 && x < 9 && y < 9; }

inline bool isBorder(int x, int y) { return x == 0 || y == 0 || x == 8 || y == 8; }

class Node {
public:
    int dfx = -1, dfy = -1;
    Node *parent = nullptr;
    vector<Node *> children;
    int visit_times = 0;
    double quality_value = 0.0;
    int current_board[9][9] = {0};
    int col = 0;
    vector<pair<int, int>> available_choices;

    void getAviliableAction() {
        available_choices.clear();
        for (int i = 0; i < 9; i++)
            for (int j = 0; j < 9; j++)
                if (judgeAvailable(i, j)) {
                    pair<int, int> a;
                    a.first = i;
                    a.second = j;
                    available_choices.push_back(a);
                }
    }

    bool dfs_air(int fx, int fy) {
        dfs_air_visit[fx][fy] = true;
        bool flag = false;
        for (int dir = 0; dir < 4; dir++) {
            int dx = fx + cx[dir], dy = fy + cy[dir];
            if (inBorder(dx, dy)) {
                if (current_board[dx][dy] == 0)
                    flag = true;
                if (current_board[dx][dy] == current_board[fx][fy] && !dfs_air_visit[dx][dy])
                    if (dfs_air(dx, dy))
                        flag = true;
            }
        }
        return flag;
    }

    bool judgeAvailable(int fx, int fy) {
        if (current_board[fx][fy])
            return false;
        current_board[fx][fy] = col;
        memset(dfs_air_visit, 0, sizeof(dfs_air_visit));
        if (!dfs_air(fx, fy)) {
            current_board[fx][fy] = 0;
            return false;
        }
        for (int dir = 0; dir < 4; dir++) {
            int dx = fx + cx[dir], dy = fy + cy[dir];
            if (inBorder(dx, dy)) {
                if (current_board[dx][dy] && !dfs_air_visit[dx][dy]) {
                    if (!dfs_air(dx, dy)) {
                        current_board[fx][fy] = 0;
                        return false;
                    }
                }
            }
        }
        current_board[fx][fy] = 0;
        return true;
    }

    double valueFunction() {
        int n1 = 0, n2 = 0;
        for (int i = 0; i < 9; i++)
            for (int j = 0; j < 9; j++)
                if (judgeAvailable(i, j)) {
                    n1++;
                }
        col = -col;
        for (int i = 0; i < 9; i++)
            for (int j = 0; j < 9; j++)
                if (judgeAvailable(i, j)) {
                    n2++;
                }
        col = -col;
        return (n2 - n1) / 81.0;
    }

    bool isAllExpanded() const {
        return available_choices.empty();
    }
};

Node *FindMax(Node *node, bool is_explor) {
    double max_score = -INF;
    Node *best_child = nullptr;
    double C = 0.0;
    if (is_explor)
        C = 1 / sqrt(2);
    for (int i = 0; i < node->children.size(); i++) {
        Node *p = node->children[i];
        double score =
                p->quality_value / (p->visit_times) + 2 * C *
                                                      sqrt(log(2 * node->visit_times) / (p->visit_times));
        if (p->dfy != -1 && p->dfx != -1) {
            int flag = 0;
            int flag0 = 0;
            for (int k = 0; k < 4; k++) {
                if (p->dfx + cx[k] < 9 && p->dfx + cx[k] >= 0 && p->dfy + cy[k] < 9
                    && p->dfy + cy[k] >= 0 &&
                    p->current_board[p->dfx + cx[k]][p->dfy + cy[k]] == p->col)
                    flag++;
                if (p->dfx + cx[k] < 9 && p->dfx + cx[k] >= 0 && p->dfy + cy[k] < 9
                    && p->dfy + cy[k] >= 0 &&
                    p->current_board[p->dfx + cx[k]][p->dfy + cy[k]] == -p->col)
                    flag0++;
            }
            if ((flag == 2 && isBorder(p->dfx, p->dfy) && flag0 != 1)||
                (flag == 3 && flag0 != 1)) score += 0.05;
            for(int k=0; k < 8; k++) {
                if(p->dfx==demo_x[k]&&p->dfy==demo_y[k]&&p->current_board[demo0_x[k]][demo0_y[k]]==p->col) {
                    score+=0.01;
                }
            }
            for (int k = 0; k < 4; k++) {
                if (p->dfx + cx[k] < 9 && p->dfx + cx[k] >= 0 && p->dfy + cy[k] < 9
                    && p->dfy + cy[k] >= 0 &&
                    p->current_board[p->dfx + cx[k]][p->dfy + cy[k]] != p->col
                    && p->current_board[p->dfx + cx[k]][p->dfy + cy[k]] != -p->col ) {
                    flag0=0;
                    for (int o = 0; o < 4; o++) {
                        if (p->dfx + cx[k] + cx[o] < 9 && p->dfx + cx[k] + cx[o] >= 0 && p->dfy + cy[k] + cy[o] < 9
                            && p->dfy + cy[k] + cy[o] >= 0 &&
                            p->current_board[p->dfx + cx[k] + cx[o]][p->dfy + cy[k] + cy[o]] == -p->col)
                            flag0++;
                    }
                    if(flag0==3||(flag0==2&&isBorder(p->dfx + cx[k],p->dfy + cy[k]))) score+=0.01;
                }
            }
        }
        if (score > max_score) {
            max_score = score;
            best_child = p;
        }
    }
    return best_child;
}

Node *expand(Node *node) {
    Node *new_node = new Node;
    int i = rand() % node->available_choices.size();
    pair<int, int> a = node->available_choices[i];
    node->available_choices.erase(node->available_choices.begin() + i); //清除已经展开的节点
    *new_node = *node;
    new_node->col = -node->col;
    new_node->current_board[a.first][a.second] = node->col;
    new_node->dfx = a.first, new_node->dfy = a.second;
    new_node->getAviliableAction();
    new_node->parent = node;
    node->children.push_back(new_node);
    return new_node;
}

Node *treePolicy(Node *node) {
    if (node->available_choices.empty() &&
        node->children.empty()) {
        return node;
    }
    if (node->isAllExpanded()) {
        Node *p = FindMax(node, true);
        return treePolicy(p);
    } else
        return expand(node);
}


inline double defaultPolicy(Node *node) {
    return node->valueFunction();
}

void backup(Node *node, double reward) {
    Node *p = node;
    while (p) {
        p->visit_times++;
        p->quality_value += reward;
        reward = -reward;
        p = p->parent;
    }
}

int main() {
    srand((unsigned) time(nullptr));
    string str;
    int x, y;

    getline(cin, str);

    int start = clock();
    int timeout = (int) (TIME_OUT_SET * (double) CLOCKS_PER_SEC);

    Json::Reader reader;
    Json::Value input;
    reader.parse(str, input);

    int first = 0;
    Node *node = new Node;
    if (input["requests"][first]["x"].asInt() == -1)
        node->col = 1;
    else
        node->col = -1;

    int color = node->col;
    int turnID = input["responses"].size();
    for (int i = 0; i < turnID; i++) {
        x = input["requests"][i]["x"].asInt(), y = input["requests"][i]["y"].asInt();
        if (x != -1) {
            board[x][y] = -color;
        }
        x = input["responses"][i]["x"].asInt(), y = input["responses"][i]["y"].asInt();
        if (x != -1) {
            board[x][y] = color;
        }
    }
    x = input["requests"][turnID]["x"].asInt(), y = input["requests"][turnID]["y"].asInt();
    if (x != -1) {
        board[x][y] = -color;
    }

    for (int i = 0; i < 9; i++)
        for (int j = 0; j < 9; j++)
            node->current_board[i][j] = board[i][j];

    node->getAviliableAction();

    while (clock() - start < timeout) {
        Node *expand_node = treePolicy(node);
        double reward = defaultPolicy(expand_node);
        backup(expand_node, reward);
    }

    Json::Value ret;
    Json::Value action;

    Node *best_child = FindMax(node, false);

    for (int i = 0; i < 9; i++)
        for (int j = 0; j < 9; j++)
            if (board[i][j] != best_child->current_board[i][j]) {
                action["x"] = i;
                action["y"] = j;
                break;
            }

    ret["response"] = action;
    Json::FastWriter writer;
    char buffer[4096];
    sprintf(buffer, "我是个伞兵！");
    ret["debug"] = buffer;
    cout << writer.write(ret) << endl;
    return 0;
}
