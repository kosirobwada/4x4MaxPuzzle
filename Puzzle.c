/*日本語でコメントを書いています。文字化けしている可能性があるので、適宜コメントを消して使用してください。
*/
#include <m8c.h> // part specific constants and macros
#include "PSoCAPI.h" // PSoC API definitions for all User Modules
#include <stdlib.h>
#include <stdio.h>
#define MAX(a,b) ((a) > (b) ? (a) : (b)) //C言語にmax関数がなかったので、ここで定義しています。
/* These two variables are used to store the row number and the column number */

static const keypad_LUT[256] =
{/* 0 1 2 3 4 5 6 7
  8     9     A     B     C     D     E     F  */
/*0*/0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
/*1*/0x20, 'a', 'b', 0x20, 'c', 0x20, 0x20, 0x20,
 'd', 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
/*2*/0x20, 'e', 'f', 0x20, 'g', 0x20, 0x20, 0x20,
 'h', 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
/*3*/0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
/*4*/0x20, 'i', 'j', 0x20, 'k', 0x20, 0x20, 0x20,
 'l', 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
/*5*/0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
/*6*/0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
/*7*/0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
/*8*/0x20, 'm', 'n', 0x20, 'o', 0x20, 0x20, 0x20,
 'p', 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
/*9*/0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
/*A*/0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
/*B*/0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
/*C*/0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
/*D*/0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
/*E*/0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
/*F*/0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20
};
BYTE rows,cols;

/* These two variables store the key number */
unsigned char key[2] = {0x20,0};
/*****************************************************/

int dif = 1; //difは私たちが参考にした176Aが使っていたものになります。
int Max = 0; //最適経路の得点。
int Max_tmp; //出力する際にchar型に変換する必要があるため、一時的にMaxの値をint方で保存するために使いました。
int Sum = 0; //得点の合計。
int place = 7; //経路の復元の際に使います。詳しくは関数Restorationを見てください。
int i,j,k; //i,j,kはfor文の中で用いる変数。

/****************Functions****************************/

/* Handles keypad function- Checks the key pressed and debounces key */
void KeyPadHandler(void);

/* Keypad scanning routine */
unsigned char keypad_scan(void);

/* Initializes user modules used in the design */
void InitModules(void);

/* Interrupt handler for GPIO interrupt */
void GPIOInterrupt(void);
/****************************************************/
/*queは盤面を表す2次元配列です。名前は176Aが使っていたままqueになっています。
　詳しくはprint_boardを見てもらいたいのですが、que[i][j]=-1のとき空白を出力しています。
　また、配列外参照を防ぐために大きさ6x6の配列を用意したものとみられます。
*/
int que[6][6] = {{-1,-1,-1,-1,-1,-1},
                {-1, 1, 2, 3, 4,-1},
                 {-1, 5, 6, 7, 8,-1},
                    {-1, 9,10,11,12,-1},
                 {-1,13,14,15, 0,-1},
                 {-1,-1,-1,-1,-1,-1}};
//xLocは動く点を表します。はじめはque[1][1]にいて、que[4][4]に達したとき操作を終了します。
int xLoc[2]={1,1};
//配列dpは動的計画法で使います。配列外参照しないために、大きさを6x6にしました。4x4ではうまくうごきません。
int dp[6][6];
/*配列routeは通った経路を保存していくために使います。本当はrouteの中身を出力したかったのですができませんでした。
興味のある人はやってみてください。*/
/*配列hukugenは動的計画法の経路復元に用います。
最適経路を出力したかったのですが、できませんでした。興味のある人はやってみてください。*/


//print_boardは盤面を出力します。
void print_board(void){
 //int型の変数を出力する際に一度char型のポインタに変換し、UART_PutString(ポインタ名)で出力します。
  char *buffer;
  for(i=0;i<6;i++)que[0][i]=-1;
  for(i=0;i<6;i++)que[5][i]=-1;
  for(i=1;i<6;i++){
      for(j=0;j<6;j++){
          if(que[i][j]==-1)
              //盤面の外側の文字は出力せず、空白を出力する。
              UART_CPutString("   ");
          else if(que[i][j]==0)
              //配列の値が0のときはxを出力する。
              UART_CPutString("  x");
          else{
              /*itoa(ポインタ名,変換したい変数,10)でint型の変数をchar型のポインタに変換できます。
                10は10進数を意味します。
               */
              itoa(buffer,que[i][j],10);
              //1桁の数については空白を2つ出力する。
              if(que[i][j]<10)
                  UART_CPutString("  ");
              //2桁の数については空白を1つだけ出力する。
              else
                  UART_CPutString(" ");
              UART_PutString(buffer);
          }
      }
      //\rは「先頭に戻る」を意味する。
      UART_CPutString("\r\n");
  }
}

