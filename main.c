/* Name: main.c
 * Author: <Alex Garcia>
 * Copyright: <insert your copyright message here>
 * License: <insert your license reference here>
 */

#include "teensy_general.h"  // includes the resources included in the teensy_general.h file
#include "t_usb.h" // includes the resources included in the t_usb.h
#include "stdio.h"
#include "stdlib.h"
#include "string.h"


#define PACKETSIZE 8
char buf[PACKETSIZE];
volatile unsigned char pos;
volatile char process;
int prevFreq;

// Initialize variables
int photo;    
int counter;
int frequency;
int flag;
int health;
int healingHigh = 0;



void ADC_setup()
{
    
    //Set pins to output 
    teensy_clockdivide(0);

    // Initialize USB transmission
    // m_usb_init();

    // Set voltage reference
    set(ADMUX,REFS0);

    // Set ADC prescaler to 1/128
    set(ADCSRA,ADPS2);
    set(ADCSRA,ADPS1);
    set(ADCSRA,ADPS0);

    // Set trigger
    set(ADCSRA,ADATE);

    // Enable ADC subsystem
    set(ADCSRA,ADEN);

    // Start conversion
    set(ADCSRA,ADSC);

} 

void ADC_read(char port_letter, int pin_number)
{

    // Disable JTAG if necessary
    if(port_letter == 'F'){
        teensy_disableJTAG();
    }


    // Disable digital input to pin
    if(port_letter == 'F'){
        set(DIDR0,pin_number);      
    }
    else if(port_letter == 'B'){
        set(DIDR2, pin_number - 1);
    }
    else if(port_letter == 'D'){
        if(pin_number == 4 ){
            set(DIDR2, 0 ); 
        }
        else{
            set(DIDR2,pin_number - 5);
        }
    }




    // SET PIN FOR SINGLE ENDED CHANNEL:

    // Determine if MUX5 needs to be set
    if(port_letter != 'F'){
        set(ADCSRB,MUX5);
    }
    else{
        clear(ADCSRB,MUX5);
    }


    // Determine if MUX2 needs to be set
    if(port_letter == 'F' && pin_number > 3){
        set(ADMUX,MUX2);
    }
    else if(port_letter == 'B' && pin_number > 4){
        set(ADMUX,MUX2);
            }
    else{
        clear(ADMUX,MUX2);
    }


    // Determine if MUX1 needs to be set
    if(port_letter == 'F' && pin_number > 5){
        set(ADMUX,MUX1);
    }
    else if(port_letter == 'B' && pin_number == 4){
        set(ADMUX,MUX1);

    }
    else if(port_letter == 'D' && pin_number == 7){
        set(ADMUX,MUX1);
    }
    else{
        clear(ADMUX,MUX1);
    }


    // Determine if MUX0 needs to be set
    if(port_letter == 'F' && (pin_number % 2 != 0)){
        set(ADMUX,MUX0);
    }
    else if(port_letter == 'D' && (pin_number % 2 == 0)){
        set(ADMUX,MUX0);
    }
    else if(port_letter == 'B' && (pin_number % 2 == 0)){
        set(ADMUX,MUX0);
    }
    else{
        clear(ADMUX,MUX0);

    }


}

// void teamcolor(char redorblue){
//     // Display colored LED
//     if (redorblue == '1'){
//         set(PORTC,8);
//     }
//     else if(redorblue == '0'){
//         clear(PORTC,8);
//     }
// }



void displayHealth(int currentHealth){

    

// 10/10
if (currentHealth > 80){
    set(PORTB,6);
    set(PORTB,5);
    set(PORTB,4);
    set(PORTD,7);
    set(PORTD,6);
}

// 8/10
else if(currentHealth > 60){
    clear(PORTB,6);
    set(PORTB,5);
    set(PORTB,4);
    set(PORTD,7);
    set(PORTD,6);
}

// 6/10
else if(currentHealth > 40){
    clear(PORTB,6);
    clear(PORTB,5);
    set(PORTB,4);
    set(PORTD,7);
    set(PORTD,6);  
}

// 4/10
else if(currentHealth > 20){
    clear(PORTB,6);
    clear(PORTB,5);
    clear(PORTB,4);
    set(PORTD,7);
    set(PORTD,6);
}

// 2/10
else if(currentHealth > 0){
    clear(PORTB,6);
    clear(PORTB,5);
    clear(PORTB,4);
    clear(PORTD,7);
    set(PORTD,6);
}

// 0/10
else{
    clear(PORTB,6);
    clear(PORTB,5);
    clear(PORTB,4);
    clear(PORTD,7);
    clear(PORTD,6);
}


}



