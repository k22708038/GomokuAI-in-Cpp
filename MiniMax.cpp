#include <algorithm>
#include <array>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <limits.h>
#include <list>
#include <map>
#include <set>
#include <vector>

enum SPOT_STATE {
    EMPTY = 0,
    BLACK = 1,
    WHITE = 2
};

enum Shape {
    NOTHING = 0,
    DALIVE1 = 10,
    ALIVE1 = 100,
    DALIVE2 = 100,
    ALIVE2 = 1000,
    DALIVE3 = 1000,
    ALIVE3 = 10000,
    DALIVE4 = 10000,
    ALIVE4 = 100000,
    WIN5 = 10000000
};

int player;
int opponent;
const int SIZE = 15;
const int DEPTH = 3;
bool first_hand = true;
int alive3_cnt = 0, death4_cnt = 0, alive4_cnt = 0; // for bonus shape combo
std::array<std::array<int, SIZE>, SIZE> board;

bool in_board(int x, int y) {
    return (y >= 0 && y < 15 && x >= 0 && x < 15);
}

bool empty_around(int x, int y) {
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (i == 0 && j == 0) {
                continue;
            }
            if (in_board(x + i, y + j)) {
                if (board[x + i][y + j] != EMPTY) {
                    return false;
                }
            }
        }
    }
    return true;
}

void read_board(std::ifstream &fin) {
    fin >> player;
    opponent = 3 - player;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            fin >> board[i][j];
            if (board[i][j] != EMPTY) {
                first_hand = false;
            }
        }
    }
}

bool OutstandingMove(int x, int y) {
    int serial_cnt = 1;
    for (int dx = x + 1; dx <= x + 4; dx++) {
        if (in_board(dx, y)) {
            if (board[dx][y] != player) {
                break;
            }
            serial_cnt++;
        } else {
            break;
        }
    }
    if (serial_cnt == 5) {
        return true;
    }
    serial_cnt = 1;
    for (int dx = x - 1; dx >= x - 4; dx--) {
        if (in_board(dx, y)) {
            if (board[dx][y] != player) {
                break;
            }
            serial_cnt++;
        } else {
            break;
        }
    }
    if (serial_cnt == 5) {
        return true;
    }

    serial_cnt = 1;
    for (int dy = y + 1; dy <= y + 4; dy++) {
        if (in_board(x, dy)) {
            if (board[x][dy] != player) {
                break;
            }
            serial_cnt++;
        } else {
            break;
        }
    }
    if (serial_cnt == 5) {
        return true;
    }
    serial_cnt = 1;
    for (int dy = y - 1; dy >= y - 4; dy--) {
        if (in_board(x, dy)) {
            if (board[x][dy] != player) {
                break;
            }
            serial_cnt++;
        } else {
            break;
        }
    }
    if (serial_cnt == 5) {
        return true;
    }

    int sum = x + y;
    serial_cnt = 1;
    for (int i = x + 1; i <= x + 4; i++) {
        if (in_board(i, sum - i)) {
            if (board[i][sum - i] != player) {
                break;
            }
            serial_cnt++;
        } else {
            break;
        }
    }
    if (serial_cnt == 5) {
        return true;
    }
    serial_cnt = 1;
    for (int i = x - 1; i <= x - 4; i--) {
        if (in_board(i, sum - i)) {
            if (board[i][sum - i] != player) {
                break;
            }
            serial_cnt++;
        } else {
            break;
        }
    }
    if (serial_cnt == 5) {
        return true;
    }

    int diff = x - y;
    serial_cnt = 1;
    for (int i = x + 1; i <= x + 4; i++) {
        if (in_board(i, i - diff)) {
            if (board[i][i - diff] != player) {
                break;
            }
            serial_cnt++;
        } else {
            break;
        }
    }
    if (serial_cnt == 5) {
        return true;
    }
    serial_cnt = 1;
    for (int i = x - 1; i <= x - 4; i--) {
        if (in_board(i, i - diff)) {
            if (board[i][i - diff] != player) {
                break;
            }
            serial_cnt++;
        } else {
            break;
        }
    }
    if (serial_cnt == 5) {
        return true;
    }

    return false;
}

