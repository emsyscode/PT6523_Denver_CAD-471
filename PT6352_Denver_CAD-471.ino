/*
This code is not clean and is not perfect, this is only 
a reference to extract ideas and adapte to your solution.
*/

//This code is used at PT6523 on DENVER CAD-471 (Car Radio Panel)
//
void send_char(unsigned char a);
void send_data(unsigned char a);
void send_update_clock(void);
void DigitTo7SegEncoder(unsigned char digit);
void update_clock(void );

#define LCD_in 8  // This is the pin number 8 on Arduino UNO
#define LCD_clk 9 // This is the pin number 9 on Arduino UNO
#define LCD_stb 10 // This is the pin number 10 on Arduino UNO
#define LCD_INH 11 // This is the pin number 11 on Arduino UNO
#define fase2 12
#define led7 7
#define led6 6


//unsigned int numberSeg = 0;  // Variable to supporte the number of segment
//unsigned int numberByte = 0; // Variable to supporte the number byte 
unsigned int shiftBit=0;
unsigned int nBitOnBlock=0; // Used to count number of bits and split to 8 bits... (number of byte)
unsigned int nByteOnBlock=0; 
unsigned int sequencyByte=0x00;
unsigned int statusPin12 = LOW;
byte Aa,Ab,Ac,Ad,Ae,Af,Ag,Ah,Ai,Aj,Ak,Al,Am,An,Ao,Ap,Aq,Ar,As,At ;
byte blockBit =0x00;

#define BUTTON_PIN2 2 //Att check wich pins accept interrupts... Uno is 2 & 3
volatile byte buttonReleased2 = false;

#define BUTTON_PIN3 3 //Att check wich pins accept interrupts... Uno is 2 & 3
volatile byte buttonReleased3 = false;

// variables will change:
int buttonState = 0;  // variable for reading the pushbutton status

bool forward = false;
bool backward = false;
bool isRequest = true;
bool allOn=false;
bool cycle=false;

//ATT: On the Uno and other ATMEGA based boards, unsigned ints (unsigned integers) are the same as ints in that they store a 2 byte value.
//Long variables are extended size variables for number storage, and store 32 bits (4 bytes), from -2,147,483,648 to 2,147,483,647.