//make_boardは176Aのコードがわかりにくかったので、修正を加えた。
void make_board(void){
  int used[16];  //usedは0~15の文字を使用したかどうかを表している。使用したものは1。そうでないものは0。
   int t1;  //t1はランダム関数で作った文字を保持する変数。
   que[1][1]=0;  //マスの左上の値を0にする。
   used[0]=1;  //0を使用したので、used[0]の値を変更。
   for(i=1; i<=4; i++){
       for(j=1; j<=4; j++){
           if(i==1 && j==1)continue;  //que[1][1]の値は0なので変更しない。
           t1=rand()%16;  //rand関数を作り、0~15の値をt1に入れる。
                       //マスに書かれていない文字が出るまで、rand関数を動かす。
           while(used[t1]==1){
               t1=rand()%16;
           }
                       //マスに書かれていない文字を入れる。
           que[i][j]=t1;
                       //一度使用した文字usedのt1の値を変更する。
           used[t1]=1;
       }
   }
}

/*void print_route(void){
     k=1;route[1]=0;
      UART_CPutString("your route : ");
      for(k=1 ; k<=7 ; k++){
       for(i=1 ; i<=k ; i++){
              char buffer[3];
              itoa(buffer,route[i],10);
              UART_PutString(buffer);
              UART_CPutString("  ->  ");
          }
   }
      itoa(buffer,sum,10);
      UART_PutString(buffer);
      UART_CPutString(" /r/n");
}*/

//マス上で、xLocいる座標の値を0にする。例えばこのあとにprint_boardを呼び出せば、xを出力することができる。
void make_x(void){
  que[xLoc[0]][xLoc[1]]=0;
}
//TimerInterruptを定義。176Aから変更した点。
void TimerInterrupt(void);

//operationの挙動に注意。
void operation(void){
/*a,bはマス上の座標。詳しくはHexキーパッドの見出しを参照。
   flagは176Aが使っていたものです。場合分けの中身があまりわかっていません。
   変にいじっても怖いので残しています。
   
  */
 int a,b,c,flag=0,k=0;
 char *buffer;
  while(1){
  TimerInterrupt();
      if(key[0] != 0x20){
      c=key[0]-'a';
      //UART_PutString(&key[0]);
      switch(c){
          case 0: a=1;b=1;break;
          case 1: a=1;b=2;break;
          case 2: a=1;b=3;break;
          case 3: a=1;b=4;break;
          case 4: a=2;b=1;break;
          case 5: a=2;b=2;break;
          case 6: a=2;b=3;break;
          case 7: a=2;b=4;break;
          case 8: a=3;b=1;break;
          case 9: a=3;b=2;break;
          case 10: a=3;b=3;break;
          case 11: a=3;b=4;break;
          case 12: a=4;b=1;break;
          case 13: a=4;b=2;break;
          case 14: a=4;b=3;break;
          case 15: a=4;b=4;break;
          default: break; 
      }
          flag=0;
          //Sum += que[xLoc[0]][xLoc[1]];
          if(xLoc[0]==a&&xLoc[1]!=b){
              if(b-xLoc[1]==1&&xLoc[1]+1<5){
              //right
                 make_x();
                 xLoc[1]+=1;
                 //Sum += 1;
                 Sum+=que[xLoc[0]][xLoc[1]];
                 //itoa(buffer,Sum,10);
                 //UART_PutString(buffer);
                 //route[k]=que[xLoc[0]][xLoc[1]];
                 //k++;
              }
              else if(b-xLoc[1]==-1&&xLoc[1]-1>0){
              //left
              }
              else
                  flag=2;
          }
          else if(xLoc[1]==b&&xLoc[0]!=a){
              if(a-xLoc[0]==1&&xLoc[0]+1<5){
              //down
                  make_x();
                  xLoc[0]+=1;
                  //Sum += 1;
                  Sum+=que[xLoc[0]][xLoc[1]];
                  //itoa(buffer,Sum,10);
                  //UART_PutString(buffer);
                  //route[k]=que[xLoc[0]][xLoc[1]];
                  //k++;
              }
              else if(a-xLoc[0]==-1&&xLoc[0]-1>0){
              //up
              }
              else
                  flag=2;
          }
          else{
              flag=2;
          }
          if(!flag){
              key[0] = 0x20;
              break;
          }
          else if(flag==2){
          }
          key[0] = 0x20;
      }
      
  }
}
//ゴールしたら終了する。
int check_clear(void){
 if(xLoc[0]==4 && xLoc[1]==4)return 0;
  return 1;
}


//動的計画法を実行。詳しくは動的計画法の欄を見てください。MAX関数は冒頭define文を書いています。
int DP(void){
  dp[1][1]=que[1][1];
   for(i=1;i<=4;i++){
       for(j=1;j<=4;j++){
           if(i-1>=1){
               dp[i][j]=MAX(dp[i][j],dp[i-1][j]+que[i][j]);
           }
           if(j-1>=1){
               dp[i][j]=MAX(dp[i][j],dp[i][j-1]+que[i][j]);
           }
       }
   }
   return dp[4][4];
}
/*動的計画法の復元。できそうな人はやってみてください。詳しくは動的計画法の復元の欄を見てください。
void Restoration(void){
  i=4;j=4;
   while(1){
       if(place==1)break;
       if(dp[i][j]-dp[i-1][j]==que[i][j]){
           hukugen[place]=que[i][j];
           i--;place--;
       }
       else if(dp[i][j]-dp[i][j-1]==que[i][j]){
           hukugen[place]=que[i][j];
           j--;place--;
       }
   }
}
最適経路の出力。
void print_optimal(void){
  UART_CPutString("optimal route : ");
   for(i=1; i<=7 ; i++){
       char buffer[3];
       itoa(buffer,hukugen[i],10);
       UART_PutString(buffer);
       UART_CPutString(" -> ");
   }
   UART_CPutString("/r/n");
}*/

