#include<iostream>
#include<windows.h>
using namespace std;
void draw(char board[][3] ) ;
void logic(char board[][3]);
int isgameover(char board[][3]);

bool isvalid=true;
int player=1;
int count=0;
HANDLE h=GetStdHandle(STD_OUTPUT_HANDLE);

int main(){
    char board[3][3]={{'1' , '2' , '3'} , {'4' , '5' , '6'} , {'7' ,'8' , '9'}};
    
    draw(board );
    
    while(isgameover(board)==0){
         logic(board);
        
         draw(board);
         
    }
    system("pause");
}


void draw(char board[][3]){
    for(int i=0 ; i<3 ; i++){
        for(int j=0 ; j<3 ; j++){
            if(board[i][j]=='X')
            SetConsoleTextAttribute(h , 13);
            else if(board[i][j]=='O')
            SetConsoleTextAttribute(h,4);

            cout<<board[i][j]<<" ";
            SetConsoleTextAttribute(h , 15);
            if(j!=2){
                cout<<"| ";
            }
        }
        cout<<endl;
        if(i!=2)
        cout<<"----------";
        cout<<endl;
    }
}


int isgameover(char board[][3]){
    bool draw=true;
    //Check all 3 rows
    for(int i=0 ; i<3 ; i++){
            if(board[i][0] == board[i][1] && board[i][1]==board[i][2] ){
                return 1;
            }
    }
    //check all 3 columns
    for(int i= 0 ; i<3 ; i++){
            if(board[0][i] == board[1][i] && board[1][i]==board[2][i] )
            {
                return 1;
        }
        }
    //check dioganals
    for(int i=0 ; i<1 ; i++){
        for(int j=0 ; j<1 ; j++){
            if(board[i][j] == board[i+1][j+1] && board[i+1][j+1]==board[i+2][j+2]){
                return 1;
            }
            if(board[i][j+2]==board[i+1][j+1] && board[i+1][j+1]==board[i+2][j]){
                return 1;
            }
        }
    }
   if(count==9)
    return -1;
    return 0;

}



void logic(char board[][3] ){
   char mark;
     int choice;
     int row,col;   
     mark = player==1 ?  'X' : 'O' ;
     cout<< (player ==1 ? (" Player 1 Choose the box: ") :  (" Player 2 Choose the box: ")) ;
          cin>>choice;
          system("cls");
          if(choice>0 && choice<10){
          row=(choice-1)/3;
          col=(choice-1)%3;
          if(board[row][col]!='X' && board[row][col]!='O'){
          board[row][col]=mark;
          isvalid=true;
          count++;
          }
          else{
            cout<<"Error: Already Marked with "<<board[row][col]<<endl;
            isvalid=false;
          }
          if(isgameover(board)==1){
            cout<<"Cograts player "<<player<<" won"<<endl;
          }
          else if(isgameover(board)==-1){
            cout<<"Game Draw"<<endl;
          }
          if(player==1 && isvalid){
            player++;
          }
          else if(player==2 && isvalid){
            player--;
          }     
     }
     else{
        cout<<"Invalid box number"<<endl;;
     }}