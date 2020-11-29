#include <iostream>
#include <iomanip>
#include <math.h>
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <string>
#include <time.h>
#include <tuple>

using namespace std;

#define RAND_MAX 32767.0
#define PI acos(-1)
#define DIMENSION 30
#define RUNTIME 30

static int state;
void srand(int seed);
int rand();

int run_time, update_floor_count; // 實驗次數
int dimension;
double cal_fitness(float x, string Info);
float hunting(float location);
float speed_up(float location);
float speed_down(float location);
float location_floor[DIMENSION][10000]; // 記錄走過的坑底,可能要用二維數組 [維度][坑數]
void epin_write(string address, string Info, float best_location, float now_location);
int JA_count = 0, gb_count; // 計算次數,最佳解次數
float location[DIMENSION], f_location, pb_location[DIMENSION]; // 位置
int tmp_hunting_count, hunting_count, update_gb_fitness_count = 0; // 狩猎次数,,更新最佳解計算次數,坑數
int  floor_count[DIMENSION];
double pb_fitness, pb_fitness_r, pb_fitness_l;
float jump_r_location[DIMENSION], jump_l_location[DIMENSION]; // 兩倍跳出後的位置
float gb_location[DIMENSION]; // 坑底最佳位置和fitness
double gb_fitness;
float gb_fitness_float;
double tmp_fitness;
float speed, f_speed, speed_c; // 步伐
double fitness, new_fitness, g_fitness;
int c, jump_c; // 控制右左跳
bool small_floor; //遇到小平台
float DOMAIN_UPPER = 15;
float DOMAIN_LOWER = -15;

bool epin = 0;// 控制輸出epin
bool DEBUG = 0; // 控制输出debug信息

