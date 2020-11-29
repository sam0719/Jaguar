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
#define DIMENSION 1
#define RUNTIME 1

static int state;
void srand(int seed);
int rand();
int run_time, update_floor_count; // 實驗次數
int dimension;
void jump_speed_up();
double cal_fitness(float x, string Info);
float hunting(float location);
float speed_up(float location);
float speed_down(float location);
float location_floor[10000]; // 記錄走過的坑底,可能要用二維數組 [維度][坑數]
void epin_write(string address, string Info, float best_location, float now_location);
int JA_count = 0, gb_count; // 計算次數,最佳解次數
float location[DIMENSION], f_location, pb_location[DIMENSION]; // 位置
int tmp_hunting_count, hunting_count, update_gb_fitness_count, floor_count = 0; // 狩猎次数,,更新最佳解計算次數,坑數
double pb_fitness, pb_fitness_r, pb_fitness_l;
float jump_r_location[DIMENSION], jump_l_location[DIMENSION]; // 兩倍跳出後的位置
float gb_location[DIMENSION]; // 坑底最佳位置和fitness
double gb_fitness;
float gb_fitness_float;
double tmp_fitness;
float speed, f_speed, speed_c; // 步伐
double fitness, new_fitness, g_fitness;
int c, jump_c; // 控制右左跳
float DOMAIN_UPPER = 32.768;
float DOMAIN_LOWER = -32.768;