int Shape_to_Score(int serial_cnt, bool SINGLE_LIVE, bool DOUBLE_LIVE, bool DOUBLE_DEATH) {
    int score = 0;
    if (serial_cnt == 1) {
        if (DOUBLE_LIVE) {
            score += ALIVE1;
        } else if (!DOUBLE_LIVE && SINGLE_LIVE) {
            score += DALIVE1;
        } else if (DOUBLE_DEATH) {
            score += NOTHING;
        }

    } else if (serial_cnt == 2) {
        if (DOUBLE_LIVE) {
            score += ALIVE2;
        } else if (!DOUBLE_LIVE && SINGLE_LIVE) {
            score += DALIVE2;
        } else if (DOUBLE_DEATH) {
            score += NOTHING;
        }

    } else if (serial_cnt == 3) {
        if (DOUBLE_LIVE) {
            score += ALIVE3;
            alive3_cnt++;
        } else if (!DOUBLE_LIVE && SINGLE_LIVE) {
            score += DALIVE3;
        } else if (DOUBLE_DEATH) {
            score += NOTHING;
        }
    } else if (serial_cnt == 4) {
        if (DOUBLE_LIVE) {
            score += ALIVE4;
            alive4_cnt++;
        } else if (!DOUBLE_LIVE && SINGLE_LIVE) {
            score += DALIVE4;
            death4_cnt++;
        } else if (DOUBLE_DEATH) {
            score += NOTHING;
        }
    } else if (serial_cnt == 5) {
        score += WIN5;
    }
    return score;
}

int Calculate_Score(const int Chess_player) {
    bool SINGLE_LIVE = false, DOUBLE_LIVE = false, DOUBLE_DEATH = false;

    int score = 0, serial_cnt = 0;
    alive3_cnt = 0, death4_cnt = 0, alive4_cnt = 0;
    const int Chess_oppoent = 3 - Chess_player;

    for (int i = 0; i < SIZE; i++) {
        serial_cnt = 1;
        for (int j = 0; j < SIZE; j++) {
            if (board[i][j] == Chess_player) {
                if (in_board(i, j - 1)) { //檢查頭部死活
                    if (board[i][j - 1] == EMPTY) {
                        SINGLE_LIVE = true;
                    } else if (board[i][j - 1] == Chess_oppoent) {
                        SINGLE_LIVE = false;
                    }
                } else {
                    SINGLE_LIVE = false;
                }

                if (in_board(i, j + 1)) { //檢查尾部死活
                    if (board[i][j + 1] == EMPTY) {
                        if (SINGLE_LIVE) {
                            DOUBLE_LIVE = true;
                            DOUBLE_DEATH = false;
                        } else {
                            SINGLE_LIVE = true;
                            DOUBLE_LIVE = false;
                            DOUBLE_DEATH = false;
                        }
                    } else if (board[i][j + 1] == Chess_oppoent) {
                        if (!SINGLE_LIVE) {
                            DOUBLE_DEATH = true;
                        }
                    }
                } else {
                    if (!SINGLE_LIVE) {
                        DOUBLE_DEATH = true;
                    }
                }
                serial_cnt++;

            } else if (board[i][j] != Chess_player) {
                score += Shape_to_Score(serial_cnt, SINGLE_LIVE, DOUBLE_LIVE, DOUBLE_DEATH);
                serial_cnt = 0;
                SINGLE_LIVE = false;
                DOUBLE_LIVE = false;
                DOUBLE_DEATH = false;
            }
        }
    }

    for (int j = 0; j < SIZE; j++) {
        serial_cnt = 0;
        SINGLE_LIVE = false;
        DOUBLE_LIVE = false;
        DOUBLE_DEATH = false;
        for (int i = 0; i < SIZE; i++) {
            if (board[i][j] == Chess_player) {
                if (in_board(i - 1, j)) { //檢查頭部死活
                    if (board[i - 1][j] == EMPTY) {
                        SINGLE_LIVE = true;
                    } else if (board[i - 1][j] == Chess_oppoent) {
                        SINGLE_LIVE = false;
                    }
                } else {
                    SINGLE_LIVE = false;
                }

                if (in_board(i + 1, j)) { //檢查尾部死活
                    if (board[i + 1][j] == EMPTY) {
                        if (SINGLE_LIVE) {
                            DOUBLE_LIVE = true;
                            DOUBLE_DEATH = false;
                        } else {
                            SINGLE_LIVE = true;
                            DOUBLE_LIVE = false;
                            DOUBLE_DEATH = false;
                        }
                    } else if (board[i + 1][j] == Chess_oppoent) {
                        if (!SINGLE_LIVE) {
                            DOUBLE_DEATH = true;
                        }
                    }
                } else {
                    if (!SINGLE_LIVE) {
                        DOUBLE_DEATH = true;
                    }
                }
                serial_cnt++;

            } else if (board[i][j] != Chess_player) {
                score += Shape_to_Score(serial_cnt, SINGLE_LIVE, DOUBLE_LIVE, DOUBLE_DEATH);
                serial_cnt = 0;
                SINGLE_LIVE = false;
                DOUBLE_LIVE = false;
                DOUBLE_DEATH = false;
            }
        }
    }

    for (int sum = 0; sum < 2 * SIZE - 1; sum++) {
        for (int i = 0; i <= sum; i++) {
            if (in_board(i, sum - i)) {
                if (board[i][sum - i] == Chess_player) {
                    if (in_board(i - 1, sum - (i - 1))) { //檢查頭部死活
                        if (board[i - 1][sum - (i - 1)] == EMPTY) {
                            SINGLE_LIVE = true;
                        } else if (board[i - 1][sum - (i - 1)] == Chess_oppoent) {
                            SINGLE_LIVE = false;
                        }
                    } else {
                        SINGLE_LIVE = false;
                    }

                    if (in_board(i + 1, sum - (i + 1))) { //檢查尾部死活
                        if (board[i + 1][sum - (i + 1)] == EMPTY) {
                            if (SINGLE_LIVE) {
                                DOUBLE_LIVE = true;
                                DOUBLE_DEATH = false;
                            } else {
                                SINGLE_LIVE = true;
                                DOUBLE_LIVE = false;
                                DOUBLE_DEATH = false;
                            }
                        } else if (board[i + 1][sum - (i + 1)] == Chess_oppoent) {
                            if (!SINGLE_LIVE) {
                                DOUBLE_DEATH = true;
                            }
                        }
                    } else {
                        if (!SINGLE_LIVE) {
                            DOUBLE_DEATH = true;
                        }
                    }
                    serial_cnt++;

                } else if (board[i][sum - i] != Chess_player) {
                    score += Shape_to_Score(serial_cnt, SINGLE_LIVE, DOUBLE_LIVE, DOUBLE_DEATH);
                    serial_cnt = 0;
                    SINGLE_LIVE = false;
                    DOUBLE_LIVE = false;
                    DOUBLE_DEATH = false;
                }
            }
        }
    }

    for (int diff = 1 - SIZE; diff < SIZE; diff++) {
        for (int i = 0; i < SIZE; i++) {
            if (in_board(i - diff, i)) {
                if (board[i - diff][i] == Chess_player) {
                    if (in_board(i - (diff + 1), i - 1)) { //檢查頭部死活
                        if (board[i - (diff + 1)][i - 1] == EMPTY) {
                            SINGLE_LIVE = true;
                        } else if (board[i - (diff + 1)][i - 1] == Chess_oppoent) {
                            SINGLE_LIVE = false;
                        }
                    } else {
                        SINGLE_LIVE = false;
                    }

                    if (in_board(i - (diff - 1), i + 1)) { //檢查尾部死活
                        if (board[i - (diff - 1)][i + 1] == EMPTY) {
                            if (SINGLE_LIVE) {
                                DOUBLE_LIVE = true;
                                DOUBLE_DEATH = false;
                            } else {
                                SINGLE_LIVE = true;
                                DOUBLE_LIVE = false;
                                DOUBLE_DEATH = false;
                            }
                        } else if (board[i - (diff - 1)][i + 1] == Chess_oppoent) {
                            if (!SINGLE_LIVE) {
                                DOUBLE_DEATH = true;
                            }
                        }
                    } else {
                        if (!SINGLE_LIVE) {
                            DOUBLE_DEATH = true;
                        }
                    }
                    serial_cnt++;

                } else if (board[i - diff][i] != Chess_player) {
                    score += Shape_to_Score(serial_cnt, SINGLE_LIVE, DOUBLE_LIVE, DOUBLE_DEATH);
                    serial_cnt = 0;
                    SINGLE_LIVE = false;
                    DOUBLE_LIVE = false;
                    DOUBLE_DEATH = false;
                }
            }
        }
    }

    if (alive3_cnt == 2 || (alive3_cnt == 1 && death4_cnt == 1)) {
        score += 50000;
    }
    return score;
}

