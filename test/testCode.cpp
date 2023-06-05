#include <iostream>
#include <vector>
#include <string>
#include <random>

#include <string.h>
#include <unistd.h> 			// POSIX 운영체제 API에 대한 액세스 제공
#include <fcntl.h> 			// 타겟시스템 입출력 장치 관련
#include <sys/types.h> 		// 시스템에서 사용하는 자료형 정보
#include <sys/ioctl.h> 		// 하드웨어의 제어와 상태 정보
#include <sys/stat.h> 		// 파일의 상태에 대한 정보
#include <time.h> 			// 시간 관련
#include <stdlib.h>

#define fnd "/dev/fnd" 		// 7-Segment FND 
#define dot "/dev/dot" 		// Dot Matrix
#define tact "/dev/tactsw" 	// Tact Switch
#define led "/dev/led" 		// LED 
#define dip "/dev/dipsw"	// Dip Switch
#define clcd "/dev/clcd" 	// Character LCD

using namespace std;

bool game_care();
void trainingResult(int successRate, vector<int> trainings);
void train(int& successRate, vector<int>& trainings);
bool check_gameover_1();

void print_dot_mtx_gameover();
int printFnd(int input, unsigned int sleepSec);
int getDipSw(unsigned char& input);
int clearClcd();
int printClcd(string str);
int getTactSw(int& input);
int drawDotMTX(unsigned char& input, unsigned int sleepSec);
int drawDotMTX(vector<unsigned char>& input, unsigned int sleepSec);

int clcds;
int dipSw;
int fnds;
int tactSw;
int dotMtx;
// unsigned char egg[8] = {0x7E, 0x60, 0x60, 0x7C, 0x60, 0x60, 0x7E, 0x00};
// unsigned char immature[8] = {0x3C, 0x18, 0x18, 0x18, 0x18, 0x18, 0x3C, 0x00};
unsigned char smile[8] = {0x00, 0x00, 0x42, 0xA5, 0x00, 0x00, 0x00, 0x00};
unsigned char TT[8] = {0x00, 0x00, 0xE7, 0x42, 0x42, 0x42, 0x00, 0x00};
unsigned char fnd_number[] = {~0x3f, ~0x06, ~0x5b, ~0x4f, ~0x66, ~0x6d, ~0x7d, ~0x07, ~0x7f, ~0x67, ~0x00};
unsigned char c[9][8] = { 
    {0x00, 0x3c, 0x7e, 0x5a, 0x66, 0x7e, 0x66, 0x42},   //유년기
    {0x00, 0x84, 0x58, 0x20, 0x6E, 0x1F, 0x29, 0x2A},   //사슴, run, fly
    {0x00, 0x00, 0x8E, 0x5F, 0x00, 0x29, 0x00, 0x00},   //거북이, run, swim
    {0x0E, 0x11, 0x20, 0x36, 0x70, 0x78, 0x68, 0x08},   //독수리,   fly, run
    {0x00, 0x00, 0x67, 0x18, 0x24, 0x02, 0x00, 0x00},   //잠자리, fly  swim
    {0x00, 0x00, 0x08, 0x3D, 0x6E, 0x11, 0x00, 0x00},   //돌고래, swim, run
    {0x1C, 0x22, 0x41, 0x7F, 0x2A, 0x2A, 0x2A, 0x2A},   //해파리, swim, fly
    {0x81, 0x42, 0x24, 0x18, 0x18, 0x24, 0x42, 0x81},   //게임오버 표시 X
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}    //초기화
};

struct Creature{
    int status;
    string name;
    // unsigned char face[8];
    vector<unsigned char> face;
    vector<int> state;

    void init(int current){
        status = current;
        state.assign(4,0);

        if (status == 0){
            name = "egg";
            // memcpy(&face, &egg, 8);
            face = {0x7E, 0x60, 0x60, 0x7C, 0x60, 0x60, 0x7E, 0x00};
        }
        else{
            name = "immature";
            // memcpy(&face, &immature, 8);
            face = {0x00, 0x3c, 0x7e, 0x5a, 0x66, 0x7e, 0x66, 0x42};
        }
    }
};

Creature creature;