int main(int argc, char* argv[])
{
    clock_t start, end;
    start = clock();
    srand(114);
    for (int i = 0; i < 26; i++) {
        // float test = rand();
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
        floor_count = 0;
        tmp_hunting_count = 0;
        hunting_count = 0;
        update_gb_fitness_count = 0;
        c = 1;
        jump_c = 1;
        for (int i = 0; i < DIMENSION; i++) {
            location[i] = float(rand() / RAND_MAX);
            use_rand_count++;
            location[i] = pb_location[i] = gb_location[i] = location[i] * (DOMAIN_UPPER - DOMAIN_LOWER) + DOMAIN_LOWER;
        }
        for (dimension = 0; dimension < DIMENSION; dimension++) {
            cout << "進入第" << dimension + 1 << "維度" << endl;
            f_location = location[dimension];
            pb_location[dimension] = location[dimension];
            // cout<< setprecision(20) << location << endl; // 位置
            bool is_jump_domain_r, is_jump_domain_l, is_find_new_floor_r, is_find_new_floor_l, is_stop_r, is_stop_l, is_worst_fitness;
            is_jump_domain_r = is_jump_domain_l = is_find_new_floor_r = is_find_new_floor_l = is_stop_r = is_stop_l = is_worst_fitness = false;
            location[dimension] = hunting(location[dimension]);
            cout << "狩獵總計算次數:" << JA_count << ",找到最佳解的次數:" << tmp_hunting_count << endl;
            gb_location[dimension] = location[dimension];
            gb_fitness = fitness;
            float r = fabs(f_location - location[dimension]); // 半徑

            jump_r_location[dimension] = gb_location[dimension] + r * 2;
            float jump_r_location_first = jump_r_location[dimension];
            if (jump_r_location[dimension] > DOMAIN_UPPER) {
                jump_r_location[dimension] = DOMAIN_UPPER;
                is_jump_domain_r = 1; // 右跳是否跳出定義域
            }
            else
                is_jump_domain_r = 0;
            jump_l_location[dimension] = gb_location[dimension] - r * 2;
            float jump_l_location_first = jump_l_location[dimension];
            if (jump_l_location[dimension] < DOMAIN_LOWER) {
                jump_l_location[dimension] = DOMAIN_LOWER;
                is_jump_domain_l = 1; // 左跳是否跳出定義域
            }
            else
                is_jump_domain_l = 0;
            cout << "向右跳脱" << endl;
            pb_location[dimension] = jump_r_location[dimension];
            jump_r_location[dimension] = hunting(jump_r_location[dimension]);
            pb_fitness_r = fitness;
            if (gb_location[dimension] != jump_r_location[dimension])
                is_find_new_floor_r = 1;

            cout << "向左跳脫" << endl;
            pb_location[dimension] = jump_l_location[dimension];
            jump_l_location[dimension] = hunting(jump_l_location[dimension]);
            pb_fitness_l = fitness;
            if (gb_location[dimension] != jump_l_location[dimension])
                is_find_new_floor_l = 1;
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
                cout << "三個坑底比較" << endl;
                r = max(r_r, r_l);
                // 要改的地方 左右半徑選大的那個 如果有一邊不做 不需要設置半徑 設為-1
                if (is_jump_domain_r == 0 && is_find_new_floor_r == 0) {
                    jump_r_location_first = gb_location[dimension] + r * 2.0;
                    if (jump_r_location_first > DOMAIN_UPPER) {
                        jump_r_location_first = DOMAIN_UPPER;
                        is_jump_domain_r = 1; // 右跳是否跳出定義域
                    }
                    pb_location[dimension] = jump_r_location_first;
                    jump_r_location[dimension] = hunting(jump_r_location_first);
                    pb_fitness_r = fitness;
                    if (gb_location[dimension] != jump_r_location[dimension])
                        is_find_new_floor_r = 1;
                }
                if (is_jump_domain_l == 0 && is_find_new_floor_l == 0) {
                    jump_l_location_first = gb_location[dimension] - r * 2.0; // 
                    if (jump_l_location_first < DOMAIN_LOWER) {
                        jump_l_location_first = DOMAIN_LOWER;
                        is_jump_domain_l = 1; // 左跳是否跳出定義域
                    }
                    pb_location[dimension] = jump_l_location_first;
                    jump_l_location[dimension] = hunting(jump_l_location_first);
                    pb_fitness_l = fitness;
                    if (gb_location[dimension] != jump_l_location[dimension])
                        is_find_new_floor_l = 1;
                }
                r_r = fabs(jump_r_location_first - gb_location[dimension]);
                r_l = fabs(jump_l_location_first - gb_location[dimension]);
                if (is_find_new_floor_l == 1 || is_jump_domain_l == 1)
                    r_l = -1;
                if (is_find_new_floor_r == 1 || is_jump_domain_r == 1)
                    r_r = -1;

            }
            if ((float)gb_fitness <= (float)pb_fitness_r && (float)gb_fitness <= (float)pb_fitness_l) {
                cout << "左右兩邊的fitness都大於等於中間fitness" << endl;
                if ((is_jump_domain_l == 1 && is_jump_domain_r == 1) && (pb_fitness_r == gb_fitness && pb_fitness_l == gb_count)) {
                    cout << "左右都跳出定義域，並且左右fitness與中間一樣,結束跳脫" << endl;
                    continue;
                }
                else if (is_find_new_floor_l == true || is_find_new_floor_r == true) {
                    cout << "左或右其中一邊找到新的坑底,進入跳脫減速階段" << endl;
                    is_worst_fitness = 1;
                }
            }
            else if ((float)pb_fitness_r == (float)pb_fitness_l && (float)pb_fitness_r < (float)gb_fitness) {
                cout << "左右fitness相同並且比中間的好,以隨機或固定的方式往其中一邊跳脫加速" << endl;
                r = fabs(jump_r_location[dimension] - gb_location[dimension]);
                is_stop_l = 1;
            }
            else if ((float)pb_fitness_r < min((float)pb_fitness_l, (float)gb_fitness) || (float)pb_fitness_l < min((float)pb_fitness_r, (float)gb_fitness)) {
                cout << "左或右有一邊比另外兩個好，往好的那一邊跳脫加速" << endl;
                if ((float)pb_fitness_l < (float)pb_fitness_r) {
                    r = fabs(jump_l_location[dimension] - gb_location[dimension]);
                    is_stop_r = 1;
                }
                else if ((float)pb_fitness_r < (float)pb_fitness_l) {
                    r = fabs(jump_r_location[dimension] - gb_location[dimension]);
                    is_stop_l = 1;
                }
            }
            is_jump_domain_r = is_jump_domain_l = false;
            cout << "進入加速" << endl;
            while ((is_jump_domain_r == false && is_jump_domain_l == false) && is_worst_fitness == false) {
                if (is_stop_r == 1) {
                    // cout << "往左邊加速" << endl;
                    r *= 2;
                    jump_c = -1;
                    gb_location[dimension] = jump_l_location[dimension];
                    gb_fitness = pb_fitness_l;
                    location[dimension] = jump_l_location[dimension];
                    jump_l_location[dimension] = location[dimension] - r;
                    if (jump_l_location[dimension] < DOMAIN_LOWER) {
                        jump_l_location[dimension] = DOMAIN_LOWER;
                        is_jump_domain_l = 1;
                    }
                    jump_l_location[dimension] = hunting(jump_l_location[dimension]);
                    pb_fitness_l = fitness;
                    if (pb_fitness_l >= gb_fitness) is_worst_fitness = true;
                }
                else if (is_stop_l == 1) {
                    // cout << "往右邊加速，跳脫距離:" << jump_r_location[dimension] - gb_location[dimension] << endl;
                    r *= 2;
                    jump_c = 1;
                    gb_location[dimension] = jump_r_location[dimension];
                    gb_fitness = pb_fitness_r;
                    location[dimension] = jump_r_location[dimension];
                    jump_r_location[dimension] = location[dimension] + r;
                    if (jump_r_location[dimension] > DOMAIN_UPPER) {
                        jump_r_location[dimension] = DOMAIN_UPPER;
                        is_jump_domain_r = 1;
                    }
                    jump_r_location[dimension] = hunting(jump_r_location[dimension]);
                    pb_fitness_r = fitness;
                    if (pb_fitness_r >= gb_fitness) {
                        is_worst_fitness = true; //找到不好的解
                    }
                }
            }
            cout << "中間坑底：" << gb_location[dimension] << ",中間fitness:" << gb_fitness << ",右邊坑底:" << jump_r_location[dimension] << ",右邊fitness:" << pb_fitness_r << "左邊坑底:" << jump_l_location[dimension] << "左邊fitness:" << pb_fitness_l << endl;
            // 最佳坑底同方向跳一次減半距離 另一邊已經看過 不用看
            cout << "進入減速" << endl;
            /*
            r /= 2;
            if (jump_c == 1) {
                jump_r_location[dimension] = gb_location[dimension] + r;
                if (jump_r_location[dimension] < DOMAIN_UPPER)
                    jump_r_location[dimension] = hunting(jump_r_location[dimension]);
                pb_fitness_r = fitness;
                if (pb_fitness_r < gb_fitness) {
                    gb_location[dimension] = jump_r_location[dimension];
                    gb_fitness = pb_fitness_r;
                }
            }
            else
            {
                jump_l_location[dimension] = gb_location[dimension] - r;
                if (jump_l_location[dimension] > DOMAIN_LOWER)
                    jump_l_location[dimension] = hunting(jump_l_location[dimension]);
                pb_fitness_l = fitness;
                if (pb_fitness_l < gb_fitness) {
                    gb_location[dimension] = jump_l_location[dimension];
                    gb_fitness = pb_fitness_l;
                }
            }
            */
            while (1) {
                r /= 2;
                jump_r_location[dimension] = gb_location[dimension] + r;
                if (jump_r_location[dimension] > DOMAIN_UPPER)
                    jump_l_location[dimension] = DOMAIN_UPPER;
                jump_r_location[dimension] = hunting(jump_r_location[dimension]);
                pb_fitness_r = fitness;
                jump_l_location[dimension] = gb_location[dimension] - r;
                if (jump_l_location[dimension] < DOMAIN_LOWER)
                    jump_l_location[dimension] = DOMAIN_LOWER;
                jump_l_location[dimension] = hunting(jump_l_location[dimension]);
                pb_fitness_l = fitness;
                if (pb_fitness_r == pb_fitness_l && pb_fitness_r < gb_fitness) {
                    cout << "隨機左右選一邊當最佳，然後繼續循環" << endl;
                    gb_location[dimension] = jump_r_location[dimension];
                    gb_fitness = pb_fitness_r;
                }
                else if (pb_fitness_r < min(gb_fitness, pb_fitness_l) || pb_fitness_l < min(gb_fitness, pb_fitness_r)) {
                    cout << "左右不相同並且有一邊優於中間" << endl;
                    if (pb_fitness_r < pb_fitness_l) {
                        cout << "右邊比較好" << endl;
                        gb_location[dimension] = jump_r_location[dimension];
                        gb_fitness = pb_fitness_r;
                    }
                    else if (pb_fitness_l < pb_fitness_r) {
                        cout << "左邊比較好" << endl;
                        gb_location[dimension] = jump_l_location[dimension];
                        gb_fitness = pb_fitness_l;
                    }
                }
                else if (jump_r_location[dimension] == jump_l_location[dimension] && jump_l_location[dimension] == pb_location[dimension] && jump_r_location[dimension] == pb_location[dimension]) {
                    cout << "左右和中間的都相同" << endl;
                    break;
                }
                cout << "左右都差於中間,count:" << JA_count << endl;
            }
            location[dimension] = gb_location[dimension];
            cout << "減速結束" << endl;
        }
        cout << "第" << run_time + 1 << "次實驗找到最佳解:" << gb_location[0] << ",找到最佳解次數:" << update_gb_fitness_count << ",坑數:" << floor_count << ",更新坑計算次數:" << update_floor_count << ",rand()使用次數:" << use_rand_count << endl;
    }

    // cout<< speed<<endl; // 輸出減速後的speed
    end = clock();
    cout << "time:" << (double)(end - start) / CLOCKS_PER_SEC << endl;
    return 0;

}