int MiniMax(int maximizingPlayer, int depth, std::ofstream &fout) {
    using namespace std;
    int value;
    if (depth == 0) {
        int player_score = Calculate_Score(player);
        int oppoent_score = Calculate_Score(opponent);
        return player_score - oppoent_score * 1.3;
    }

    if (maximizingPlayer == player) {
        value = INT_MIN;
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                if (!empty_around(i, j) && board[i][j] == EMPTY) {
                    board[i][j] = player;
                    if (depth == DEPTH) {
                        if (OutstandingMove(i, j)) {
                            fout << i << " " << j << endl;
                            fout.flush();
                            return 0;
                        }
                    }
                    int score = MiniMax(opponent, depth - 1, fout);
                    board[i][j] = EMPTY;
                    if (score > value) {
                        value = score;
                        if (depth == DEPTH) {
                            fout << i << " " << j << endl;
                            fout.flush();
                        }
                    }
                }
            }
        }
    } else {
        value = INT_MAX;
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                if (!empty_around(i, j) && board[i][j] == EMPTY) {
                    board[i][j] = opponent;
                    int score = MiniMax(player, depth - 1, fout);
                    board[i][j] = EMPTY;
                    value = min(score, value);
                }
            }
        }
    }
    return value;
}

int main(int, char **argv) {
    std::ifstream fin(argv[1]);
    std::ofstream fout(argv[2]);
    read_board(fin);
    if (first_hand) {
        fout << 7 << " " << 7 << std::endl;
        fout.flush();
    } else {
        MiniMax(player, DEPTH, fout);
    }
    fin.close();
    fout.close();
    return 0;
}