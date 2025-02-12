#include <io.h>
#include <i2c.h>
#include <alcd.h>
#include <delay.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <ds1307.h>

#define DeviceAddress       0xA0
#define mainMenuAdd         1
#define buffSize            16
#define maxMenuitemsX       3
#define maxMenuitemsY       3

// Declare your global variables here
void EEPROM_WritePage(uint8_t deviceAddress, uint8_t memAddress, uint8_t* data, uint8_t len);
void EEPROM_ReadPage(uint8_t deviceAddress, uint8_t memAddress, uint8_t* data, uint8_t len);
void showTimedate();

unsigned int i,j;
int MemAddress = mainMenuAdd;
char tempStr[buffSize];
char mStr[buffSize];
//int SubAddress = mainMenuAdd + ((maxMenuitemsX+1)*buffSize);

/*************************************
MM
{{"", 4}, {"", 5}, {"", 7}};
SM
{{"",3},{"",4},{"",5}},
{{"",2},{"",6},{"",2}},
{{"",7},{"",7},{"",9}}
};
**************************************/
char * psw[2] = {"0000","1111"};
char * str[maxMenuitemsX]   = {"File","Motor","Setting"};
//                               4       5        7
char * stritems[maxMenuitemsX][maxMenuitemsY]= 
{
{"New","Edit","Close"},
{"DC","Steper","AC"},
{"SetPass","Voltage","Date&Time"}
};

uint8_t hour;
uint8_t min;
uint8_t sec;
uint8_t weekDay;
uint8_t year;
uint8_t month;
uint8_t day;

