#include "motor.h"
#include <iostream>
#include <wiringPi.h>

/*************************************************************
 * motor0
 * pin_1 ==> 24 ==> OUT3
 * pin_2 ==> 25 ==> OUT4
 * channel ==> pca.0
 * 
 * motor1
 * pin_1 ==> 22 ==> OUT1
 * pin_2 ==> 23 ==> OUT2
 * channel ==> pca.1
 * 
 * motor2
 * pin_1 ==> 27 ==> OUT3
 * pin_2 ==> 28 ==> OUT4
 * channel ==> pca.2
*************************************************************/

void channelSetDuty(int channel, pca inputPca)
{
    using namespace std;
    double duty;
    cout << "input duty:" << endl;
    cin >> duty;
    inputPca.setDuty(channel, duty);
    cout << "chennel 4 set to "
        << duty << "duty"
        << endl;
}

void channelSetWidth(int channel, pca inputPca)
{
    using namespace std;
    int width;
    cout << "input pulse width:" << endl;
    cin >> width;
    inputPca.setmk(channel, width);
    cout << "chennel 4 set to "
        << width << " us"
        << endl;
}

void turnServo(servo &inputServo)
{
    using namespace std;
    double angle;
    cout << "input angle:" << endl;
    cin >> angle;
    if(angle >= 0.0 && angle <= 180.0)
    {
        inputServo.write(angle);
        cout << "servo at set to " << angle << endl;
    }
    else
    {
        inputServo.write(0);
        cout << "input error" << endl;
    }
}

void runMotor(motor &inputMotor)
{
    using namespace std;
    double speed;
    pca myPca(0x40, 7);

    cout << "input speed(-1.0~1.0):" << endl;
    cin >> speed;
    if(speed == 0)
    {
        inputMotor.stop();
        cout << "motor stopped" << endl;
    }
    else if(speed >= -1.0 && speed <= 1.0)
    {
        inputMotor.setSpeed(speed);
        cout << "motor speed set to " << speed << endl;
    }
    else
    {
        cout << "input error" << endl;
        inputMotor.stop();
        cout << "motor has stopped" << endl;
    }
}

int main()
{
    using namespace std;
    wiringPiSetup();
    pca myPca(0x40, 7);
    servo myServo(myPca, 3);
    
    int motorChannel = 0;
    motor myMotor(24, 25, myPca, motorChannel);
    myPca.enable();
    myPca.setmk(motorChannel, 0);
    cout << "channel " << motorChannel
        << " set to 0" << endl;
    cout << "wiringPi set up successfully" << endl;
    myPca.enable();
    cout << "pca9685 enabled" << endl;
    
    while (1)
    {
        //runMotor(myMotor);
        turnServo(myServo);
    }
}
