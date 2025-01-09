#ifndef EWN_H
#define EWN_H

#define MAX_ROW 9
#define MAX_COL 9
#define MAX_PIECES 6
#define MAX_PERIOD 18
#define MAX_PLIES 100
#define MAX_MOVES 16

extern int ROW;
extern int COL;

class EWN {
    int row, col;
    int board[MAX_ROW * MAX_COL];
    int pos[MAX_PIECES + 2];  // pos[0] and pos[MAX_PIECES + 1] are not used
    int dice_seq[MAX_PERIOD];
    int period;
    int goal_piece;

    int history[MAX_PLIES];
    int n_plies;
    int eaten = 0; // record the eaten piece (for A*)
    int piece_certainty[MAX_PIECES + 1] = {0}; // 0: total, 1~6: repective num of dice

    int ans = 1;

public:
    EWN();
    void scan_board();
    void print_board();
    bool is_goal();

    int move_gen_all(int *moves);
    void do_move(int move);
    void undo();

    float heuristic(int piece);
    int heuristic2();
    void sort_move(int *moves, int n_move);
    void apply_state_to_board(std::tuple<int, int, int, int, int, int, int> v);

    bool dfs(int *moves, std::stack<int> & path, int move_count, int limit);
    bool A_star(int limit);
    int get_ans();
    
};

#endif