void indicateHealing(int healState){



    // If healing light is sensed 
    if(healState == 1){
        // Turn on healing indicator
        set(PORTF,7);
        // teensy_wait(50);
    }

    // If not sensed
    else if(healState == 0){
        // Turn off healing indicator
        clear(PORTF,7);
    }
}



// void indicateWeapon1(int weapon1State){
    
//     // If weapon 1 hit 
//     if(weapon1State == 1){
//         // Turn on hit indicator
//         set(PORTC,7);
//         teensy_wait(50);
//     }

//     // If not sensed
//     else if(weapon1State == 0){
//         // Turn off hit indicator
//         clear(PORTC,7);
//     }
// }



// void indicateWeapon2(int weapon2State){

//     // If weapon 2 hit 
//     if(weapon2State == 1){
//         // Turn on hit indicator
//         set(PORTD,5);
//         teensy_wait(50);
//     }

//     // If not sensed
//     else if(weapon2State == 0){
//         // Turn off hit indicator
//         clear(PORTD,5);
//     }
// }



int weapon1Sensor(){

    // check if button on weapon 1 is set
    if (bit_is_clear(PINF,1)){
        set(PORTC,7);
        // teensy_wait(50);
    }
    else{
        clear(PORTC,7);
        return 0;
    }
}



// int weapon2Sensor(){

//     int force2 = bit_is_set(PORTD,0);

//     // check if button on weapon 2 is set
//     if (force2 == 1){
//         return 1;
//     }
//     else{
//         return 0;
//     }
// }



void weapon1Strike(char F, char R){

    // If switch is set Forward
    if(F == '1' && R == '0'){
        set(PORTF,5);
        clear(PORTF,4);


        // PWM
        set(PORTF,6);
    }
    // If switch is set Reverse
    else if(F == '0' && R == '1'){
        set(PORTF,4);
        clear(PORTF,5);

        // PWM
        set(PORTF,6);
    }
    // If switch is set Neutral
    else if(F == '0' && R == '0'){
        clear(PORTF,4);
        clear(PORTF,5);

        // Set PWM
        clear(PORTF,6);

    }

}



// void weapon2Strike(char F2, char R2){

//     // If switch is set Forward
//     if(F2 == '1' && R2 == '0'){
//         clear(PORTF,6);
//         set(PORTF,5);
//     }
//     // If switch is set Reverse
//     else if(F2 == '0' && R2 == '1'){
//         clear(PORTF,5);
//         set(PORTF,6);
//     }
//     // If switch is set Neutral
//     else if(F2 == '0' && R2 == '0'){
//         clear(PORTF,6);
//         clear(PORTD,5);

//     }

// }


ISR (SPI_STC_vect)
{ // grab byte from SPI Data Register
    //set(PORTD,7);
    
    // set(PORTB,5);
    
    //teensy_led(ON);
    unsigned char c = SPDR;



    if (pos <= PACKETSIZE) { // add to buffer if room 
        buf [pos] = c;
        if (c == 'w'){ // set flag if newline
            process = 1;
        }
        pos++;
    }
    else pos = 0;

}



ISR(TIMER3_COMPA_vect){
    //THIS INTERRUPT IS TRIGGERED EVERY SECOND
    
    //IF STATEMENT FOR FORWARDS
    frequency = counter;
    //m_usb_tx_int(frequency);
            //m_usb_tx_int(flag);
    //m_usb_tx_string("\n");

    counter = 0;


    if(frequency > 126 && frequency < 326 && prevFreq > 126 && prevFreq < 326){
        healingHigh = 1;
    }
    else{
        healingHigh = 0;
    }



    // m_usb_tx_int(frequency);
    // m_usb_tx_string(" ");
    // m_usb_tx_int(prevFreq);
    // m_usb_tx_string(" \n");

    prevFreq = frequency;

}