void main(void)
{
// Declare your local variables here

// Input/Output Ports initialization
// Port A initialization
// Function: Bit7=In Bit6=In Bit5=In Bit4=In Bit3=In Bit2=In Bit1=In Bit0=In 
DDRA=(0<<DDA7) | (0<<DDA6) | (0<<DDA5) | (0<<DDA4) | (0<<DDA3) | (0<<DDA2) | (0<<DDA1) | (0<<DDA0);
// State: Bit7=T Bit6=T Bit5=T Bit4=T Bit3=T Bit2=T Bit1=T Bit0=T 
PORTA=(0<<PORTA7) | (0<<PORTA6) | (0<<PORTA5) | (0<<PORTA4) | (0<<PORTA3) | (0<<PORTA2) | (0<<PORTA1) | (0<<PORTA0);

// Port B initialization
// Function: Bit7=In Bit6=In Bit5=In Bit4=In Bit3=In Bit2=In Bit1=In Bit0=In 
DDRB=(0<<DDB7) | (0<<DDB6) | (0<<DDB5) | (0<<DDB4) | (0<<DDB3) | (0<<DDB2) | (0<<DDB1) | (0<<DDB0);
// State: Bit7=T Bit6=T Bit5=T Bit4=T Bit3=T Bit2=T Bit1=T Bit0=T 
PORTB=(0<<PORTB7) | (0<<PORTB6) | (0<<PORTB5) | (0<<PORTB4) | (0<<PORTB3) | (0<<PORTB2) | (0<<PORTB1) | (0<<PORTB0);

// Port C initialization
// Function: Bit7=In Bit6=In Bit5=In Bit4=In Bit3=In Bit2=In Bit1=In Bit0=In 
DDRC=(0<<DDC7) | (0<<DDC6) | (0<<DDC5) | (0<<DDC4) | (0<<DDC3) | (0<<DDC2) | (0<<DDC1) | (0<<DDC0);
// State: Bit7=T Bit6=T Bit5=T Bit4=T Bit3=T Bit2=T Bit1=T Bit0=T 
PORTC=(0<<PORTC7) | (0<<PORTC6) | (0<<PORTC5) | (0<<PORTC4) | (0<<PORTC3) | (0<<PORTC2) | (0<<PORTC1) | (0<<PORTC0);

// Port D initialization
// Function: Bit7=In Bit6=In Bit5=In Bit4=In Bit3=In Bit2=In Bit1=In Bit0=In 
DDRD=(0<<DDD7) | (0<<DDD6) | (0<<DDD5) | (0<<DDD4) | (0<<DDD3) | (0<<DDD2) | (0<<DDD1) | (0<<DDD0);
// State: Bit7=T Bit6=T Bit5=T Bit4=T Bit3=T Bit2=T Bit1=T Bit0=T 
PORTD=(0<<PORTD7) | (0<<PORTD6) | (0<<PORTD5) | (0<<PORTD4) | (0<<PORTD3) | (0<<PORTD2) | (0<<PORTD1) | (0<<PORTD0);

// Timer/Counter 0 initialization
// Clock source: System Clock
// Clock value: Timer 0 Stopped
// Mode: Normal top=0xFF
// OC0 output: Disconnected
TCCR0=(0<<WGM00) | (0<<COM01) | (0<<COM00) | (0<<WGM01) | (0<<CS02) | (0<<CS01) | (0<<CS00);
TCNT0=0x00;
OCR0=0x00;

// Timer/Counter 1 initialization
// Clock source: System Clock
// Clock value: Timer1 Stopped
// Mode: Normal top=0xFFFF
// OC1A output: Disconnected
// OC1B output: Disconnected
// Noise Canceler: Off
// Input Capture on Falling Edge
// Timer1 Overflow Interrupt: Off
// Input Capture Interrupt: Off
// Compare A Match Interrupt: Off
// Compare B Match Interrupt: Off
TCCR1A=(0<<COM1A1) | (0<<COM1A0) | (0<<COM1B1) | (0<<COM1B0) | (0<<WGM11) | (0<<WGM10);
TCCR1B=(0<<ICNC1) | (0<<ICES1) | (0<<WGM13) | (0<<WGM12) | (0<<CS12) | (0<<CS11) | (0<<CS10);
TCNT1H=0x00;
TCNT1L=0x00;
ICR1H=0x00;
ICR1L=0x00;
OCR1AH=0x00;
OCR1AL=0x00;
OCR1BH=0x00;
OCR1BL=0x00;

// Timer/Counter 2 initialization
// Clock source: System Clock
// Clock value: Timer2 Stopped
// Mode: Normal top=0xFF
// OC2 output: Disconnected
ASSR=0<<AS2;
TCCR2=(0<<PWM2) | (0<<COM21) | (0<<COM20) | (0<<CTC2) | (0<<CS22) | (0<<CS21) | (0<<CS20);
TCNT2=0x00;
OCR2=0x00;

// Timer(s)/Counter(s) Interrupt(s) initialization
TIMSK=(0<<OCIE2) | (0<<TOIE2) | (0<<TICIE1) | (0<<OCIE1A) | (0<<OCIE1B) | (0<<TOIE1) | (0<<OCIE0) | (0<<TOIE0);

// External Interrupt(s) initialization
// INT0: Off
// INT1: Off
// INT2: Off
MCUCR=(0<<ISC11) | (0<<ISC10) | (0<<ISC01) | (0<<ISC00);
MCUCSR=(0<<ISC2);

// USART initialization
// USART disabled
UCSRB=(0<<RXCIE) | (0<<TXCIE) | (0<<UDRIE) | (0<<RXEN) | (0<<TXEN) | (0<<UCSZ2) | (0<<RXB8) | (0<<TXB8);

// Analog Comparator initialization
// Analog Comparator: Off
// The Analog Comparator's positive input is
// connected to the AIN0 pin
// The Analog Comparator's negative input is
// connected to the AIN1 pin
ACSR=(1<<ACD) | (0<<ACBG) | (0<<ACO) | (0<<ACI) | (0<<ACIE) | (0<<ACIC) | (0<<ACIS1) | (0<<ACIS0);
SFIOR=(0<<ACME);

// ADC initialization
// ADC disabled
ADCSRA=(0<<ADEN) | (0<<ADSC) | (0<<ADATE) | (0<<ADIF) | (0<<ADIE) | (0<<ADPS2) | (0<<ADPS1) | (0<<ADPS0);

// SPI initialization
// SPI disabled
SPCR=(0<<SPIE) | (0<<SPE) | (0<<DORD) | (0<<MSTR) | (0<<CPOL) | (0<<CPHA) | (0<<SPR1) | (0<<SPR0);

// TWI initialization
// TWI disabled
TWCR=(0<<TWEA) | (0<<TWSTA) | (0<<TWSTO) | (0<<TWEN) | (0<<TWIE);

// Bit-Banged I2C Bus initialization
// I2C Port: PORTD
// I2C SDA bit: 1
// I2C SCL bit: 0
// Bit Rate: 100 kHz
// Note: I2C settings are specified in the
// Project|Configure|C Compiler|Libraries|I2C menu.
i2c_init();

// Alphanumeric LCD initialization
// Connections are specified in the
// Project|Configure|C Compiler|Libraries|Alphanumeric LCD menu:
// RS - PORTB Bit 0
// RD - PORTB Bit 1
// EN - PORTB Bit 2
// D4 - PORTB Bit 4
// D5 - PORTB Bit 5
// D6 - PORTB Bit 6
// D7 - PORTB Bit 7
// Characters/line: 8
lcd_init(16);

rtc_set_date(7,  5, 24, 24);
rtc_set_time(8, 6, 1);
    
// Writing password in EEPROM
EEPROM_WritePage(DeviceAddress, MemAddress, (char*)psw[0], strlen(psw[0]));
delay_ms(10);
MemAddress += buffSize ;

for(i=0; i<maxMenuitemsX ; i++){
    EEPROM_WritePage(DeviceAddress, MemAddress, (char*)str[i], strlen(str[i]));
    delay_ms(10);
    MemAddress += buffSize ;
}

for(i=0; i<maxMenuitemsX ; i++){
    for(j=0; j<maxMenuitemsY ; j++){
        EEPROM_WritePage(DeviceAddress, MemAddress, (char*)stritems[i][j], strlen(stritems[i][j]));
        delay_ms(10);
        MemAddress += buffSize ;
    }
} 

// Reading
MemAddress = mainMenuAdd;
EEPROM_ReadPage(DeviceAddress, MemAddress , tempStr, strlen(psw[0]));
delay_ms(10);
MemAddress += buffSize ;
lcd_clear();
lcd_gotoxy(0,0);
lcd_puts(tempStr);
delay_ms(1000);

for(i=0; i<maxMenuitemsX ; i++){
    EEPROM_ReadPage(DeviceAddress, MemAddress , tempStr, strlen(str[i]));
    delay_ms(10);
    MemAddress += buffSize ;
    lcd_clear();
    lcd_gotoxy(0,0);
    lcd_puts(tempStr);
    delay_ms(1000);
}

for(i=0; i<maxMenuitemsX ; i++){
    for(j=0; j<maxMenuitemsY ; j++){
        EEPROM_ReadPage(DeviceAddress, MemAddress, tempStr, strlen(stritems[i][j]));
        delay_ms(10);
        MemAddress += buffSize ;
        lcd_clear();
        lcd_gotoxy(0,1);
        lcd_puts(tempStr);
        delay_ms(1000);
    }
}

lcd_clear();
lcd_puts("FINISH.");

//showTimedate();  

while (1)
{
}
}

