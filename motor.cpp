#include "motor.h"
#include <wiringPi.h>
#include <wiringPiI2C.h>

//#include <iostream>

#define PCA9685_SUBADR1 0x2
#define PCA9685_SUBADR2 0x3
#define PCA9685_SUBADR3 0x4

#define PCA9685_MODE1 0x0
#define PCA9685_PRESCALE 0xFE

#define LED0_ON_L 0x6
#define LED0_ON_H 0x7
#define LED0_OFF_L 0x8
#define LED0_OFF_H 0x9

#define ALLLED_ON_L 0xFA
#define ALLLED_ON_H 0xFB
#define ALLLED_OFF_L 0xFC
#define ALLLED_OFF_H 0xFD

//#define unch unsigned char
/*
class pca
{
public:
	//int init();
	void setmk(int num, int mk);
	pca(unch inputAddr, int enPin);
	void setDuty(int num, double duty);

private:
	unch addr;
	int fd, en;	
	int init();
	//pca(unch inputAddr, int enPin);
};
*/
/*
pca::pca(const pca &inputPca) : 
    addr(inputPca.addr), en(inputPca.en), fd(init()) {}

pca::pca(unsigned char inputAddr, int enPin) : //setup pca with input an i2c address and the en-pin
    addr(inputAddr), en(enPin), fd(init()) {}

pca::pca(const unsigned char &inputAddr, const int &enPin) : 
    addr(inputAddr), en(enPin), fd(init()) {}
*/
void pca::setmk(int num, int mk) //设置指定通道的脉宽。fd是在pca9685_init时获得的设备描述符，num是通道号（从0开始），mk是脉宽单位是us。周期已经固定为20ms了
{
//	using namespace std;
	unsigned int ON, OFF;
	ON = 0;															 //每次周期一开始就输出高电平
	OFF = (unsigned int)((((double)mk) / 20000 * 4096) * 1.0067114); //最后的1.0067114是校准用的

	wiringPiI2CWriteReg16(fd, LED0_ON_L + 4 * num, ON);
	wiringPiI2CWriteReg16(fd, LED0_OFF_L + 4 * num, OFF);

//	cout << "pca." << num << " set to pulse width of " << mk << "us" << endl;
}

void pca::setDuty(int num, double duty) //set the channel to a duty(less than 1 and more than 0)
{
//	using namespace std;
	setmk(num, int(duty * 20000.0));
//	cout << "pca." << num << " set to duty of " << duty << endl;
}

int pca::init() // addr是7位的i2c从机地址，返回的是linux标准的设备描述符，调用它的地方视作pca9685的设备描述符
				//因为可以多个pca9685级联，通过设备描述符区别它们
				//此驱动仅作为驱动舵机使用，周期固定死位20ms，不允许外部设置
{
	int pca9685;
	pca9685 = wiringPiI2CSetup(addr);

	{					  //初始化pca9685芯片
		double T = 20000; //周期，单位是us
		unsigned char prescale;
		double osc_clock = 25000000;
		unsigned char oldmode, newmode;
		T /= 0.93;	//不知道为什么，会有所偏差，这里校准一下就ok了，从网上找的arduino代码也进行了校准。
		T /= 1000000; //把T转换成秒
		prescale = (unsigned char)(osc_clock / 4096 * T - 1);
		oldmode = wiringPiI2CReadReg8(pca9685, PCA9685_MODE1);
		newmode = (oldmode & 0x7f) | 0x10; //准备进入sleep，设置时钟前必须先进入sleep模式
		wiringPiI2CWriteReg8(pca9685, PCA9685_MODE1, newmode);
		wiringPiI2CWriteReg8(pca9685, PCA9685_PRESCALE, prescale);
		oldmode &= 0xef; //清除sleep位
		wiringPiI2CWriteReg8(pca9685, PCA9685_MODE1, oldmode);
		delay(0.005);
		wiringPiI2CWriteReg8(pca9685, PCA9685_MODE1, oldmode | 0xa1);
	}
	pinMode(en, OUTPUT);
	return pca9685;
}

void pca::enable() //enable the en-pin by setting it to low
{
	digitalWrite(en, LOW);
}

motor::motor(int pin1, int pin2, const pca &inputPca, int inputChannel):
	pin_1(pin1), pin_2(pin2), channel(inputChannel) 
{
	motorPca.addr=inputPca.addr; 
	motorPca.en=inputPca.en; 
	motorPca.fd=inputPca.fd;
	pinMode(pin_1, OUTPUT);
	pinMode(pin_2, OUTPUT);
	digitalWrite(pin_1, LOW);
	digitalWrite(pin_2, LOW);
}

motor::motor(int &pin1, int &pin2, const pca &inputPca, int &inputChannel):
	pin_1(pin1), pin_2(pin2), channel(inputChannel) 
{
	motorPca.addr=inputPca.addr; 
	motorPca.en=inputPca.en; 
	motorPca.fd=inputPca.fd;
	pinMode(pin_1, OUTPUT);
	pinMode(pin_2, OUTPUT);
	digitalWrite(pin_1, LOW);
	digitalWrite(pin_2, LOW);
}

void motor::setup(int pin1, int pin2, const pca &inputPca, int inputChannel)
{
	pin_1=pin1;
	pin_2=pin2;
	channel=inputChannel; 

	motorPca.addr=inputPca.addr; 
	motorPca.en=inputPca.en; 
	motorPca.fd=inputPca.fd;
	pinMode(pin_1, OUTPUT);
	pinMode(pin_2, OUTPUT);
	digitalWrite(pin_1, LOW);
	digitalWrite(pin_2, LOW);
}

void motor::setSpeed(double duty)
{
//	using namespace std;
	bool flag = (duty >= 0);
	digitalWrite(pin_1, flag);
//		cout << "pin " << pin_1 << " set to " << flag << endl;
	digitalWrite(pin_2, !flag);
//		cout << "pin " << pin_2 << " set to " << !flag << endl;
	duty *= double(int(flag) * 2 - 1);
//		cout << "abs(duty) = " << duty << endl;
	motorPca.setDuty(channel, duty);
//		cout << "motor at pca." << channel 
//			<< " set to duty of " << duty 
//			<< endl;
}

void motor::stop()
{
	for(int i = 0; i < 3; ++i)
	{
		setSpeed(-0.9);
		delay(10);
		setSpeed(0.9);
		delay(10);
	}
	setSpeed(0);
}

servo::servo(const pca &inputPca, int inputChannel)
{
	channel = inputChannel;
	//servoPca(inputPca);
	servoPca.addr = inputPca.addr;
	servoPca.en = inputPca.en;
	servoPca.fd = inputPca.fd;
	servoPca.setmk(channel, 0);
}

void servo::write(double angle)
{
	int zeroWidth = 550, maxWidth = 2100;
	servoPca.setmk(channel, zeroWidth + int(double(maxWidth - zeroWidth) * angle / 180.0));
}