int main()
{
    clock_t start, end;
    start = clock();
    srand(114);
    for (int i = 0; i < 30 * 12; i++) {
          //float test = rand();
    }
    // cout<<"rand_max:"<<RAND_MAX<<endl;
    // cout<< rand_num << endl;
    // cout<<setprecision(20)<<rand_num<<endl;//  ran()
    // location[0] = pb_location[0] = gb_location[0] = (1263 / RAND_MAX) * (DOMAIN_UPPER-DOMAIN_LOWER) + DOMAIN_LOWER;
    for (run_time = 0; run_time < RUNTIME; run_time++) {
        update_floor_count = 0;
        int use_rand_count = 0;
        bool floor_is_hunting = 0;
        JA_count = 0;
        tmp_hunting_count = 0;
        hunting_count = 0;
        update_gb_fitness_count = 0;
        c = 1;
        jump_c = 1;
        for (int i = 0 ;i < DIMENSION;i++) {
             floor_count[i] = 0;
        }
        // 初始化
        // location [0] = float(1263/RAND_MAX);
        // location[0] = pb_location[0] = gb_location[0] = location[0] * (DOMAIN_UPPER - DOMAIN_LOWER) + DOMAIN_LOWER;
        // location [1] = float(5797/RAND_MAX);
        // location[1] = pb_location[1] = gb_location[1] = location[1] * (DOMAIN_UPPER - DOMAIN_LOWER) + DOMAIN_LOWER;
        for (int i = 0; i < DIMENSION; i++) {
            location[i] = float(rand() / RAND_MAX);
            use_rand_count++;
            location[i] = pb_location[i] = gb_location[i] = location[i] * (DOMAIN_UPPER - DOMAIN_LOWER) + DOMAIN_LOWER;
        }
        for (dimension = 0; dimension < DIMENSION; dimension++) {
            if (DEBUG)
                cout << "進入第" << dimension + 1 << "維度,當前計算次數:" << JA_count << endl;
            f_location = location[dimension];
            pb_location[dimension] = location[dimension];
            // cout<< setprecision(20) << location << endl; // 位置
            bool is_jump_domain_r, is_jump_domain_l, is_find_new_floor_r, is_find_new_floor_l, is_stop_r, is_stop_l, is_worst_fitness;
            is_jump_domain_r = is_jump_domain_l = is_find_new_floor_r = is_find_new_floor_l = is_stop_r = is_stop_l = is_worst_fitness = false;
            location[dimension] = hunting(location[dimension]);
            if(DEBUG)
                cout << "狩獵總計算次數:" << JA_count << ",找到最佳解的次數:" << tmp_hunting_count << endl;
            gb_location[dimension] = location[dimension];
            gb_fitness = fitness;
            double r = fabs(f_location - location[dimension]); // 半徑
            if(r == 0.0) {
                r = powf(2.0, (int)log2(DOMAIN_UPPER) - 11);
            }

            jump_r_location[dimension] = gb_location[dimension] + r * 2;
            float jump_r_location_first = jump_r_location[dimension];
            if (jump_r_location[dimension] > DOMAIN_UPPER) {
                jump_r_location[dimension] = DOMAIN_UPPER;
                is_jump_domain_r = 1; // 右跳是否跳出定義域
                if(DEBUG)
                    cout << "右跳跳出定義域" << endl;
            }
            else
                is_jump_domain_r = 0;
            jump_l_location[dimension] = gb_location[dimension] - r * 2;
            float jump_l_location_first = jump_l_location[dimension];
            if (jump_l_location[dimension] < DOMAIN_LOWER) {
                jump_l_location[dimension] = DOMAIN_LOWER;
                is_jump_domain_l = 1; // 左跳是否跳出定義域
                if(DEBUG)
                    cout << "左跳跳出定義域" << endl;
            }
            else
                is_jump_domain_l = 0;
            //cout << "向右跳脱" << endl;
            pb_location[dimension] = jump_r_location[dimension];
            jump_r_location[dimension] = hunting(jump_r_location[dimension]);
            pb_fitness_r = fitness;
            if (gb_location[dimension] != jump_r_location[dimension]){
                is_find_new_floor_r = 1;
                if(DEBUG)
                    cout << "右跳找到新的坑底:" << jump_r_location[dimension] << ",計算次數:" << JA_count << endl;
            }

            //cout << "向左跳脫" << endl;
            pb_location[dimension] = jump_l_location[dimension];
            jump_l_location[dimension] = hunting(jump_l_location[dimension]);
            pb_fitness_l = fitness;
            if (gb_location[dimension] != jump_l_location[dimension]){
                is_find_new_floor_l = 1;
                if(DEBUG)
                    cout << "左跳找到新的坑底:" << jump_l_location[dimension] << ",計算次數:" << JA_count << endl;
            }
            if(DEBUG)
                cout << "中間坑底：" << setprecision(20) << gb_location[dimension] << ",中間fitness:" << gb_fitness << ",右邊坑底:" << jump_r_location[dimension] << ",右邊fitness:" << pb_fitness_r << "左邊坑底:" << jump_l_location[dimension] << "左邊fitness:" << pb_fitness_l << endl;
            // 三個坑底比較
            float r_r = fabs(jump_r_location_first - gb_location[dimension]);
            float r_l = fabs(jump_l_location_first - gb_location[dimension]);
            if (is_find_new_floor_l == 1 || is_jump_domain_l == 1)
                r_l = -1;
            if (is_find_new_floor_r == 1 || is_jump_domain_r == 1)
                r_r = -1;
            while ((is_find_new_floor_r == 0 && is_jump_domain_r == 0) || (is_find_new_floor_l == 0 && is_jump_domain_l == 0))
            {
                // cout << "三個坑底比較" << endl;
                r = max(r_r, r_l);
                // 要改的地方 左右半徑選大的那個 如果有一邊不做 不需要設置半徑 設為-1
                if (is_jump_domain_r == 0 && is_find_new_floor_r == 0) {
                    jump_r_location_first = gb_location[dimension] + r * 2.0;
                    if (jump_r_location_first > DOMAIN_UPPER) {
                        jump_r_location_first = DOMAIN_UPPER;
                        is_jump_domain_r = 1; // 右跳是否跳出定義域
                        if(DEBUG)
                            cout << "右跳跳出定義域" << endl;
                    }
                    pb_location[dimension] = jump_r_location_first;
                    jump_r_location[dimension] = hunting(jump_r_location_first);
                    pb_fitness_r = fitness;
                    if (gb_location[dimension] != jump_r_location[dimension]){
                        is_find_new_floor_r = 1;
                        if(DEBUG)
                            cout << "右跳找到新的坑底:" << jump_r_location[dimension] << ",計算次數:" << JA_count << endl;
                    }
                }
                if (is_jump_domain_l == 0 && is_find_new_floor_l == 0) {
                    jump_l_location_first = gb_location[dimension] - r * 2.0; // 
                    if (jump_l_location_first < DOMAIN_LOWER) {
                        jump_l_location_first = DOMAIN_LOWER;
                        is_jump_domain_l = 1; // 左跳是否跳出定義域
                        if(DEBUG)
                            cout << "左跳找跳出定義域" << endl;
                    }
                    pb_location[dimension] = jump_l_location_first;
                    jump_l_location[dimension] = hunting(jump_l_location_first);
                    pb_fitness_l = fitness;
                    if (gb_location[dimension] != jump_l_location[dimension]){
                        is_find_new_floor_l = 1;
                        if(DEBUG)
                            cout << "左跳找到新的坑底:" << jump_l_location[dimension] << ",計算次數:" << JA_count << endl;
                    }
                }
                r_r = fabs(jump_r_location_first - gb_location[dimension]);
                r_l = fabs(jump_l_location_first - gb_location[dimension]);
                if (is_find_new_floor_l == 1 || is_jump_domain_l == 1)
                    r_l = -1;
                if (is_find_new_floor_r == 1 || is_jump_domain_r == 1)
                    r_r = -1;

            }
            if ((float)gb_fitness <= (float)pb_fitness_r && (float)gb_fitness <= (float)pb_fitness_l) {
                if(DEBUG)
                    cout << "左右兩邊的fitness都大於等於中間fitness" << endl;
                if ((is_jump_domain_l == 1 && is_jump_domain_r == 1) && (jump_r_location[dimension] == gb_location[dimension] && jump_l_location[dimension] == gb_location[dimension])) { // 存疑 同一個坑底是不是應該用位置來判斷
                    if(DEBUG)
                        cout << "左右都跳出定義域，並且左右fitness與中間一樣,結束跳脫" << endl;
                    continue;
                }
                else if (is_find_new_floor_l == true || is_find_new_floor_r == true) {
                    if(DEBUG)
                        cout << "左或右其中一邊找到新的坑底,進入跳脫減速階段" << endl;
                    is_worst_fitness = 1;
                }
            }
            else if ((float)pb_fitness_r == (float)pb_fitness_l && (float)pb_fitness_r < (float)gb_fitness) {
                if(DEBUG)
                    cout << "左右fitness相同並且比中間的好,以隨機或固定的方式往其中一邊跳脫加速" << endl;
                r = fabs(jump_r_location[dimension] - gb_location[dimension]);
                is_stop_l = 1;
            }
            else if ((float)pb_fitness_r < min((float)pb_fitness_l, (float)gb_fitness) || (float)pb_fitness_l < min((float)pb_fitness_r, (float)gb_fitness)) {
                if(DEBUG)
                    cout << "左或右有一邊比另外兩個好，往好的那一邊跳脫加速" << endl;
                if ((float)pb_fitness_l < (float)pb_fitness_r) {
                    if(jump_l_location[dimension] > gb_location[dimension]) {
                        if(DEBUG)
                            cout << "特殊情況,往左跳跨坑,實際坑底在右邊,往右跳" << endl;
                        r = fabs(jump_l_location[dimension] - gb_location[dimension]);
                        jump_r_location[dimension] = jump_l_location[dimension];
                        pb_fitness_r = pb_fitness_l;
                        is_stop_l = 1;
                    } else {
                        r = fabs(jump_l_location[dimension] - gb_location[dimension]);
                        if(DEBUG)
                            cout << "往左跳脫" << endl;
                        is_stop_r = 1;
                    }
                }
                else if ((float)pb_fitness_r < (float)pb_fitness_l) {
                    if (jump_r_location[dimension] < gb_location[dimension]) {
                        if(DEBUG)
                            cout << "特殊情況,往右跳跨坑,實際坑底在左邊,往左跳" << endl;
                        r = fabs(jump_r_location[dimension] - gb_location[dimension]);
                        jump_l_location[dimension] = jump_r_location[dimension];
                        pb_fitness_l = pb_fitness_r;
                        is_stop_r = 1;
                    } else {
                        r = fabs(jump_r_location[dimension] - gb_location[dimension]);
                        if(DEBUG)
                            cout << "往右跳脫" << endl;
                        is_stop_l = 1;
                    }
                }
            }
            is_jump_domain_r = is_jump_domain_l = false;
            if(DEBUG)
                cout << "進入加速" << ",計算次數:" << JA_count << endl;
            while ((is_jump_domain_r == false && is_jump_domain_l == false) && is_worst_fitness == false) {
                if (is_stop_r == 1) {
                    if(DEBUG)
                        cout << "往左邊加速，跳脫距離:" << r * 2 << ",計算次數:" << JA_count << endl;
                    r *= 2;
                    jump_c = -1;
                    gb_location[dimension] = jump_l_location[dimension];
                    gb_fitness = pb_fitness_l;
                    location[dimension] = jump_l_location[dimension];
                    jump_l_location[dimension] = location[dimension] - r;
                    if (jump_l_location[dimension] < DOMAIN_LOWER) {
                        if(DEBUG)
                            cout << "往左邊跳脫加速跳出定義域" << endl;
                        jump_l_location[dimension] = DOMAIN_LOWER;
                        is_jump_domain_l = 1;
                    }
                    jump_l_location[dimension] = hunting(jump_l_location[dimension]);
                    pb_fitness_l = fitness;
                    if ((float)pb_fitness_l >= (float)gb_fitness) {
                        if(DEBUG)
                            cout << "往左邊跳脫加速找到較差的坑" << endl;
                        is_worst_fitness = true;
                    }
                }
                else if (is_stop_l == 1) {
                    if(DEBUG)
                        cout << "往右邊跳脫加速，跳脫距離:" << r * 2<< ",計算次數:" << JA_count << endl;
                    r *= 2;
                    jump_c = 1;
                    gb_location[dimension] = jump_r_location[dimension];
                    gb_fitness = pb_fitness_r;
                    location[dimension] = jump_r_location[dimension];
                    jump_r_location[dimension] = location[dimension] + r;
                    if (jump_r_location[dimension] > DOMAIN_UPPER) {
                        if(DEBUG)
                            cout << "往右邊跳脫加速跳出定義域" << endl;
                        jump_r_location[dimension] = DOMAIN_UPPER;
                        is_jump_domain_r = 1;
                    }
                    jump_r_location[dimension] = hunting(jump_r_location[dimension]);
                    pb_fitness_r = fitness;
                    if ((float)pb_fitness_r >= (float)gb_fitness) {
                        if(DEBUG)
                            cout << "往右邊跳脫加速找到新的較差的坑" << endl;
                        is_worst_fitness = true; //找到不好的解
                    }
                }
            }
            if ((float)pb_fitness_l < min((float)pb_fitness_r,(float)gb_fitness)){
                gb_location[dimension] = jump_l_location[dimension];
                gb_fitness = pb_fitness_l;
            } else if((float)pb_fitness_r < min((float)pb_fitness_l,(float)gb_fitness)){
                gb_location[dimension] = jump_r_location[dimension];
                gb_fitness = pb_fitness_r;
            }
            // cout << "中間坑底：" << gb_location[dimension] << ",中間fitness:" << gb_fitness << ",右邊坑底:" << jump_r_location[dimension] << ",右邊fitness:" << pb_fitness_r << "左邊坑底:" << jump_l_location[dimension] << "左邊fitness:" << pb_fitness_l << endl;
            // 最佳坑底同方向跳一次減半距離 另一邊已經看過 不用看
            if(DEBUG)
                cout << "進入減速" << ",加速結束時跳脫距離為:" << r << ",計算次數:" << JA_count << endl;
            while (1) {
                r /= 2.0;
                jump_r_location[dimension] = gb_location[dimension] + r;
                jump_l_location[dimension] = gb_location[dimension] - r;
                if(jump_r_location[dimension] == gb_location[dimension] || jump_l_location[dimension] == gb_location[dimension]) {
                    if(DEBUG)
                        cout << "左右跳出去的位置有一邊跟中間相同" << endl;
                    break;
                }
                if (jump_r_location[dimension] > DOMAIN_UPPER)
                    jump_r_location[dimension] = DOMAIN_UPPER;
                jump_r_location[dimension] = hunting(jump_r_location[dimension]);
                pb_fitness_r = fitness;          
                if (jump_l_location[dimension] < DOMAIN_LOWER)
                    jump_l_location[dimension] = DOMAIN_LOWER;
                jump_l_location[dimension] = hunting(jump_l_location[dimension]);
                pb_fitness_l = fitness;
                if ((float)pb_fitness_r == (float)pb_fitness_l && (float)pb_fitness_r < (float)gb_fitness) {
                    if(DEBUG)
                        cout << "隨機左右選一邊當最佳，然後繼續循環" << endl;
                    gb_location[dimension] = jump_r_location[dimension];
                    gb_fitness = pb_fitness_r;
                }
                else if ((float)pb_fitness_r < (float)gb_fitness || (float)pb_fitness_l < (float)gb_fitness) {
                    if(DEBUG)
                        cout << "左右不相同並且有一邊優於中間,計算次數:" << JA_count << endl;
                    if ((float)pb_fitness_r < (float)pb_fitness_l) {
                        if(DEBUG)
                            cout << "右邊比較好" << endl;
                        gb_location[dimension] = jump_r_location[dimension];
                        gb_fitness = pb_fitness_r;
                    }
                    else if ((float)pb_fitness_l < (float)pb_fitness_r) {
                        if(DEBUG)
                            cout << "左邊比較好" << endl;
                        gb_location[dimension] = jump_l_location[dimension];
                        gb_fitness = pb_fitness_l;
                    }
                }
                else if (jump_l_location[dimension] == gb_location[dimension] && jump_r_location[dimension] == gb_location[dimension]) {
                    if(DEBUG)
                        cout << "狩獵到底的左右和中間的位置都相同" << endl;
                    break;
                } 
                else if ((float)pb_fitness_r >= (float)gb_fitness && (float)pb_fitness_l >= (float)gb_fitness){
                    if(DEBUG)
                        cout << "左右都差於中間,count:" << JA_count << endl;
                }
                else {
                    cout << "有問題" << endl;
                }
                // cout << "中間坑底：" << gb_location[dimension] << ",中間fitness:" << gb_fitness << ",右邊坑底:" << jump_r_location[dimension] << ",右邊fitness:" << pb_fitness_r << "左邊坑底:" << jump_l_location[dimension] << "左邊fitness:" << pb_fitness_l << endl;
            }
            location[dimension] = gb_location[dimension];
            if(DEBUG){
                cout << "中間坑底：" << gb_location[dimension] << ",中間fitness:" << gb_fitness << ",右邊坑底:" << jump_r_location[dimension] << ",右邊fitness:" << pb_fitness_r << "左邊坑底:" << jump_l_location[dimension] << "左邊fitness:" << pb_fitness_l << endl;
                cout << "減速結束" << endl;
            }
            
        }
        int floor_count_sum = 0;
        for (int i = 0;i< DIMENSION;i++) {
            floor_count_sum += floor_count[i];
        }
        cout << "第" << run_time + 1 << "次實驗找到最佳解:" << gb_fitness << ",總計算次數:" << JA_count << ",找到最佳解次數:" << update_gb_fitness_count << ",坑數:" << floor_count_sum << ",更新坑計算次數:" << update_floor_count << ",rand()使用次數:" << use_rand_count << endl;
    }

    // cout<< speed<<endl; // 輸出減速後的speed
    end = clock();
    cout << "time:" << (double)(end - start) / CLOCKS_PER_SEC << endl;
    return 0;

}