int main() {
    
    printClcd("Press any key to start Game");
    while (true){
        int tactSwInput = 0;
        getTactSw(tactSwInput);
        if (tactSwInput == 0) continue;
        else{
            cout << "Current tact switch input : " << tactSwInput << endl;
            break;
        }
    }
    clearClcd();

    // Game start
    bool gameOver = false;
    for (int status=0; status < 2; status++){
        creature.init(status);
        drawDotMTX(creature.face, 2000000);
        
        int day = 0;
        unsigned char dipSwInput;
        getDipSw(dipSwInput);
        if (dipSwInput != 0){
            printClcd("Please init  dip switch ");
            while(dipSwInput != 0){
                getDipSw(dipSwInput);
            }
            clearClcd();
        }

        while (dipSwInput < 255){ // repeat 8
            unsigned char pre_dipSwInput = dipSwInput;
            int successRate = 100;
            vector<int> trainings;
            if (game_care()) successRate += 10;

            while (pre_dipSwInput == dipSwInput){
                train(successRate, trainings);
                getDipSw(dipSwInput);
            }

            trainingResult(successRate, trainings); // 2nd operate function();
        }

        // if creature.state > threshold 
        // and next status
        gameOver = check_gameover_1();
        if (gameOver) break;
    }

    if (!gameOver){
        // 3rd operate function();
    }

    printClcd("Press any key to terminate");
    while (true){
        int tactSwInput = 0;
        getTactSw(tactSwInput);
        if (tactSwInput == 0) continue;
        else{
            cout << "Current tact switch input : " << tactSwInput << endl;
            break;
        }
    }
    clearClcd();
 

    return 0;
}


/* Operate functions*/
// int check_gameover_1(unsigned int score_run,unsigned int score_fly,unsigned int score_swim)
bool check_gameover_1()
{
    int threshold_score_run = 3;  //달리기 점수 임계값
    int threshold_score_fly = 3;  //비행 점수 임계값
    int threshold_score_swim = 3; //수영 점수 임계값
    int score_run = creature.state[1];
    int score_fly = creature.state[2];
    int score_swim = creature.state[3];

    if( (score_run < threshold_score_run) ||(score_fly<threshold_score_fly) || (score_swim < threshold_score_swim) )
    {
        //game_over
        print_dot_mtx_gameover();
        return true;
    }
    else
    {
        return false;
    }
}

void print_dot_mtx_gameover() // 게임 오버 표시 도트 매트릭스 표현  X 표시 점멸 2번 후 초기화
{
    printClcd("   Game Over    ");

    dotMtx = open(dot, O_RDWR);
    write(dotMtx, &c[8], sizeof(c[8])); 
    usleep(500000); 
    write(dotMtx, &c[9], sizeof(c[8])); 
    usleep(500000); 
    write(dotMtx, &c[8], sizeof(c[8])); 
    usleep(500000); 
    write(dotMtx, &c[9], sizeof(c[8])); 
    usleep(500000);
    close(dotMtx);
    return;
}

bool game_care() {

	string care_arr[4] = {"My pet looks        hungry", 
                            "My pet looks        dirty", 
                            "My pet looks        bored", 
                            "My pet looks        sleepy"};// { "1.Feed", "2.Wash", "3.Play", "4.Sleep" };
	int random_index = rand() % 4;
	string care_str = care_arr[random_index];
	
	printClcd(care_str);

	int tactInput = 0; //tactswinput

	while (tactInput == 0) {
		getTactSw(tactInput);
		//cout << tactInput << endl;		
		// if (tactInput != 0) break;
	}

	if (random_index + 1 == tactInput) {
		cout << "care success" << endl;
		return true;
	}
	else {
		cout << "failure to care" << endl;
		return false;
	}

}

// 현재 예약된 훈련 목록을 clcd로 보여줌, dotmtx로 현재 얼굴 보여줌, FND로 현재 성공확률 보여줌
void train(int& successRate, vector<int>& trainings){
    // if ( 돌봐주기 성공 ) { successRate += 10; }
    int training = 0;
    string trainClcd = "Today's traingin : ";
    printClcd(trainClcd);
    
    printFnd(successRate, 1000000); // 0.25s
    while (training == 0){
        getTactSw(training);
        usleep(1000);
        drawDotMTX(creature.face, 250000); // 1s
    }

    if ( training > 4 ){
        //잘못된 입력
    }
    else{
        trainings.push_back(training);
        // trainClcd += to_string(training);
        trainClcd = trainClcd.append(to_string(training));
        successRate -= 10;
        if (training == 4){
            successRate += 3; // 사냥은 7 감소
        }
    } 
    return;
}