void EEPROM_WritePage(uint8_t deviceAddress, uint8_t memAddress, uint8_t* data, uint8_t len){
    uint8_t current_byte;
    i2c_start();
    i2c_write(deviceAddress);
    i2c_write(memAddress);
    while (len--) {
        current_byte = *data++;
        i2c_write(current_byte);
}
current_byte = '\0';
i2c_write(current_byte);
i2c_stop();
}

void EEPROM_ReadPage(uint8_t deviceAddress, uint8_t memAddress, uint8_t* data, uint8_t len) {
    i2c_start();
    i2c_write(deviceAddress);
    i2c_write(memAddress);
    i2c_start();
    i2c_write(deviceAddress | 0x01);
    while (--len) {
        *data++ = i2c_read(1);
    }
    *data++ = i2c_read(0);
    *data = '\0'; // ����� ���� '\0' �� ������ ����
    i2c_stop();
}

void showTimedate()
{
    int k=0;
    for(k=0;k<6;k++)
    { 
        rtc_get_date(&weekDay, &day, &month, &year);
        sprintf(mStr, "%02u/%02u/%02u - Day %u", month, day, year, weekDay);
        lcd_clear();
        lcd_gotoxy(0, 1);
        lcd_puts(mStr);
        rtc_get_time(&hour, &min, &sec);
        sprintf(mStr, "%02u:%02u:%02u", hour, min, sec);
        lcd_gotoxy(0, 0);
        lcd_puts(mStr);
        delay_ms(1000);
    }
}