int main(void)
{

    teensy_clockdivide(0);
    teensy_led(ON);

    // m_usb_init();

    // Set pins for SPI
    set(DDRB,3);
    set(SPCR,SPE);
    set(SPCR,SPIE);
    sei();


    // Set Health Display LED pins to outputs
    set(DDRB,6);
    set(DDRB,5);
    set(DDRB,4);
    set(DDRD,7);
    set(DDRD,6);



    // Set Healing indicator to output
    set(DDRF,7);


    // Set Weapon indicators to outputs
    set(DDRC,7);
    set(DDRD,5);


    // Set Weapon Controls to Outputs
    set(DDRF,4);
    set(DDRF,5);
    set(DDRF,6);


    // Set sensors to inputs
    clear(DDRF,0);
    clear(DDRF,1);


    set(PORTF,1);

    // Team color switch
    set(DDRD,3);

    set(PORTD,3);




    // Initialize variables and arrays
    char weapon11 = ' ';
    char weapon12 = ' ';
    char weapon21 = ' ';
    char weapon22 = ' ';
    int weapon1Hit = 0;
    int weapon2Hit = 0;

    char health1[2] = "";
    char team = ' ';
    int prevHealth;
    
    //char health2;
    int health;
    int h;




    int val = 0;

    int i;



    set(TIMSK3,OCIE3A);


    set(TCCR3B,CS32);
    clear(TCCR3B,CS31);
    set(TCCR3B,CS30);

    clear(TCCR3B,WGM33);
    clear(TCCR3A,WGM31);
    clear(TCCR3A,WGM30);

    // Start timer
    set(TCCR3B,WGM32);


    OCR3A = 15625; // 1 Hz
    

    // Initialize setup and start reading
    ADC_setup();




    




    for(;;){

        

    // Read phototransistor
        ADC_read('F',0);
        photo = ADC;
        teensy_wait(1);








         // If no light is on the phototransistor 
        // And there was light on the transistor
        if(photo < 100 && flag == 1){
        // set(PORTB,6);
        // clear(PORTB,5);
        // Mark the voltage as low
            flag = 0;
        }

        // If there is light on the phototransistor 
        // And there previously wasn't
        else if(photo > 100 && flag == 0){

            // Mark voltage as high
            flag = 1;
            // Count a rise
            counter = counter + 1;

        }




        //SPI INPUT
        if (process){
            if(buf[0] == 'a' && buf[7] == 'w'){
                teensy_led(ON);
                // for(i=0;i<PACKETSIZE;i++){
                //     // if(buf[i] != '0' && buf[i] != '1' && buf[i] != 'w' && buf[i] != '5'){
                //     m_usb_tx_char(buf[i]);
                //     // }
                            
                //     //
                // }
                                // Pull out weapon 1 state
                weapon11 = buf[1];
                weapon12 = buf[2];

                // Pull out weapon 1 state
                weapon21 = buf[3];
                weapon22 = buf[4];

                // Pull out weapon 1 state
                health1[0] = buf[5];


                health1[1] = buf[6];
                health = atoi(health1);
                // h = atoi(health1[1]);


                //Pull out red or blue team
                //team = buf[6];


                pos = 0;
                process = 0;
                teensy_led(OFF);
                // clear(PORTB,5);
                // m_usb_tx_int(health);
                // m_usb_tx_int(h);
                // m_usb_tx_string(" \n");
                // m_usb_tx_string(" \n");
        
            }


                

            
            // weapon12I = atoi(weapon12);
            // weapon11I = atoi(weapon11);
            // weapon21I = atoi(weapon21);
            // weapon22I = atoi(weapon22);
// 
        }



        ///////////////
 

        
        // m_usb_tx_char(weapon11);
        // m_usb_tx_char(weapon12);
        // m_usb_tx_char(health1[0]);
        // m_usb_tx_char(health1[1]);




        // Check Weapon Sensors
        weapon1Hit = weapon1Sensor();
       // weapon2Hit = weapon2Sensor();




        //Update health display
        displayHealth(health);

        // Update healing indicator
        indicateHealing(healingHigh);


        // Update weapons indicators
        // indicateWeapon1(weapon1Hit);       
        //indicateWeapon2(weapon2Hit);
     




        // Control weapons
        weapon1Strike(weapon11,weapon12);
       // weapon2Strike(weapon21,weapon22);


        if(health == 0){
            // HEALTH BAR LEDS
            clear(PORTB,6);
            clear(PORTB,5);
            clear(PORTB,4);
            clear(PORTD,7);
            clear(PORTD,6);

            // HEALING INDICATOR
            clear(PORTF,7);

            // TEAM COLOR
            clear(PORTD,3);


            // WEAPON INDICATORS
            clear(PORTC,7);
            // clear(PORT,)
        }
        else{
            set(PORTD,3);
        }
        
        


    }

    
    return 0;   /* never reached */
}


