#include <iostream>
#include <string.h>

#include<unistd.h> 			// POSIX 운영체제 API에 대한 액세스 제공
#include<fcntl.h> 			// 타겟시스템 입출력 장치 관련
#include<sys/types.h> 		// 시스템에서 사용하는 자료형 정보
// #include<sys/ioctl.h> 		// 하드웨어의 제어와 상태 정보
#include<sys/stat.h> 		// 파일의 상태에 대한 정보
#include <time.h> 			// 시간 관련

#define fnd "/dev/fnd" 		// 7-Segment FND 
#define dot "/dev/dot" 		// Dot Matrix
#define tact "/dev/tactsw" 	// Tact Switch
#define led "/dev/led" 		// LED 
#define dip "/dev/dipsw"	// Dip Switch
#define clcd "/dev/clcd" 	// Character LCD

using namespace std;

int clearClcd();
int printClcd(string str);
int getTactSw(int& input);
int drawDotMTX(unsigned char& input, unsigned int sleepSec);

int clcds;
int dipSw;
int fnds;
int tactSw;
int dotMtx;
int a = 0b0101010;
unsigned char figure[] = {0x7E, 0x60, 0x60, 0x7C, 0x60, 0x60, 0x7E, 0xff}; //E
unsigned char figure2[] = {0x7E, 0x60, 0x60, 0x7C, 0x60, 0x60, 0xff, 0xff};
unsigned char figure3[] = {0x7E, 0x60, 0x60, 0x7C, 0x60, 0xff, 0xff, 0xff};

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
    printClcd("Start Program");
    
    int startTime, currentTime;

    cout << "dot2 run" << endl;
    drawDotMTX(*figure, 500000);


    drawDotMTX(*figure3, 500000);
    drawDotMTX(*figure2, 500000);
    cout << "sleep(1)" << endl;

    


    printClcd("Press any key to terminate Program");
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
    cout << clcds << endl;
    return 0;
}

int printClcd(string str){
    clcds = open(clcd, O_RDWR);

    if (clcds < 0){
        cout << "can't find Dev dirver" << endl;
        return -1; 
    }

    if (write(clcds, str.c_str(), 32) == -1){
        cout << "file write error" << endl; // str.size()이 걸로 되는지 모르겠음
        return -1;
    } 
    close(clcds);
    cout << clcds << endl;
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
    cout << &input << endl;
    cout << input << endl;
    usleep(sleepSec);
    close(dotMtx);
    return 0;
}