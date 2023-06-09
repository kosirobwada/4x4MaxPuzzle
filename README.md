# 4x4MaxPuzzle
http://mikamir.wiki.fc2.com/wiki/%E3%83%81%E3%83%BC%E3%83%A02269 に詳しく書いています。
# 概要
4x4のマスの上に0~15の数がランダムに書かれている。今いるマスから1つ右のマスに移動、もしくは1つ下のマスに移動のいずれかを繰り返し、一番左上のマスから一番右下のマスに移動する。この際に移動したマスの上に書かれた点数を得ることができる。コマが通るマスに書かれている数字の合計が最も大きくなるようにコマを移動できればクリア、そうでなければ失敗、というゲーム。なお、コマを動かした際に得られる点の最大値は動的計画法を使ってただ一つに求まることが分かっている。

![image](https://github.com/kosirobwada/4x4MaxPuzzle/assets/97875031/5761c988-778b-461e-b3a0-493cce2b488b)
http://mikamir.wiki.fc2.com/jump/https/algo-method.com%2Ftasks%2F334
より写真を掲載。
このとき、1->2->9->8->7と移動すれば27点獲得でき、これが最大値である。この順にマスをたどることができればクリアそうでばければ失敗である。

# 操作方法
1.リセットボタンを押す。
2.teletermに盤面が表示されたらゲームスタート。
3.PCのキーボードの'e'を押した後、EnterKeyを押す。
（176Aのプログラムの名残が残っているので、直したい人は直してください。）
4.1番左上から始め、押したマスがXに変わる。押したボタンの右または下のボタンを押す。
（1回目の移動が画面に反映されてないです。2回目の移動の時に、1回目の移動が反映されます。
　入力に対して画面の反映が1回分遅れています。直せる人は直してください。）
5.右下に到着したら終了。
　（終了時に移動2回分が1度に表示されますが、得点結果に影響はありません。
　　表示の仕方が気になる人は直してください。得点結果に応じて、メッセージが表示されます。）
# 動的計画法
今回の最適化問題を解くために、マスの左上から右下へと至る経路を走査していくような動的計画法をする。

前提として、マスに書かれてある数は二次元配列queに格納されている。que[1][1]=1であり、上から i 行目、左から j 列目のマスに書かれている整数はque[i][j]である。

次のdpテーブルを用意する。
dp[i][j] : 上から i 行目、右から j 列目のマスにたどり着くまでの道順のうち、得られる得点の最大値。
![image](https://github.com/kosirobwada/4x4MaxPuzzle/assets/97875031/4e25b483-4021-4b0d-b930-8d8bddece6d0)
http://mikamir.wiki.fc2.com/jump/https/algo-method.com%2Ftasks%2F329%2Feditorial
り写真を掲載。

マスque[i][j]に至る最後の操作で場合分けを行う。
1.上のマスから下へ移動する場合、つまりque[i-1][j] ->que[i][j]へと移動する場合について考える。このときマス( i-1, j )に至るまでの最大スコアがdp[i-1][j]であることから、dp[i][j] = dp[i-1][j] + que[i][j]である。
2. 左のマスから右へ移動する場合、つまりque[i][j-1] ->que[i][j]へと移動する場合について考える。このときマス( i-1, j )に至るまでの最大スコアがdp[i][j-1]であることから、dp[i][j] = dp[i][j-1] + que[i][j]である。
3.これらから、次の関係が成り立つ。
　dp[i][j] = max(dp[i-1][j],dp[i][j-1])+que[i][j]
4.上のマスが存在しない場合（i-1 = 0である場合）は上のマス（i-1,j）を参照しないように注意する。また左のマスが存在しない場合（j-1 = 0である場合）も同様に左のマス（i,j-1）参照しないように注意する。
5.求める答え（最大値）は上から4列目、右から4列目のマスにたどり着くまでの道順のうち、得られる得点の最大値であるから、dp[4][4]である。

ここで、先ほどの3x3のマスの例を示す。なお、que、dpともに配列番号は図面の都合上削除してある。また、最大値をとる道順を青い矢印で表している。

![image](https://github.com/kosirobwada/4x4MaxPuzzle/assets/97875031/a08f3003-85e6-4dc5-bace-e472b096d130)
例えばdp[2][2]について考えてみよう。マス上に書かれた数はque[2][2]=3である。また、dp[1][2]=5、dp[2][1]=3より、dp[1][2]>dp[2][1]である。このため、マス上を[1][2]->[2][2]のように下に移動したほうがよさそうである。よって、dpテーブルをdp[2][2]=8に更新する。

# 回路図
![image](https://github.com/kosirobwada/4x4MaxPuzzle/assets/97875031/e97ce16b-d122-421c-9470-55c6e8e6725d)
- 4x4のマトリックスキーパッドの入力を読み取るために割り込みを発生させるTimerモジュールを使用している。
- PCとの通信を行うUARTモジュールとクロック供給用のCounterモジュールを使用している。
- Matrix keypadの回路図にcounter8,uartモジュールを追加し、配線する。

# hexキーパッド
![image](https://github.com/kosirobwada/4x4MaxPuzzle/assets/97875031/7c874de4-15bb-4d49-9a84-354a556cff0d)
- Hexキーパッドは紐の位置が右側に来るよう正面に置く。

![image](https://github.com/kosirobwada/4x4MaxPuzzle/assets/97875031/306acc2e-d2b8-4d97-8f34-8f14fd2b9b48)
- port0にマトリックスキーパッドをつなぎ、port1[3]とRX,port1[5]とTXをジャンパ線でつないだ。