float hunting(float location) {
    hunting_count = -1;
    ostringstream oss;
    oss << "狩獵開始";
    f_speed = powf(2.0, (int)log2(DOMAIN_UPPER) - 11);
    speed = f_speed;
    speed_c = 1;
    pb_location[dimension] = location;
    c = 0;
    fitness = cal_fitness(location, oss.str());
    hunting_count = JA_count - 1; // 記錄從第幾次開始狩獵
    pb_fitness = fitness;
    // cout<<setprecision(20)<<fitness<< endl; // fitness
    // cout << speed<<endl; // 初始步伐
    float new_location_r, new_location;
    float new_location_l = 0.0;
    double fitness_r = 0.0, fitness_l = 0.0;
    int k = 0; // 
    float f;

    while (k == 0) {
        f = 0;
        speed = f_speed;
        new_location_r = location + speed;
        new_location_l = location - speed;
        // cout << setprecision(20) << new_location_l << "," << new_location_r << "," << location << speed << endl;
        if (new_location_r == location || new_location_l == location) { // 存疑 對齊文檔寫左右和中間相同，實際epin看起來是有一邊相同就行
            if(DEBUG)
                cout << "狩獵右左看位置有一邊和中間一樣,結束狩獵";
            break;
        }
        // cout<< "右左看" <<endl;
        if ((new_location_r <= DOMAIN_UPPER && new_location_r >= DOMAIN_LOWER)) {
            // cout<< "right:" << new_location_r << endl;
            c = 1;
            fitness_r = cal_fitness(new_location_r, "狩獵向右看"); // 向右跳
        }
        else
            f = 1;
        if (new_location_l <= DOMAIN_UPPER && new_location_l >= DOMAIN_LOWER) {
            // cout<< "left:" << new_location_l << endl;
            c = -1;
            fitness_l = cal_fitness(new_location_l, "狩獵向左看"); // 向左跳
        }
        else
            f = 2;
        // cout <<"test:"<< fitness_l << ","<< fitness_r << "," << fitness <<endl;
        // if( fitness_l == fitness && fitness_r == fitness) {
        //     cout << "結束";
        //     break;
        // }
        if ((fitness_l >= fitness || f == 2) && (fitness_r >= fitness || f == 1)) { // 如果右左看的fitness都大於等於最好位置
            f_speed /= 2;
            speed = f_speed;
            continue;
        }
        else if (f == 1) {
            c = -1;
            location = new_location_l;
            pb_location[dimension] = location;
            fitness = fitness_l;
        }
        else if (f == 2) {
            c = 1;
            location = new_location_r;
            pb_location[dimension] = location;
            fitness = fitness_r;
        }
        else if (fitness_l < fitness_r) {
            c = -1;
            location = new_location_l;
            pb_location[dimension] = location;
            fitness = fitness_l;
        }
        else if (fitness_l >= fitness_r) {
            c = 1;
            location = new_location_r;
            pb_location[dimension] = location;
            fitness = fitness_r;
        }
        speed *= 2;
        speed_c *= 2;
        // cout<< "進入加速"<<endl;
        location = speed_up(location); // 加速
        // cout<< "沒有比較好，速度減半按照原來方向再看一次" <<endl;
        new_location = location + c * speed;
        if (new_location < DOMAIN_UPPER && new_location > DOMAIN_LOWER) {
            new_fitness = cal_fitness(new_location, "加速到較差解,速度減半按照原來方向看");
            if (new_fitness < fitness) {
                fitness = new_fitness;
                location = new_location;
                pb_location[dimension] = location;
            }
        }
        k = 0;
        c = 1;
        location = speed_down(location);
        if(small_floor == 1) {
            if(DEBUG)
                cout << "遇到小平台,結束狩獵" << endl;
            small_floor = 0;
            break;
        }
        f_speed /= 2;
        speed = f_speed;
        // cout<< "速度回到1，初始步伐減半" <<endl;
    }
    bool floor_is_hunting = 0;
    for (int i = 0; i < floor_count[dimension]; i++) {
        if (location == location_floor[dimension][i]) { // 坑底位置已經走過
            floor_is_hunting = 1;
            break;
        }
    }
    if (floor_is_hunting == 0) {
        if(DEBUG)
            cout << "找到新坑底:" << location << endl;
        location_floor[dimension][floor_count[dimension]] = location;
        floor_count[dimension]++;
        update_floor_count = JA_count;
    }
    return location;
}

