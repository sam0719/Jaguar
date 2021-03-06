[toc]
# JA
 
## 對齊進度

|方程式|維度|其他說明|是否對齊|
|----|----|---|---|
|ABS|1||√|
|ABS|2||√|
|Sphere|1||√|
|Sphere|2||√|
|Rastrigin|1||√|
|Rastrigin|2||√|
|Rastrigin|1|srand(114),30次實驗|√|
|Ackley|1|srand(114),30次實驗|√|
|Rastrigin|2|srand(114),30次實驗||
|Ackley|2|srand(114),30次實驗||

## 對齊時遇到的情況

1. 位置**都**有移動,fitness相同,初始步伐減半跳出狩獵減速
2. 狩獵初期右左看後，左右位置其中有一邊與中間相等即可(對齊文件寫的是兩邊都要與中間相同)
3. 狩獵減速時左右位置和中間**都**不一樣，並且左右fitness與中間**都**一樣，直接結束狩獵
4. 跳脫距離是double類型,文檔裡實際上是float(用float的情況下 Rastrigin 30維30次實驗，第13次次數少了570次)
5. Ackley30維30次實驗裡,第27次實驗,42559fitness計算不同:

|版本|sum(pow(x,2))|-0.2*sqrt(sum1)|exp(-0.2*sqrt(sum1))|
|---|---|---|---|
|vs2015;vs2017|1088.35477258716673532035201788|1.20463259825678359327127964207|0.29980212710559139832611208476|
|vs2019;g++9|1088.35477258716673532035201788|1.20463259825678359327127964207|0.299802127105591453837263316018|
|比較|相同|相同|第16位開始不同|





### 2020/11/26
