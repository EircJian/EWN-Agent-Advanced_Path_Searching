#include<bits/stdc++.h>
#include "ewn.h"
using namespace std;

int main(){

    EWN ewn;
    ewn.scan_board();

    if(ewn.A_star(max(ROW, COL) * 5))
        return 0;
    cout << -1 << '\n';

    // limited-depth dfs
    // stack<int> path;
    // int *moves = (int *) malloc(sizeof(int) * MAX_MOVES);
    // int n_move = 0;
    
    // for(int i = 1; ; i++){
    //     bool find = ewn.dfs(moves, path, 0, i);
    //     if(find){
    //         n_move = i;
    //         break;
    //     }
    // }
    
    // n_move = ewn.get_ans();
    // cout << n_move << '\n';
    // while(path.size()){
    //     int piece = path.top() >> 4;
    //     int direction = path.top() & 15;
    //     path.pop();
    //     cout << piece << ' ' << direction << '\n';
    //  }
    return 0;
}

/*
piece_certainty
11: 5 plies, 38 
12: 9 plies, 5548
13: 9 plies, 16427
14: 10 plies, 59043
21: 12 plies, 130435
22: 13 plies, 347954
23: 12 plies, 153201
31: 13 plies, 316596
32: 14 plies, 358927
33: 16 plies, 794091 --> 32 plies, 189021
*/

/*
9 9 
3 0 0 0 1 0 0 0 0
0 0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 5 0
0 0 0 0 0 0 0 0 0
0 0 2 0 0 0 0 0 0
0 0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0 0
0 0 0 0 0 0 6 0 0
0 0 0 0 0 0 0 0 0

3
1 2 3 
3

*/