float speed_up(float location) {
    float new_location = location + c * speed;
    if (new_location > DOMAIN_UPPER || new_location < DOMAIN_LOWER) { // 超出邊界 一樣
        // new_location -= c * speed;
        speed /= 2;
        speed_c /= 2;
        // k *= -1;
        return location;
    }
    ostringstream oss;
    if (c == 1) {
        oss << "向右加速";
    }
    else if (c == -1) {
        oss << "向左加速";
    }
    new_fitness = cal_fitness(new_location, oss.str());
    if (new_fitness > fitness) { // 如果遇到不好的值 位置回到當前最好 減速並跳出
        // new_location -= c * speed;
        speed /= 2;
        speed_c /= 2;
        // k *= -1;
        return location;
    }
    speed *= 2;
    speed_c *= 2;
    fitness = new_fitness;
    // pb_location[dimension] = location;
    location = new_location;
    pb_location[dimension] = location;
    //  cout<< speed<<endl;
    return speed_up(location);
}

float speed_down(float location) {
    // cout<<"進入減速"<<endl;
    c = 1;
    double fitness_r, fitness_l;
    bool is_jump_domain_r = 0, is_jump_domain_l = 0;
    if (c * speed != f_speed) {
        speed /= 2;
        speed_c /= 2;
        float new_location_r = location + speed;
        float new_location_l = location - speed;
        if (new_location_l == new_location_r && new_location_l == location) {
            return location; //  失去精細度
        }
        if (new_location_r > DOMAIN_UPPER) { // 超出解空間,不計算,當做最差解
            is_jump_domain_r = 1;
        }
        if (new_location_l < DOMAIN_LOWER) {
            is_jump_domain_l = 1;
        }
        if (new_location_r <= DOMAIN_UPPER && new_location_r >= DOMAIN_LOWER) {
            c = 1;
            fitness_r = cal_fitness(new_location_r, "減速右看");
        }
        if (new_location_l <= DOMAIN_UPPER && new_location_l >= DOMAIN_LOWER) {
            c = -1;
            fitness_l = cal_fitness(new_location_l, "減速左看");
        }
        if(fitness_r == fitness && fitness_l == fitness) {
            // 左右fitness和中間一樣，位置不一樣
            small_floor = 1;//遇到小平台問題
            return location;
        }
        if (is_jump_domain_l == 1) {
            if (fitness_r < fitness) { // 如果左邊跳出解空間並且右邊fitness小於中間
                location = new_location_r;
                fitness = fitness_r;
                pb_location[dimension] = location;
            }
        }
        else if (is_jump_domain_r == 1) {
            if (fitness_l < fitness) { // 如果右邊跳出解空間並且左邊fitness小於中間
                location = new_location_l;
                pb_location[dimension] = location;
                fitness = fitness_l;
            }
        }
        else if (min(fitness_l, fitness_r) < fitness) {
            if (fitness_l < fitness_r) {
                location = new_location_l;
                pb_location[dimension] = location;
                fitness = fitness_l;
            }
            else {
                location = new_location_r;
                fitness = fitness_r;
                pb_location[dimension] = location;
            }
        }
    }
    else {
        return location;
    }
    return speed_down(location);
}