float hunting(float location) {
    hunting_count = -1;
    ostringstream oss;
    oss << "初始撒點";
    f_speed = powf(2.0, int(log2(DOMAIN_UPPER)) - 11);
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
        if (new_location_r == new_location_l) {
            k = 1;
            break;
        }
        // cout<< "右左看" <<endl;
        if ((new_location_r <= DOMAIN_UPPER && new_location_r >= DOMAIN_LOWER)) {
            // cout<< "right:" << new_location_r << endl;
            c = 1;
            fitness_r = cal_fitness(new_location_r, "初始向右看"); // 向右跳
        }
        else
            f = 1;
        if (new_location_l <= DOMAIN_UPPER && new_location_l >= DOMAIN_LOWER) {
            // cout<< "left:" << new_location_l << endl;
            c = -1;
            fitness_l = cal_fitness(new_location_l, "初始向左看"); // 向左跳
        }
        else
            f = 2;
        // cout <<"test:"<< fitness_l << ","<< fitness_r << "," << fitness <<endl;
        if (fitness_l >= fitness && fitness_r >= fitness) { // 如果右左看的fitness都大於等於最好位置
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
        else if (fitness_l <= fitness_r) {
            c = -1;
            location = new_location_l;
            pb_location[dimension] = location;
            fitness = fitness_l;
        }
        else if (fitness_l > fitness_r) {
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
        f_speed /= 2;
        speed = f_speed;
        // cout<< "速度回到1，初始步伐減半" <<endl;
    }
    bool floor_is_hunting = 0;
    for (int i = 0; i < floor_count; i++) {
        if (location == location_floor[i]) { // 坑底位置已經走過
            floor_is_hunting = 1;
            break;
        }
    }
    if (floor_is_hunting == 0) {
        location_floor[floor_count] = location;
        floor_count++;
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
    new_fitness = cal_fitness(new_location, "加速");
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
        if (new_location_l == new_location_r) {
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
        if (min(fitness_l, fitness_r) < fitness) {
            if (is_jump_domain_l == 1 || is_jump_domain_r == 1) {
                if (is_jump_domain_l == 1) { // 如果左邊跳出解空間
                    location = new_location_r;
                    fitness = fitness_r;
                    pb_location[dimension] = location;
                }
                else if (is_jump_domain_r == 1) {
                    location = new_location_l;
                    pb_location[dimension] = location;
                    fitness = fitness_l;
                }
            }
            else if (fitness_l < fitness_r) {
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
        f << "Dimension : 1\nFormula : 20+exp(1)-20*exp((0-0.2*(sum(Xi^2)))/2^0.5)-exp(sum(cos(2*PI*Xi))/2)\nRange : " << DOMAIN_LOWER << " ~ " << DOMAIN_UPPER << "\nPosition :" << endl;
        f << "*" << JA_count << "," << Info << " " << best_location << ": " << now_location << ",10,0,3" << endl;
        f.close();
    }
    else {
        fstream f(address, ios::app);
        if (f.bad())
        {
            cout << "打開文檔錯誤" << endl;
            return;
        }
        f << "*" << JA_count << "," << Info << " " << best_location << ": " << now_location << ",10,0,3" << endl;
        f.close();
    }

}

double cal_fitness(float x, string Info) {
    double r = 0.0,sum1 = 0.0,sum2 = 0;
    location[dimension] = x;
    for (int i = 0; i < DIMENSION; i++) {
        sum1 += pow(location[i], 2.0);
        sum2 += cos(2.0 * (double)PI * location[i]);
        // r += ((10 - 10 * cos(2.0 * (double)PI * location[dimension])) + pow(location[dimension],2.0));
    }
    r = exp(1) + 20 - (20 * exp((0.0 - 0.2) * pow((1 / (double)DIMENSION) * sum1, 0.5))) - exp((1 / (double)DIMENSION) * sum2);

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
    oss << Info << ",當前位置為" << setprecision(20) << x << ",當前步伐為" << speed << ",初始步伐" << f_speed << ",當前fitness為" << r << ",區域最佳位置" << pb_location[0] << ",最佳坑底位置" << gb_location[0];
    oss2 << "第" << run_time + 1 << "次實驗JA.epin";
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
