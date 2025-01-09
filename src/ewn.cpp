#include<bits/stdc++.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "ewn.h"

using namespace std;

// 給定6個棋子的位置還有n_plies, 就可以算出g(P)以及預估出h(P)
// 第0位是 n_plies, 第1~6位是positions
using pieces_pos = tuple<int, int, int, int, int, int, int>;
using QueuePair = pair<float, pieces_pos>;

// these variables are available after calling EWN::scan_board()
int ROW;
int COL;
static int dir_value[8];

EWN::EWN() {
    row = 0;
    col = 0;
    pos[0] = 999;
    pos[MAX_PIECES + 1] = 999;
    for (int i = 1; i <= MAX_PIECES; i++) {
        pos[i] = -1;
    }
    period = 0;
    goal_piece = 0;
    n_plies = 0;
}

void set_dir_value() {
    dir_value[0] = -COL - 1;
    dir_value[1] = -COL;
    dir_value[2] = -COL + 1;
    dir_value[3] = -1;
    dir_value[4] = 1;
    dir_value[5] = COL - 1;
    dir_value[6] = COL;
    dir_value[7] = COL + 1;
}

void EWN::scan_board() {
    scanf(" %d %d", &row, &col);
    for (int i = 0; i < row * col; i++) {
        scanf(" %d", &board[i]);
        if (board[i] > 0)
            pos[board[i]] = i;
    }
    scanf(" %d", &period);
    for (int i = 0; i < period; i++) {
        scanf(" %d", &dice_seq[i]);
        piece_certainty[dice_seq[i]]++;
        piece_certainty[dice_seq[0]]++;
    }
    scanf(" %d", &goal_piece);

    // initialize global variables
    ROW = row;
    COL = col;
    set_dir_value();
}

void EWN::print_board() {
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            printf("%4d", board[i * col + j]);
        }
        printf("\n");
    }
}

bool EWN::is_goal() {
    if (goal_piece == 0) {
        if (board[row * col - 1] > 0) return true;
    }
    else {
        if (board[row * col - 1] == goal_piece) return true;
    }
    return false;
}

/*
move: an integer using only 12 bits
3 ~ 0: store the direction
7 ~ 4: store the piece number
11 ~ 8: store the eaten piece (used only in history)
*/
int move_gen2(int *moves, int piece, int location) {
    int count = 0;
    int row = location / COL;
    int col = location % COL;

    bool left_ok = col != 0;
    bool right_ok = col != COL - 1;
    bool up_ok = row != 0;
    bool down_ok = row != ROW - 1;

    if (up_ok) moves[count++] = piece << 4 | 1;
    if (left_ok) moves[count++] = piece << 4 | 3;
    if (right_ok) moves[count++] = piece << 4 | 4;
    if (down_ok) moves[count++] = piece << 4 | 6;

    if (up_ok && left_ok) moves[count++] = piece << 4 | 0;
    if (up_ok && right_ok) moves[count++] = piece << 4 | 2;
    if (down_ok && left_ok) moves[count++] = piece << 4 | 5;
    if (down_ok && right_ok) moves[count++] = piece << 4 | 7;

    return count;
}

int EWN::move_gen_all(int *moves) {
    int count = 0;
    int dice = dice_seq[n_plies % period];
    if (pos[dice] == -1) {
        int small = dice - 1;
        int large = dice + 1;

        while (pos[small] == -1) small--;
        while (pos[large] == -1) large++;

        if (small >= 1)
            count += move_gen2(moves, small, pos[small]);
        if (large <= MAX_PIECES)
            count += move_gen2(moves + count, large, pos[large]);
    }
    else {
        count = move_gen2(moves, dice, pos[dice]);
    }
    return count;
}