void epin_write(string address, string Info, float best_location, float now_location) {
    if (JA_count == 1) {
        fstream f(address, ios::out);
        if (f.bad())
        {
            cout << "打開文檔錯誤" << endl;
            return;
        }
        f << "Dimension : 1\nFormula : 10*2+sum(Xi^2-10*cos(2*Pi*Xi))\nRange : " << DOMAIN_LOWER << " ~ " << DOMAIN_UPPER << "\nPosition :" << endl;
        f << setprecision(20) << "*" << JA_count << "," << Info << " " << best_location << ": " << now_location << ",10,0,3" << endl;
        f.close();
    }
    else {
        fstream f(address, ios::app);
        if (f.bad())
        {
            cout << "打開文檔錯誤" << endl;
            return;
        }
        f  << setprecision(20) << "*" << JA_count << "," << Info << " " << best_location << ": " << now_location << ",10,0,3" << endl;
        f.close();
    }

}

double cal_fitness(float x, string Info) {
    double r = 0.0;
    location[dimension] = x;
    for (int i = 0; i < DIMENSION; i++) {
        r += ((pow((double)location[i], 2.0) - 10.0 * cos(2.0 * (double)PI * (double)location[i])));

    }
    r += (10.0 * (double)DIMENSION);
    // r += ((10 - 10 * cos(2.0 * (double)PI * location[dimension])) + pow(location[dimension],2.0));

    // double r = pow(x,2);
    // double r = fabs(x);
    // double r = pow(x,2) - 10 * cos(2.0 * (double)PI * x);
    JA_count++;
    // 計算局部最佳解的次數和狩獵次數
    if (hunting_count == -1) pb_fitness = tmp_fitness = r;

    if (Info != "減速左看" && Info != "初始向左看") pb_fitness = tmp_fitness;

    // cout<< fixed << setprecision(20) << x << endl;
    // cout << fixed << setprecision(20) << c*speed<<endl;
    // cout << fixed << setprecision(20) << r << endl;
    //if (JA_count > 250 || JA_count < 195) {
         // cout<< "計算次數:" << JA_count << "狀態：" << Info << ",局部最好位置:" << setprecision(20) <<pb_location[dimension]<<" 當前位置:" << setprecision(20) << x << " 適應值:" <<setprecision(20)<< r << "局部最佳適應值:" << pb_fitness << ",全域最好適應值:" << setprecision(20) << g_fitness <<" 初始步伐："<<f_speed<<" 當前步伐:"<< speed * c << endl;
    //}
    //else {
    /*
    cout << setprecision(20) << JA_count  << "," << dimension + 1 <<"," << "" << ",";
    for (int i = 0; i < DIMENSION; i++) {
        cout << setprecision(20) << location[i] << ",";
    }
    cout << setprecision(20) << r << "," << speed << "," << speed_c << ",";
    for (int i = 0; i < DIMENSION; i++) {
        cout << setprecision(20) << pb_location[i] << ",";
    }
    cout << pb_fitness << "," << c << ",";
    for (int i = 0; i < DIMENSION; i++) {
        cout << setprecision(20) << gb_location[i] << ",";
    }
    cout << setprecision(20) << float(gb_fitness) << endl;
    */
    //}
    // 寫入excel
    // if (JA_count ==  1) { 
    //     fstream oFile("JA.csv",ios::out);x`
    //     oFile<<"計算次數" << "," << "狀態" << ","<< "位置(1維)"<<","<<"fitness" << "," << "目前步伐" << "," << "速度" << "," << "目前最佳位置(1維)" << "," << "最佳fitness" << "," << "狩獵方向" << endl;
    //     oFile<< JA_count <<","<< " " << "," << setprecision(20) << x <<","<< setprecision(20) << r << "," << setprecision(20) << speed << "," <<" " << "," << setprecision(20) << pb_location[0] << "," << setprecision(20) << pb_fitness << "," << " " << endl;
    //     oFile.close();
    // } else {
    //     fstream oFile("JA.csv",ios::app);
    //     oFile<< JA_count <<","<< " " << "," << setprecision(20) << x <<","<< setprecision(20) << r << "," << setprecision(20) << speed << "," <<" " << "," << setprecision(20) << pb_location[0] << "," << setprecision(20) << pb_fitness << "," << " " << endl;
    //     oFile.close();
    // }
    if (gb_fitness <= r) {
        location[dimension] = gb_location[dimension];
    } // 如果找到位置的fitness不如全域最佳fitness,就將全域最佳位置賦予location
    if (r < tmp_fitness) {
        tmp_fitness = r;
        tmp_hunting_count = JA_count;
    }


    if (JA_count == 1) g_fitness = r;
    else {
        if (r < g_fitness) {
            g_fitness = r;
            update_gb_fitness_count = JA_count;
        }
    }

    // if (JA_count == 1) tmp_fitness = r;
    // else {
    //     if (r < tmp_fitness) tmp_fitness = r;
    // }
    // if (Info != "減速右看" && Info != "初始向右看") g_fitness = tmp_fitness;

    ostringstream oss;
    ostringstream oss2;
    oss << "第" << dimension+1 << "維," << Info << ",當前位置為" << setprecision(26) << x << ",當前步伐為" << speed << ",初始步伐" << f_speed << ",當前fitness為" << r << ",(float)當前fitness為" << (float)r << ",局部最佳fitness為" << pb_fitness << ",全域最佳fitness為" << gb_fitness << ",區域最佳位置" << pb_location[dimension] << ",最佳坑底位置" << gb_location[dimension];
    oss2 << "第" << run_time + 1 << "次實驗JA.epin";
    if (epin)
        epin_write(oss2.str(), oss.str(), g_fitness, x);
    return r;
}
void srand(int seed) {
    state = seed;
}

int rand() {
    state = (state * 214013 + 2531011) & 0xFFFFFFFF;
    return (state >> 16) & 0x7FFF;
}
