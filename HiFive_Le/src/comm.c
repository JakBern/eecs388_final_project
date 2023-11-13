#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "eecs388_lib.h"

void steering(int gpio, int pos)
{
    // Task-3: 
    // Your code goes here (Use Lab 05 for reference)
    // Check the project document to understand the task
	int upulse = SERVO_PULSE_MIN + pos*((SERVO_PULSE_MAX - SERVO_PULSE_MIN)/180);
	gpio_write(gpio, ON);
	delay_usec(upulse);
	gpio_write(gpio, OFF);
	delay_usec(SERVO_PERIOD - upulse);
}

int read_from_pi(int devid)
{
    // Task-2: 
    //after performing Task-2 at dnn.py code, modify this part to read angle values from Raspberry Pi
    // You code goes here (Use Lab 09 for reference)
    if (ser_isready(1)) {
        char x[50]; // arbitrary number of characters
        ser_readline(1, 50, x); // read the deg value in bytes form from the pi
        int deg; 
        deg = x;
        return deg;
    }
}

void auto_brake(int devid)
{
    // Task-1: 
    // Your code here (Use Lab 02 - Lab 04 for reference)
    // You must use the directions given in the project document to recieve full credit
    // read data from the TFminisensor
    //
    if ('Y' == ser_read() && 'Y' == ser_read()) {
        uint16_t sixteens = 0; 
        uint16_t ones = 0; 
        ones = ser_read(); //0bXXXX
        sixteens = ser_read(); //0bYYYY 
        dist = (sixteens << 8) | ones; // 0bYYYYXXXX
        
    if (dist > 200) { // Safe
        gpio_write(GREEN_LED, ON);
        gpio_write(RED_LED, OFF);
     } else if (dist <= 200 && dist >= 100) { // Close
        gpio_write(GREEN_LED, ON);
        gpio_write(RED_LED, ON);
     } else if (dist < 100) { // Very vlose
        gpio_write(GREEN_LED, OFF);
        gpio_write(RED_LED, ON);
     } else if (dist < 60) { // Too close, must stop
        gpio_write(GREEN_LED, OFF);
        gpio_write(RED_LED, ON);
        delay(500); //use cycles instead so the rest of the program isn't delayed
        gpio_write(RED_LED, OFF); 
        delay(300); 
     }
}

int main()
{
    // initialize UART channels
    ser_setup(0); // uart0
    ser_setup(1); // uart1
    int pi_to_hifive = 1; //The connection with Pi uses uart 1
    int lidar_to_hifive = 0; //the lidar uses uart 0
    
    printf("\nUsing UART %d for Pi -> HiFive", pi_to_hifive);
    printf("\nUsing UART %d for Lidar -> HiFive", lidar_to_hifive);
    
    //Initializing PINs
    gpio_mode(PIN_19, OUTPUT);
    gpio_mode(RED_LED, OUTPUT);
    gpio_mode(BLUE_LED, OUTPUT);
    gpio_mode(GREEN_LED, OUTPUT);

    printf("Setup completed.\n");
    printf("Begin the main loop.\n");

    while (1) {

        auto_brake(lidar_to_hifive); // measuring distance using lidar and braking
        int angle = read_from_pi(pi_to_hifive); //getting turn direction from pi
        printf("\nangle=%d", angle) 
        int gpio = PIN_19; 
        for (int i = 0; i < 10; i++){
            // Here, we set the angle to 180 if the prediction from the DNN is a positive angle
            // and 0 if the prediction is a negative angle.
            // This is so that it is easier to see the movement of the servo.
            // You are welcome to pass the angle values directly to the steering function.
            // If the servo function is written correctly, it should still work,
            // only the movements of the servo will be more subtle
            if(angle>0){
                steering(gpio, 180);
            }
            else {
                steering(gpio,0);
            }
        }

    }
    return 0;
}