void EWN::do_move(int move) {
    int piece = move >> 4;
    int direction = move & 15;
    int dst = pos[piece] + dir_value[direction];

    if (n_plies == MAX_PLIES) {
        fprintf(stderr, "cannot do anymore moves\n");
        exit(1);
    }
    if (board[dst] > 0) {
        // record eaten piece in "move"
        eaten = board[dst];

        pos[board[dst]] = -1;
        move |= board[dst] << 8;
    }
    board[pos[piece]] = 0;
    board[dst] = piece;
    pos[piece] = dst;
    history[n_plies++] = move;
}

void EWN::undo() {
    if (n_plies == 0) {
        fprintf(stderr, "no history\n");
        exit(1);
    }

    int move = history[--n_plies];
    int eaten_piece = move >> 8;
    int piece = (move & 255) >> 4;
    int direction = move & 15;
    int dst = pos[piece] - dir_value[direction];

    if (eaten_piece > 0) {
        board[pos[piece]] = eaten_piece;
        pos[eaten_piece] = pos[piece];
    }
    else board[pos[piece]] = 0;
    board[dst] = piece;
    pos[piece] = dst;
}

void EWN::apply_state_to_board(pieces_pos v){
    memset(board, 0, sizeof(int) * (MAX_ROW * MAX_COL));

    n_plies = get<0>(v);
    if(get<1>(v) != -1) board[get<1>(v)] = 1;
    if(get<2>(v) != -1) board[get<2>(v)] = 2;
    if(get<3>(v) != -1) board[get<3>(v)] = 3;
    if(get<4>(v) != -1) board[get<4>(v)] = 4;
    if(get<5>(v) != -1) board[get<5>(v)] = 5;
    if(get<6>(v) != -1) board[get<6>(v)] = 6;

    pos[1] = get<1>(v);
    pos[2] = get<2>(v);
    pos[3] = get<3>(v);
    pos[4] = get<4>(v);
    pos[5] = get<5>(v);
    pos[6] = get<6>(v);
}

/*limited-depth dfs*/
bool EWN::dfs(int *moves, stack<int> & path, int move_count, int limit){
    
    // if(move_count > 20)
    //     return false;
    if(is_goal()){
        ans = move_count;
        return true;
    }
    if(limit <= 0)
        return false;
    if(pos[goal_piece] == -1)
        return false;
    
    // find all legal moves and do those moves
    int count = move_gen_all(moves);
    for(int i = count-1; i >= 0; i--){
        int *recursive_moves = (int *) malloc(sizeof(int) * MAX_MOVES);
        do_move(moves[i]);
        bool find_answer = dfs(recursive_moves, path, move_count+1, limit-1);
        
        
        free(recursive_moves);
        
        if(find_answer){
            // record the path
            path.push(moves[i]);
            undo();
            return true;
        }
        undo();     
    }
    return false;
}

