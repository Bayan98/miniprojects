//
//  main.cpp
//  Game Of Life
//
//  Created by Bayan on 27.09.16.
//  Copyright © 2016 Bayan. All rights reserved.
//
#include <bits/stdc++.h>
 
using namespace std;
 
class Board{
public:
   
    int width, height;
    char area[200][500];
    bool isChanged[200][500];
   
    Board(){
        width = height = 0;
        memset(isChanged, 0, sizeof isChanged);
    }
   
    void initialize(){
        ifstream read("input.txt");
        while(read >> area[height]){
            height++;
        }
        assert(height);
        width = (int)string(area[0]).size();
       
        for(int i = 0; i < height; i++){
            for(int j = 0; j < width; j++){
                if(area[i][j] == 'O') cout << "\033[37m";
                else cout << "\033[1m\033[32m";
                cout << area[i][j];
            }
            cout << "\n";
        }
        read.close();
    }
   
    int countOfNeighbour(int x, int y){
        int result = 0;
        if(area[x > 0 ? x-1 : height-1][y] == 'X') result++;
        if(area[x][y > 0 ? y-1 : width-1] == 'X') result++;
        if(area[x < height-1 ? x+1 : 0][y] == 'X') result++;
        if(area[x][y < width-1 ? y+1 : 0] == 'X') result++;
        if(area[x > 0 ? x-1 : height-1][y > 0 ? y-1 : width-1] == 'X') result++;
        if(area[x < height-1 ? x+1 : 0][y < width-1 ? y+1 : 0] == 'X') result++;
        if(area[x > 0 ? x-1 : height-1][y < width-1 ? y+1 : 0] == 'X') result++;
        if(area[x < height-1 ? x+1 : 0][y > 0 ? y-1 : width-1] == 'X') result++;
        return result;
    }
   
    void refresh(){
        for(int i = 0; i < height; i++){
            for(int j = 0; j < width; j++){
                const int count = countOfNeighbour(i, j);
                if(area[i][j] == 'O'){
                    if(count == 3){
                        isChanged[i][j] = 1;
                    }
                }
                if(area[i][j] == 'X'){
                    if(count < 2 || count > 3){
                        isChanged[i][j] = 1;
                    }
                }
            }
        }
    }
   
    void out(){
        for(int i = 0; i < height; i++){
            for(int j = 0; j < width; j++){
                if(isChanged[i][j]){
                    if(area[i][j] == 'X') {
                        area[i][j] = 'O';
                       cout << "\033[37m";     //gray color code
                    }
                    else {
                        area[i][j] = 'X';
                        cout << "\033[1m\033[32m";    //green color code
                    }
                    //"\e[x;yH" to change cursor position at coord [x;y]
                    printf("\e[%d;%dH%c", i+1, j+1, area[i][j]);
                    isChanged[i][j] = 0;
                }
            }
        }
        printf("\e[%d;%dH", height+1, 1);
    }
   
    void save(){
        ofstream write("output.txt");
        for(int i = 0; i < height; i++){
            for(int j = 0; j < width; j++){
                write << area[i][j];
            }
            write << "\n";
        }
        write.close();
    }
}board;
int main() {
    const double secondsLimit = 0.10;
    system("tput civis");  //hide cursor
    system("clear");
    board.initialize();
    int countdown = (int)clock();
    while(1){
        if(clock() > countdown + secondsLimit * CLOCKS_PER_SEC){
            board.refresh();
            countdown = (int)clock();
            board.save();
        }
        board.out();
    }
    return 0;
}