//*************************************************//
void setup() {
  pinMode(LCD_clk, OUTPUT);
  pinMode(LCD_in, OUTPUT);
  pinMode(LCD_stb, OUTPUT);
  pinMode(LCD_INH, OUTPUT);//When this pin is Low, the display is forcibly turned off. (SG1~SG52, COM1~COM3 are set to Low)
                           //When /INH = Low: Serial data transfers can be performed when the display is forcibly off. 
  pinMode(13, OUTPUT);
  pinMode(led6, OUTPUT);
  pinMode(led7, OUTPUT);
  pinMode(fase2, INPUT);

  pinMode(BUTTON_PIN2, INPUT_PULLUP);
  pinMode(BUTTON_PIN3, INPUT);//Here we don't apply Pull Up because the quadrature button have a resistor in each phase of 56k
                              //this will result at divisor of voltage not equilibrated. I apply external pull-up of 220k

 attachInterrupt(digitalPinToInterrupt(BUTTON_PIN2),
                  buttonReleasedInterrupt2,
                  FALLING);

  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN3),
                  buttonReleasedInterrupt3,
                  RISING);

  Serial.begin(115200);

  digitalWrite(LCD_stb, HIGH);
  delay(5);
  digitalWrite(LCD_INH, HIGH);
  delay(5);
  digitalWrite(13, HIGH);
  delay(50);
  digitalWrite(13, LOW);
  delay(50);
  digitalWrite(13, HIGH);
  delay(50);
  digitalWrite(13, LOW);
  delay(50);
}
/*****************************************************/
void send_char(unsigned char a){
 unsigned char data = 170; //value to transmit, binary 10101010
 unsigned char mask = 1; //our bitmask
  data=a;
  //
  digitalWrite(LCD_stb, LOW); // When strobe is low, all output is enable. If high, all output will be set to low.
  delayMicroseconds(5);
  digitalWrite(LCD_clk,LOW);// need invert the signal to allow 8 bits is is low only send 7 bits
  delayMicroseconds(5);
  for (mask = 0B00000001; mask>0; mask <<= 1) { //iterate through bit mask
  digitalWrite(LCD_clk,LOW);// need invert the signal to allow 8 bits is is low only send 7 bits
  delayMicroseconds(5);
    if (data & mask){ // if bitwise AND resolves to true
      digitalWrite(LCD_in, HIGH);
      //Serial.print(1);
    }
    else{ //if bitwise and resolves to false
      digitalWrite(LCD_in, LOW);
      //Serial.print(0);
    }
    digitalWrite(LCD_clk,HIGH);// need invert the signal to allow 8 bits is is low only send 7 bits
    delayMicroseconds(5);
    //
    digitalWrite(LCD_stb, HIGH); // When strobe is low, all output is enable. If high, all output will be set to low.
  delayMicroseconds(5);
  }
}
void send_char_without(unsigned char a){
 unsigned char data = 170; //value to transmit, binary 10101010
 unsigned char mask = 1; //our bitmask
  data=a;
  for (mask = 0B00000001; mask>0; mask <<= 1) { //iterate through bit mask
  digitalWrite(LCD_clk, LOW);
  delayMicroseconds(5);
    if (data & mask){ // if bitwise AND resolves to true
      digitalWrite(LCD_in, HIGH);
      //Serial.print(1);
    }
    else{ //if bitwise and resolves to false
      digitalWrite(LCD_in, LOW);
      //Serial.print(0);
    }
    digitalWrite(LCD_clk,HIGH);// need invert the signal to allow 8 bits is is low only send 7 bits
    delayMicroseconds(5);
  }
}
void send_char_8bit_stb(unsigned char a){
 unsigned char data = 170; //value to transmit, binary 10101010
 unsigned char mask = 1; //our bitmask
 int i = -1;
  data=a;
  //
  for (mask = 0B00000001; mask>0; mask <<= 1) { //iterate through bit mask
   i++;
   digitalWrite(LCD_clk, LOW);
  delayMicroseconds(5);
    if (data & mask){ // if bitwise AND resolves to true
      digitalWrite(LCD_in, HIGH);
      //Serial.print(1);
    }
    else{ //if bitwise and resolves to false
      digitalWrite(LCD_in, LOW);
      //Serial.print(0);
    }
    digitalWrite(LCD_clk,HIGH);// need invert the signal to allow 8 bits is is low only send 7 bits
    delayMicroseconds(1);
    if (i==7){
    //Serial.println(i); // Only to debug
    digitalWrite(LCD_stb, HIGH);
    delayMicroseconds(2);
    }
     
  }
}
/*****************************************************/
void msgDraw0(){
  //send total of 156 bits, the 4 last bits belongs to DR, SC, BU, X;
  /*
  * Notes presents at datasheet of PT6523 (23 pages): 
  * 1. Address: 41H 
  * 2. D1 to D156 = Display Data     When D1 to D156 are set to “1”, Display is turned ON. When D1 to D156 are set to “0”, Display is turned OFF. 
  * 3. DR = 1/2 Bias Drive or 1/3 Bias Drive Switching Control Data 
  * 4. SC = Segment ON/OFF Control Data 
  * 5. BU = Normal Mode/Power-Saving Control Data 
  * 6. x = Not Relevant For example, there are 63 segments that are being used, the 63 bits of display data (D94 to D156) must be sent. Please refer to the diagram below. 
  */
  digitalWrite(LCD_stb, LOW); //
  delayMicroseconds(2);
  send_char_8bit_stb(0B01000001); // firts 8 bits is address, every fixed as (0B010000001), see if clk finish LOW or HIGH Very important!
  //
  send_char_without(0B00000000);  send_char_without(0B00000000); //  8:1    -16:9 
  send_char_without(0B00000001);  send_char_without(0B00000000); // 24:17  -32:25 
  send_char_without(0B00000000);  send_char_without(0B00000000); // 40:33  -48:41  
  send_char_without(0B00000000);  send_char_without(0B00000000); // 56:49  -64:57  
  send_char_without(0B00000000);  send_char_without(0B00000000); // 72:65  -80:73 
  send_char_without(0B00000000);  send_char_without(0B00000000); // 88:81  -96:89 
  send_char_without(0B00000000);  send_char_without(0B00000000); // 104:97-112:105
  send_char_without(0B00000000);  send_char_without(0B00000000); //120:113-128:121
  send_char_without(0B00000000);  send_char_without(0B00000000); //136:129-144:137
  send_char_without(0B00000000);  send_char_without(0B00000000); //152:145-160:153 // 0B0000xxxx see the bits DR, SC, BU, X(This bits switch on/off and sleep...
  delayMicroseconds(2);
  digitalWrite(LCD_stb, LOW); // 
  delayMicroseconds(2);
}
void msgDraw1(){
  //send total of 156 bits, the 4 last bits belongs to DR, SC, BU, X;
  /*
  * Notes presents at datasheet of PT6523 (23 pages): 
  * 1. Address: 41H 
  * 2. D1 to D156 = Display Data     When D1 to D156 are set to “1”, Display is turned ON. When D1 to D156 are set to “0”, Display is turned OFF. 
  * 3. DR = 1/2 Bias Drive or 1/3 Bias Drive Switching Control Data 
  * 4. SC = Segment ON/OFF Control Data 
  * 5. BU = Normal Mode/Power-Saving Control Data 
  * 6. x = Not Relevant For example, there are 63 segments that are being used, the 63 bits of display data (D94 to D156) must be sent. Please refer to the diagram below. 
  */
  digitalWrite(LCD_stb, LOW); //
  delayMicroseconds(2);
  send_char_8bit_stb(0B01000001); // firts 8 bits is address, every fixed as (0B010000001), see if clk finish LOW or HIGH Very important!
  //
  send_char_without(0B11000000);  send_char_without(0B11111111); //  8:1    -16:9 
  send_char_without(0B00000001);  send_char_without(0B00000000); // 24:17  -32:25 
  send_char_without(0B00000000);  send_char_without(0B00000000); // 40:33  -48:41  
  send_char_without(0B00000000);  send_char_without(0B00000000); // 56:49  -64:57  
  send_char_without(0B00000000);  send_char_without(0B00000000); // 72:65  -80:73 
  send_char_without(0B00000000);  send_char_without(0B00000000); // 88:81  -96:89 
  send_char_without(0B00000000);  send_char_without(0B00000000); // 104:97-112:105
  send_char_without(0B00000000);  send_char_without(0B00000000); //120:113-128:121
  send_char_without(0B00000000);  send_char_without(0B00000000); //136:129-144:137
  send_char_without(0B00000000);  send_char_without(0B00000000); //152:145-160:153 // 0B0000xxxx see the bits DR, SC, BU, X(This bits switch on/off and sleep...
  delayMicroseconds(2);
  digitalWrite(LCD_stb, LOW); // 
  delayMicroseconds(2);
}
void arrowLeft(){
  //send total of 156 bits, the 4 last bits belongs to DR, SC, BU, X;
  /*
  * Notes presents at datasheet of PT6523 (23 pages): 
  * 1. Address: 41H 
  * 2. D1 to D156 = Display Data     When D1 to D156 are set to “1”, Display is turned ON. When D1 to D156 are set to “0”, Display is turned OFF. 
  * 3. DR = 1/2 Bias Drive or 1/3 Bias Drive Switching Control Data 
  * 4. SC = Segment ON/OFF Control Data 
  * 5. BU = Normal Mode/Power-Saving Control Data 
  * 6. x = Not Relevant For example, there are 63 segments that are being used, the 63 bits of display data (D94 to D156) must be sent. Please refer to the diagram below. 
  */
  digitalWrite(LCD_stb, LOW); //
  delayMicroseconds(2);
  send_char_8bit_stb(0B01000001); // firts 8 bits is address, every fixed as (0B010000001), see if clk finish LOW or HIGH Very important!
  //
  send_char_without(0B00000000);  send_char_without(0B00000000); //  8:1    -16:9 
  send_char_without(0B00000000);  send_char_without(0B00000000); // 24:17  -32:25 
  send_char_without(0B00000100);  send_char_without(0B00000111); // 40:33  -48:41 
  send_char_without(0B10000010);  send_char_without(0B00000000); // 56:49  -64:57 
  send_char_without(0B00000000);  send_char_without(0B00000000); // 72:65  -80:73  
  send_char_without(0B00000000);  send_char_without(0B00000000); // 88:81  -96:89 
  send_char_without(0B00000000);  send_char_without(0B00000000); // 104:97-112:105
  send_char_without(0B00000000);  send_char_without(0B00000000); //120:113-128:121
  send_char_without(0B00000000);  send_char_without(0B00000000); //136:129-144:137
  send_char_without(0B00000000);  send_char_without(0B00000000); //152:145-160:153 // 0B0000xxxx see the bits DR, SC, BU, X(This bits switch on/off and sleep...
  delayMicroseconds(2);
  digitalWrite(LCD_stb, LOW); // 
  delayMicroseconds(2);
}
void arrowRight(){
  //send total of 156 bits, the 4 last bits belongs to DR, SC, BU, X;
  /*
  * Notes presents at datasheet of PT6523 (23 pages): 
  * 1. Address: 41H 
  * 2. D1 to D156 = Display Data     When D1 to D156 are set to “1”, Display is turned ON. When D1 to D156 are set to “0”, Display is turned OFF. 
  * 3. DR = 1/2 Bias Drive or 1/3 Bias Drive Switching Control Data 
  * 4. SC = Segment ON/OFF Control Data 
  * 5. BU = Normal Mode/Power-Saving Control Data 
  * 6. x = Not Relevant For example, there are 63 segments that are being used, the 63 bits of display data (D94 to D156) must be sent. Please refer to the diagram below. 
  */
  digitalWrite(LCD_stb, LOW); //
  delayMicroseconds(2);
  send_char_8bit_stb(0B01000001); // firts 8 bits is address, every fixed as (0B010000001), see if clk finish LOW or HIGH Very important!
  //
  send_char_without(0B00000000);  send_char_without(0B00000000); //  8:1    -16:9  
  send_char_without(0B00000000);  send_char_without(0B00000000); // 24:17  -32:25  
  send_char_without(0B00000000);  send_char_without(0B00000000); // 40:33  -48:41  
  send_char_without(0B00000000);  send_char_without(0B00000000); // 56:49  -64:57  
  send_char_without(0B01000001);  send_char_without(0B10000000); // 72:65  -80:73  
  send_char_without(0B00100011);  send_char_without(0B00000000); // 88:81  -96:89 
  send_char_without(0B00000000);  send_char_without(0B00000000); // 104:97-112:105
  send_char_without(0B00000000);  send_char_without(0B00000000); //120:113-128:121
  send_char_without(0B00000000);  send_char_without(0B00000000); //136:129-144:137
  send_char_without(0B00000000);  send_char_without(0B00000000); //152:145-160:153 // 0B0000xxxx see the bits DR, SC, BU, X(This bits switch on/off and sleep...
  delayMicroseconds(2);
  digitalWrite(LCD_stb, LOW); // 
  delayMicroseconds(2);
}
void msgHi(){
  //send total of 156 bits, the 4 last bits belongs to DR, SC, BU, X;
  /*
  * Notes presents at datasheet of PT6523 (23 pages): 
  * 1. Address: 41H 
  * 2. D1 to D156 = Display Data     When D1 to D156 are set to “1”, Display is turned ON. When D1 to D156 are set to “0”, Display is turned OFF. 
  * 3. DR = 1/2 Bias Drive or 1/3 Bias Drive Switching Control Data 
  * 4. SC = Segment ON/OFF Control Data 
  * 5. BU = Normal Mode/Power-Saving Control Data 
  * 6. x = Not Relevant For example, there are 63 segments that are being used, the 63 bits of display data (D94 to D156) must be sent. Please refer to the diagram below. 
  */
  digitalWrite(LCD_stb, LOW); //
  delayMicroseconds(2);
  send_char_8bit_stb(0B01000001); // firts 8 bits is address, every fixed as (0B010000001), see if clk finish LOW or HIGH Very important!
  //
  send_char_without(0B00011100);  send_char_without(0B01100100); //  8:1    -16:9  //H
  send_char_without(0B11000000);  send_char_without(0B00000001); // 24:17  -32:25  //I
  send_char_without(0B00000000);  send_char_without(0B00000000); // 40:33  -48:41  //Space part of "F", f,
  send_char_without(0B00000000);  send_char_without(0B00000000); // 56:49  -64:57  //F
  send_char_without(0B00000000);  send_char_without(0B00000000); // 72:65  -80:73   //O
  send_char_without(0B00000000);  send_char_without(0B00000000); // 88:81  -96:89 
  send_char_without(0B00000000);  send_char_without(0B00000000); // 104:97-112:105
  send_char_without(0B00000000);  send_char_without(0B00000000); //120:113-128:121
  send_char_without(0B00000000);  send_char_without(0B00000000); //136:129-144:137
  send_char_without(0B00000000);  send_char_without(0B00000000); //152:145-160:153 // 0B0000xxxx see the bits DR, SC, BU, X(This bits switch on/off and sleep...
  delayMicroseconds(2);
  digitalWrite(LCD_stb, LOW); // 
  delayMicroseconds(2);
}
void msgFolks(){
  //send total of 156 bits, the 4 last bits belongs to DR, SC, BU, X;
  /*
  * Notes presents at datasheet of PT6523 (23 pages): 
  * 1. Address: 41H 
  * 2. D1 to D156 = Display Data     When D1 to D156 are set to “1”, Display is turned ON. When D1 to D156 are set to “0”, Display is turned OFF. 
  * 3. DR = 1/2 Bias Drive or 1/3 Bias Drive Switching Control Data 
  * 4. SC = Segment ON/OFF Control Data 
  * 5. BU = Normal Mode/Power-Saving Control Data 
  * 6. x = Not Relevant For example, there are 63 segments that are being used, the 63 bits of display data (D94 to D156) must be sent. Please refer to the diagram below. 
  */
  digitalWrite(LCD_stb, LOW); //
  delayMicroseconds(2);
  send_char_8bit_stb(0B01000001); // firts 8 bits is address, every fixed as (0B010000001), see if clk finish LOW or HIGH Very important!
  //
  send_char_without(0B00011100);  send_char_without(0B00000101); //  8:1    -16:9 
  send_char_without(0B10000110);  send_char_without(0B00110001); // 24:17  -32:25 
  send_char_without(0B10000011);  send_char_without(0B10000000); // 40:33  -48:41  
  send_char_without(0B00000011);  send_char_without(0B01000011); // 56:49  -64:57  
  send_char_without(0B01110001);  send_char_without(0B00000010); // 72:65  -80:73 
  send_char_without(0B00000000);  send_char_without(0B00000000); // 88:81  -96:89 
  send_char_without(0B00000000);  send_char_without(0B00000000); // 104:97-112:105
  send_char_without(0B00000000);  send_char_without(0B00000000); //120:113-128:121
  send_char_without(0B00000000);  send_char_without(0B00000000); //136:129-144:137
  send_char_without(0B00000000);  send_char_without(0B00000000); //152:145-160:153 // 0B0000xxxx see the bits DR, SC, BU, X(This bits switch on/off and sleep...
  delayMicroseconds(2);
  digitalWrite(LCD_stb, LOW); // 
  delayMicroseconds(2);
}
void msgHiFolks(){
  //send total of 156 bits, the 4 last bits belongs to DR, SC, BU, X;
  /*
  * Notes presents at datasheet of PT6523 (23 pages): 
  * 1. Address: 41H 
  * 2. D1 to D156 = Display Data     When D1 to D156 are set to “1”, Display is turned ON. When D1 to D156 are set to “0”, Display is turned OFF. 
  * 3. DR = 1/2 Bias Drive or 1/3 Bias Drive Switching Control Data 
  * 4. SC = Segment ON/OFF Control Data 
  * 5. BU = Normal Mode/Power-Saving Control Data 
  * 6. x = Not Relevant For example, there are 63 segments that are being used, the 63 bits of display data (D94 to D156) must be sent. Please refer to the diagram below. 
  */
  digitalWrite(LCD_stb, LOW); //
  delayMicroseconds(2);
  send_char_8bit_stb(0B01000001); // firts 8 bits is address, every fixed as (0B010000001), see if clk finish LOW or HIGH Very important!
  //
  send_char_without(0B00011100);  send_char_without(0B01100100); //  8:1    -16:9  //H
  send_char_without(0B11000000);  send_char_without(0B00000001); // 24:17  -32:25  //I
  send_char_without(0B00000000);  send_char_without(0B10000000); // 40:33  -48:41  //Space part of "F", f,
  send_char_without(0B10100011);  send_char_without(0B11000000); // 56:49  -64:57  //F
  send_char_without(0B00110000);  send_char_without(0B01100110); // 72:65  -80:73   //O
  send_char_without(0B00010000);  send_char_without(0B01110000); // 88:81  -96:89 
  send_char_without(0B01100000);  send_char_without(0B00101000); // 104:97-112:105
  send_char_without(0B01001110);  send_char_without(0B00000000); //120:113-128:121
  send_char_without(0B00000000);  send_char_without(0B00000000); //136:129-144:137
  send_char_without(0B00000000);  send_char_without(0B00000000); //152:145-160:153 // 0B0000xxxx see the bits DR, SC, BU, X(This bits switch on/off and sleep...
  delayMicroseconds(2);
  digitalWrite(LCD_stb, LOW); // 
  delayMicroseconds(2);
}
void segments(){
  //Bit function: 
      digitalWrite(LCD_stb, LOW); //
      delayMicroseconds(1);
      send_char_8bit_stb(0B01000001); //(0x4B) firts 8 bits is address, every fixed as (0B01000010), see if clk finish LOW or HIGH Very important!
      delayMicroseconds(1);
      // 
          send_char_without(~Aa);  send_char_without(~Ab);  //   8:1     -16:9// 
          send_char_without(~Ac);  send_char_without(~Ad);  //  24:17    -32:25//
          send_char_without(~Ae);  send_char_without(~Af);  //  40:33    -48:41// 
          send_char_without(~Ag);  send_char_without(~Ah);  //  56:49    -64:57// 
          send_char_without(~Ai);  send_char_without(~Aj);  //  72:65    -80:73// 
          send_char_without(~Ak);  send_char_without(~Al);  //  88:81    -96:89//  
          send_char_without(~Am);  send_char_without(~An);  // 104:97  -112:105//  
          send_char_without(~Ao);  send_char_without(~Ap);  // 120:113 -128:121// 
          send_char_without(~Aq);  send_char_without(~Ar);  // 136:129 -144:137// 
          send_char_without(~As);  send_char_without(At=(~At & 0x0F));  // 152:145 -160:153//   
          //The next switch finalize the burst of bits -41:48//  
      delayMicroseconds(1);
      digitalWrite(LCD_stb, LOW); //                   
}
void searchOfSegments(){
  int group = 0x00;
  byte nMask = 0B00000001;
  unsigned int nSeg=0x000;  // This variable need support more of 255 (600 segments)
  Serial.println();
  Serial.println("We entered test mode for the segments!");
  
  for( group=0; group<19; group++){   // Do until n bits 15*8 bits
          for (nMask = 0B00000001; nMask>0; nMask <<= 1){
            Aa=0x00; Ab=0x00; Ac=0x00; Ad=0x00; Ae=0x00; Af=0x00; Ag=0x00; Ah=0x00; Ai=0x00; Aj=0x00;
            Ak=0x00; Al=0x00; Am=0x00; An=0x00; Ao=0x00; Ap=0x00; Aq=0x00; Ar=0x00; As=0x00; At=0x00;
                  switch (group){
                    case 0:   Aa=nMask; break;
                    case 1:   Ab=nMask; break;//atoi(to integer)
                    case 2:   Ac=nMask; break;
                    case 3:   Ad=nMask; break;
                    case 4:   Ae=nMask; break;
                    case 5:   Af=nMask; break;
                    case 6:   Ag=nMask; break;//atoi(to integer)
                    case 7:   Ah=nMask; break;
                    case 8:   Ai=nMask; break;
                    case 9:   Aj=nMask; break;
                    case 10:  Ak=nMask; break;
                    case 11:  Al=nMask; break;//atoi(to integer)
                    case 12:  Am=nMask; break;
                    case 13:  An=nMask; break;
                    case 14:  Ao=nMask; break;
                    case 15:  Ap=nMask; break;
                    case 16:  Aq=nMask; break;//atoi(to integer)
                    case 17:  Ar=nMask; break;
                    case 18:  As=nMask; break;
                    case 19:  At=nMask; break;
                  } 
           nSeg++;   
      //This start the control of button to allow continue teste! 
                      while(1){
                            if(!buttonReleased2){
                              delay(200);
                            }
                            else{
                              delay(15);
                               buttonReleased2 = false;
                               break;
                               }
                         }
               delay(50);
            segments();
            Serial.print(nSeg, DEC); Serial.print(", group: "); Serial.print(group, DEC);Serial.print(", BlockBit: "); Serial.print(blockBit, HEX);Serial.print(", nMask: "); Serial.print(nMask, BIN);Serial.print("   \t");
            Serial.print(Ae, HEX);Serial.print(", ");Serial.print(Ad, HEX);Serial.print(", ");Serial.print(Ac, HEX);Serial.print(", ");Serial.print(Ab, HEX);Serial.print(", ");Serial.print(Aa, HEX); Serial.print("; ");
            Serial.print(Aj, HEX);Serial.print(", ");Serial.print(Ai, HEX);Serial.print(", ");Serial.print(Ah, HEX);Serial.print(", ");Serial.print(Ag, HEX);Serial.print(", ");Serial.print(Af, HEX); Serial.print("; ");
            Serial.print(Ao, HEX);Serial.print(", ");Serial.print(An, HEX);Serial.print(", ");Serial.print(Am, HEX);Serial.print(", ");Serial.print(Al, HEX);Serial.print(", ");Serial.print(Ak, HEX); Serial.print("; ");
            Serial.print(At, HEX);Serial.print(", ");Serial.print(As, HEX);Serial.print(", ");Serial.print(Ar, HEX);Serial.print(", ");Serial.print(Aq, HEX);Serial.print(", ");Serial.print(Ap, HEX); Serial.print("; ");
            
            Serial.println();
            delay (250);          
           }        
      }
}
void allON(){
  //send total of 156 bits, the 4 last bits belongs to DR, SC, BU, X;
  digitalWrite(LCD_stb, LOW); //
  delayMicroseconds(2);
  send_char_8bit_stb(0B01000001); // firts 8 bits is address, every fixed as (0B010000001), see if clk finish LOW or HIGH Very important!
  //
  send_char_without(0B11111111);  send_char_without(0B11111111);  send_char_without(0B11111111);  send_char_without(0B11111111);  send_char_without(0B11111111);  
  send_char_without(0B11111111);  send_char_without(0B11111111);  send_char_without(0B11111111);  send_char_without(0B11111111);  send_char_without(0B11111111); 
  send_char_without(0B11111111);  send_char_without(0B11111111);  send_char_without(0B11111111);  send_char_without(0B11111111);  send_char_without(0B11111111);  
  send_char_without(0B11111111);  send_char_without(0B11111111);  send_char_without(0B11111111);  send_char_without(0B11111111);  send_char_without(0B00001111);
  delayMicroseconds(2);
  digitalWrite(LCD_stb, LOW); // 
  delayMicroseconds(2);
}
void allOFF(){
  //send total of 156 bits, the 4 last bits belongs to DR, SC, BU, X;
  digitalWrite(LCD_stb, LOW); //
  delayMicroseconds(2);
  send_char_8bit_stb(0B01000001); // firts 8 bits is address, every fixed as (0B010000001), see if clk finish LOW or HIGH Very important!
  //
  send_char_without(0B00000000);  send_char_without(0B00000000);  send_char_without(0B00000000);  send_char_without(0B00000000);  send_char_without(0B00000000);  
  send_char_without(0B00000000);  send_char_without(0B00000000);  send_char_without(0B00000000);  send_char_without(0B00000000);  send_char_without(0B00000000); 
  send_char_without(0B00000000);  send_char_without(0B00000000);  send_char_without(0B00000000);  send_char_without(0B00000000);  send_char_without(0B00000000); 
  send_char_without(0B00000000);  send_char_without(0B00000000);  send_char_without(0B00000000);  send_char_without(0B00000000);  send_char_without(0B01100000);
  delayMicroseconds(2);
  digitalWrite(LCD_stb, LOW); // 
  delayMicroseconds(2);
}
void trigger(){
 statusPin12 = digitalRead(fase2);
  if(statusPin12 == HIGH){ //If pin is high.
    // digitalWrite(4, HIGH);
    // digitalWrite(5, LOW);
    arrowRight();
  }
  else{
    // digitalWrite(4, LOW);
    // digitalWrite(5, HIGH);
    arrowLeft();
  }
}
void loop() {
   for(uint8_t i = 0x00; i < 4; i++){
    allON(); // All on
    delay(1500);
    allOFF(); // All off
    delay(1000);
   }
    //
    msgHi();
    delay(800);
    msgFolks();
    delay(2000);
    allOFF(); // All off
    delay(500);
    msgHiFolks();
    delay(1000);

    allOFF(); // All off

   //searchOfSegments();
}
void buttonReleasedInterrupt2() {
  buttonReleased2 = true; // This is the line of interrupt button to advance one step on the search of segments!
}
void buttonReleasedInterrupt3() {
  //buttonReleased3 = true; //
 trigger();
}