bool EWN::A_star(int limit){

    // 給定6個棋子的位置還有n_plies, 就可以算出g(P)以及預估出h(P)
    // 第0位是 n_plies, 第1~6位是positions
    priority_queue<QueuePair, vector<QueuePair>, greater<QueuePair>> PQ;
    
    auto start = pieces_pos(n_plies, pos[1], pos[2], pos[3], pos[4], pos[5], pos[6]);
    map<pieces_pos, int> d;
    map<pieces_pos, int> from;
    map<pieces_pos, bool> visited;
    d[start] = 0;
    
    PQ.emplace(d[start], start);
    int j = 0;
    while(PQ.size()){
        j++;
        pieces_pos u = PQ.top().second;
        PQ.pop();

        apply_state_to_board(u);
        
        if(d[u] > limit)
            continue;
        if(is_goal()){
            //cout << "#plies = " << j << '\n';
            ans = d[u];
            cout << ans << '\n';

            // record path
            // use two for loop to save the space, and it will not waste too much time
            // because there are at most 100 plies.
            for(int i = d[u]; i > 0; i--){
                pieces_pos v = pieces_pos(i, pos[1], pos[2], pos[3], pos[4], pos[5], pos[6]);
                
                int move = from[v];
                history[--n_plies] = move;
                //cout << " " << move << " ";
                int eaten_piece = move >> 8;
                int piece = (move & 255) >> 4;
                int direction = move & 15;
                int dst = pos[piece] - dir_value[direction];
                if (eaten_piece > 0) {
                    //cout << "eating";
                    board[pos[piece]] = eaten_piece;
                    pos[eaten_piece] = pos[piece];
                }
                else board[pos[piece]] = 0;
                board[dst] = piece;
                pos[piece] = dst;
                //cout << "From the move: piece = " << piece << " dir = " << direction << '\n';
            }

            for(int i = 0; i < d[u]; i++){
                int piece = (history[i] & 255) >> 4;
                int direction = history[i] & 15;
                cout << piece << ' ' << direction << '\n';
            }

            return true;
        }

        if(visited[u])
            continue;
        visited[u] = true;

        int moves[MAX_MOVES];
        int count = move_gen_all(moves);
        for(int i = 0; i < count; i++){
            do_move(moves[i]);
            if(eaten){
                moves[i] |= eaten << 8;
                eaten = 0;
            }
            
            // Pruning: 
            // goal_piece must not be eaten!
            if(pos[goal_piece] == -1){
                undo();
                continue;
            }
            
            pieces_pos v = pieces_pos(n_plies, pos[1], pos[2], pos[3], pos[4], pos[5], pos[6]);
            
            // Pruning:
            // chebyshev distance between the current moving piece and the goal!
            float h_cost = heuristic(moves[i] >> 4);
            
            if(d.find(v) != d.end()){
                if(d[v] > d[u] + 1){
                    d[v] = d[u] + 1;
                    PQ.emplace((float)d[v] + h_cost, v);
                    from[v] = moves[i];
                }
            }
            else {
                d[v] = d[u] + 1;
                PQ.emplace((float)d[v] + h_cost, v);
                from[v] = moves[i];
            }
            undo();
        }
    }
    return false;
}



float EWN::heuristic(int piece){
    
    float cost = 0.0f;
    
    // 棋子決定度
    for(int i = 1; i <= MAX_PIECES; i++){
        cost += (pos[i] != -1);
    }


    // 棋子確定度 m >= 1, 越小越好, not used 因為怪怪的
    // float num_dice = 0.0f;
    // if(goal_piece != 0){
    //     if (pos[goal_piece] == -1) {
    //         int small = goal_piece - 1;
    //         int large = goal_piece + 1;
    //         while (pos[small] == -1) {
    //             num_dice += piece_certainty[small];
    //             small--;
    //         }
    //         while (pos[large] == -1) {
    //             num_dice += piece_certainty[large];
    //             large++;
    //         }
    //     }
    // }
    // else {
    //     if (pos[piece] == -1) {
    //         int small = piece - 1;
    //         int large = piece + 1;
    //         while (pos[small] == -1) {
    //             num_dice += piece_certainty[small];
    //             small--;
    //         }
    //         while (pos[large] == -1) {
    //             num_dice += piece_certainty[large];
    //             large++;
    //         }
    //     }
    // }
    // float m = (piece_certainty[0])/(piece_certainty[piece] + num_dice);
    // cost += (int)m;


    // 期望平均距離
    // use Chebyshev distance to calculate the estimated distance, and
    // use expectation of dice to calculate rounds.
    // Lastly, multiply these two together.
    if(goal_piece != 0){
        
        cost += max(ROW - pos[goal_piece]/COL - 1, COL - pos[goal_piece]%COL - 1);
        //cout << cost;
    }
    else {
        cost += max(ROW - pos[piece]/COL - 1, COL - pos[piece]%COL - 1);
        //cost *= (double)dice_stat[0]/(double)dice_stat[i];
    }
    return cost;
}

int EWN::get_ans(){
    return ans;
}