void trainingResult(int successRate, vector<int> trainings){
    srand((unsigned int)time(NULL));
    int dice = random() % 100;
    vector<int> table(4, 0);

    // fail
    if ( successRate <= dice ){
        printClcd("Training failed");
        drawDotMTX(*TT, 1500000);
        cout << "Training failed: " << dice << endl;
        return;
    }

    // 성공
    int volume = 10;
    int pre_training = 0;
    for (int i=0; i < trainings.size(); i++) {
        int training = trainings[i];

        if (training > 4) {
            cout << training << ": invalid input" << endl;
            break;
        }
        else if (training == 4){
            // 사냥인 경우, 이전 훈련을 2번한 것과 같은 효과지만 뒤의 훈련의 효율이 떨어짐 (80%)
            table[pre_training] += volume * 2;
            volume = volume * 8 / 10;
            // cout << "current volume : " << volume << endl;
        }
        else {
            table[training] += volume;
            pre_training = training;
        }
    }
    
    for (int i=1; i<4; i++){
        creature.state[i] += table[i];
    }

    printClcd("Training Success!");
    drawDotMTX(*smile, 700000);
    printClcd("   running :        " + to_string(creature.state[1]) + " ( + " + to_string(table[1]) + ")" ); // running : 32(+3)
    drawDotMTX(*smile, 700000);
    printClcd("   flying :         " + to_string(creature.state[2]) + " ( + " + to_string(table[2]) + ")" ); 
    drawDotMTX(*smile, 700000);
    printClcd("   running :        " + to_string(creature.state[3]) + " ( + " + to_string(table[3]) + ")" ); 
    drawDotMTX(*smile, 700000);
    clearClcd();
    // cout << "running : " << table[1] << endl;
    // cout << "flying : " << table[2] << endl;
    // cout << "swimming : " << table[3] << endl;
    return;
}

/* IO functions */
int printFnd(int input, unsigned int sleepSec){

    vector<unsigned char> data(4, fnd_number[0]);
    data[0] = fnd_number[ input / 1000  % 10 ];
    data[1] = fnd_number[ input / 100   % 10 ];
    data[2] = fnd_number[ input / 10    % 10 ];
    data[3] = fnd_number[ input / 1     % 10 ];

    fnds=open(fnd, O_RDWR);
    if (fnds < 0) {
        cout << "can't find Dev dirver" << endl;
        return -1; 
    }
    write(fnds, data.data(), 4);
    usleep(sleepSec);
    close(fnds);
    return 0;
}


int getDipSw(unsigned char& input){
    dipSw = open(dip, O_RDWR);
    if (dipSw < 0) {
        cout << "can't find Dev driver" << endl;
        return -1;
    }
    read(dipSw, &input, sizeof(input));
    close(dipSw);
    return 0;
}

int clearClcd(){
    clcds = open(clcd, O_RDWR);
    unsigned char clear[32];
    memset(clear, 0, 32);
    if (clcds < 0){
        cout << "can't find Dev dirver" << endl;
        return -1; 
    }

    write(clcds, &clear, sizeof(clear));
    close(clcds);
    return 0;
}

int printClcd(string str){
    clcds = open(clcd, O_RDWR);

    if (clcds < 0){
        cout << "can't find Dev dirver" << endl;
        return -1; 
    }

    if (write(clcds, str.c_str(), 32) == -1){
        cout << "file write error" << endl;
        return -1;
    } 
    close(clcds);
    return 0;
}

int getTactSw(int& input){
    tactSw = open(tact, O_RDWR);
    if (tactSw < 0) {
        cout << "can't find Dev driver" << endl;
        return -1;
    }
    read(tactSw, &input, sizeof(input));
    close(tactSw);
    return 0;
}

int drawDotMTX(unsigned char& input, unsigned int sleepSec){
    dotMtx = open(dot, O_RDWR);
    if (dotMtx < 0) {
        cout << "can't find Dev dirver" << endl;
        return -1; 
    }
    write(dotMtx, &input, 8);
    usleep(sleepSec);
    close(dotMtx);
    return 0;
}

int drawDotMTX(vector<unsigned char>& input, unsigned int sleepSec){
    dotMtx = open(dot, O_RDWR);
    if (dotMtx < 0) {
        cout << "can't find Dev dirver" << endl;
        return -1; 
    }
    write(dotMtx, input.data(), 8);
    usleep(sleepSec);
    close(dotMtx);
    return 0;
}