// ゴールした時に以下の場面が出ます。char型とint型がごっちゃになったためMAX_tmpを使用して一時的に保存していますが、使わなくてもいいと思います。
void result(int s, int d){
char buffer[10];
  if(Max==s){
          UART_CPutString("\r\n**********   clear!   **********\r\n\r\n");
  }
  UART_CPutString("----------   result   ----------\r\n");
  UART_CPutString("---------- your score ----------\r\n");
  itoa(buffer,s,10);
  UART_PutString(buffer);
  //itoa(buffer,Max,10);
  //UART_PutString(buffer);
  if(Max_tmp==s){
          UART_CPutString("\r\n********** conguratulation! **********\r\n");
       UART_CPutString("********** you clear it!!!! **********\r\n\r\n");
  }
  else{
          UART_CPutString("\r\n********** disappointing... **********\r\n");
       UART_CPutString("********** you lose it!!!!! **********\r\n\r\n");
  }
  UART_CPutString("--------------------------------\r\n");
}


void main(void)
{
/*finはゴールに達したかを確認するための変数。
   count,d,tcは176Aが使っていたものを消すのが怖かったのでそのまま残しています。
  */
 int fin=1,count=0,d,tc=0;
 
  char * strPtr, *buffer; 
  /* Initialize variables and user modules */
  InitModules();
  UART_CmdReset();                      // Initialize receiver/cmd  
                                        // buffer  
  UART_IntCntl(UART_ENABLE_RX_INT);     // Enable RX interrupts
  Counter8_WritePeriod(155);            // Set up baud rate generator  
  Counter8_WriteCompareValue(77);
  Counter8_Start();                   // Turn on baud rate generator       
  UART_Start(UART_PARITY_NONE);        // Enable UART   
  
  /* Enable Global Interrupt */
  M8C_EnableGInt;
  UART_CPutString("\r\nWelcome to 4x4MaxPuzzle!\r\n\r\nPlease input 'e' and EnterKey\r\n\r\n(e:easy d:difficult)-> ");
  //while文は176Aが作ったものほぼそのままになっています。各自使いやすいように変えてください。
  while(1) {
      tc++;
     if(UART_bCmdCheck()){   
        if(strPtr = UART_szGetParam()) {
           UART_PutString(strPtr);
           if(*strPtr=='e')break;
        }   
     UART_CmdReset();  
     }   
 }
 //if(UART_bCmdCheck())UART_CmdReset();
 srand(tc*10);
 make_board();
 //Maxの値をはじめに出力しておきます。おそらく、初期状態の左下あたりに出るかと思います。60~70近くの数になると思います。
 Max = DP();
 Max_tmp=Max;
 UART_CPutString("\r\n                                \r\n");
 itoa(buffer, Max,10);
 UART_PutString(buffer);
 UART_CPutString("\r\n                                \r\n");
 //Restoration();
 //print_optimal();
 //print_board();
  while(fin){
      print_board();
      operation();
      //print_route();
      //print_board();
      fin=check_clear();
  }
  print_board();
  que[4][4]=0;
  print_board();
  result(Sum,d);
  return;
}





/* This function initializes all the user modules used in the design */
void InitModules(void)
{
/* Initilize timer- 10ms delay */
  Timer16_1_WritePeriod(320);
  Timer16_1_EnableInt();
      
  /* Enable GPIO interrupt */
  INT_MSK0|=0x20;
      
  /* Write 1's at column lines */
  PRT0DR|=0xF0;
}


/* This function scan the keypad and identifies the key pressed */
unsigned char keypad_scan(void)
{
BYTE key_result;
      
  /* Drive rows */
  PRT0DR = 0x0F;
  
  /* Read columns */
  rows = PRT0DR;
  
  /* Drive columns */
  PRT0DR = 0xF0;
  
  /* Read rows */
  cols = PRT0DR;
  
  /* Combine results */
  key_result = rows & cols;
      
  /* Get the key number from LUT */
  return(keypad_LUT[key_result]);
}


/* This is the interrupt handler for the GPIO interrupt. Debounce timer is started on GPIO interrupt */
#pragma interrupt_handler GPIOInterrupt
void GPIOInterrupt(void)
{
/* Disable GPIO interrupt */
  INT_MSK0&=~0x20;    
      
  /* Start Timer */
  Timer16_1_Start();
}


/* This is the interrupt handler for the timer. Scanning routine is called for key identification */
void TimerInterrupt(void)
{
/* Stop the timer and update the flag */
  Timer16_1_Stop();
      
  /* Get the key */
  key[0] = keypad_scan();
  /* Clear GPIO posted interrupt */
  INT_CLR0&=~0x20;
      
  /* Enable GPIO interrupt */
  INT_MSK0|=0x20; 
}