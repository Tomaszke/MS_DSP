// By ENGELS Tomasz
// For MS1 v2.2 with MSNS-extra code
// "MS1/Extra format 029y3 *********"
// under GPL license

// Values to be showed (byte nbr of MS data ex. RPM = 13)
int showVal[] = { 2, 13, 6, 24, 27, 25, 5, 4, 28, 29};

// Button declares
const int  buttonPin = 7; //PIN on which the button will be connected
int buttonPushCounter = 0;   // counter for the number of button presses
int buttonPushMax = sizeof(showVal)/2; //maximum of presses before returning to start value (calculated by the number of elements in showVal[]
int buttonState = 0;         // current state of the button
int lastButtonState = 0;     // previous state of the button

// Others
int result;
int incomingByte = 0;
int MAX_WAIT = 1000;
int cycle_delay = 200; // Wait tinme before the loop start again
unsigned long starttime;
int Data_In[99];
String Data_Output[99];
String Data_Output_desc[99];
byte Data_In_Bytes[99];
int thermfactor[255];
int mapfactor[255];
int matfactor[255];
int tpsfactor[255];
int serialrate=9600;
String signature; //


// include the LCD library code:
#include <LiquidCrystal.h>
LiquidCrystal lcd(12, 11, 5, 4, 3, 2); // initialize the LCD-library with the numbers of the interface pins

void setup() {
  
    int x=0;
    
    lcd.begin(16, 2); // Set the LCD as 
    pinMode(buttonPin, INPUT); // Set the buttonPin as input
    lcd.print(buttonPushMax);
    lcd.print("Init MS_DSP");  // Display Init message
    lcd.setCursor(0,1);  // Set position for 10 sec counter
    for(int n=2; n>0; n--)  // Counter loop
    {
      lcd.setCursor(0,1);
      lcd.print(n);
      delay(1000);
    }
      
    Serial.begin(serialrate); // opens serial port, sets data rate to "serialrate" bps
    lcd.clear();
    do
    {
    Serial.print("S"); // Send command to read signature
    lcd.setCursor(0,0);
    delay(cycle_delay); //Wait for answer
    result = GetSerialData(32);
    if (result = 1)
    {
      for (int i=0;i<32;i++)
      {
        signature=signature+Data_In[i];
      }
      lcd.clear();
      lcd.print(signature);
      for(int n=5; n>0; n--)  // Counter loop
        {
          lcd.setCursor(0,1);
          lcd.print(n);
          delay(1000);
        }
    }
    else
    {
      x++;
      lcd.setCursor(0,0);
      lcd.clear();
      lcd.print("Can't connect: try " + x);
    }
    }while (result = 0);
    
    
    
}

void loop(){
  
  buttonPoller();
  Serial.print("R"); // Send command to read realtime values (output 39 bytes)
  result = GetSerialData(39);
  if(result=1) // Hop to function to get the 39 bytes of data
  {
    CalcData(); // Hop to function to calculate the received bytes
    DisplayVal(buttonPushCounter);
  }
  else
  {
    // Do nothing and let it loop again
    
  }
  delay(cycle_delay); // Add a delay to prevent MS to stress
}

int GetSerialData(int nbrbytes){
    starttime = millis(); 
  
  // Start loop to get the 39 bytes
  while ( (Serial.available()<nbrbytes) && ((millis() - starttime) < MAX_WAIT) )
  {
    // Just looping until we have all data
  }
  
  // Verify if we have received 39 bytes
  
  if(Serial.available() < nbrbytes)
  {
    // Less bytes received then expected -> write error on screen
    lcd.setCursor(0,0);
    lcd.print("Error receiving data");
    lcd.setCursor(0,1);
    lcd.print("=(");
    Serial.flush(); // Flush the buffer
    return 0;
  }
  else
  {
    // We received all data - woop woop
    // Read out received data and insert in Array
    for(int n=0; n<nbrbytes; n++)
    {
      Data_In_Bytes[n] = Serial.read();
      Data_In[n] = int(Data_In_Bytes[n]);
      lcd.setCursor(0,0);
      lcd.print(Data_In[n]);
      lcd.setCursor(0,1);
      lcd.print("");
    }
    Serial.flush(); // Flush the buffer
    return 1;
  }
  
}
 
void CalcData()
{
  // Calc data (bytes start counting from 0)
  //
  // byte 0 - clock counter that continuously counts from 0 to 255
  // NOT NEEDED
  int clockcounter = Data_In[0];
  Data_Output[0] = "" + clockcounter;
  Data_Output_desc[0] = "MS Clock Counter";
  
  // byte 1 - squirt
  // NOT NEEDED
  int squirt = Data_In[1];
  Data_Output_desc[1] = "Squirt mode";
  switch (squirt){
   case 0:
    Data_Output[1] = "inj1 squirt";
    break;
   case 1:
    Data_Output[1] = "inj2 squirt";
    break;
   case 2:
    Data_Output[1] = "scheduled to squirt";
    break;
   case 3:
    Data_Output[1] = "squirting";
    break;
   case 4:
    Data_Output[1] = "inj2 sched2";
    break;
   case 5:
    Data_Output[1] = "squirting inj2";
    break;
   case 6:
    Data_Output[1] = "Boost control off";
    break;
  }
  
  // byte 2 - Engine status
    Data_Output_desc[2] = "Engine Status";
    int engine = Data_In[2];
    switch (engine){
   case 0:
    Data_Output[2] = "running";
    break;
   case 1:
    Data_Output[2] = "cranking";
    break;
   case 2:
    Data_Output[2] = "after start enrichment";
    break;
   case 3:
    Data_Output[2] = "in warmup";
    break;
   case 4:
    Data_Output[2] = "in TPS accel mode";
    break;
   case 5:
    Data_Output[2] = "in decel mode";
    break;
   case 6:
    Data_Output[2] = "in MAP accel mode";
    break;
   case 7:
    Data_Output[2] = "idle on";
    break;
  }
  
  // byte 3 - baroADC
  Data_Output_desc[3] = "MAP for baro corr";
  int baroADC = MAPFACTOR(Data_In[3]);
  Data_Output[3] = baroADC + "kPa";
  
  // byte 4 - MAP
  Data_Output_desc[4] = "Current MAP";
  int mapADC = MAPFACTOR(Data_In[4]);
  Data_Output[4] = mapADC + "kPa";

  // byte 5 - matADC
  Data_Output_desc[5] = "Maniford AirTemp";
  int matADC = Data_In[5];
  
  // byte 6 - cltADC
  Data_Output_desc[6] = "Coolant temp";
  //int cltADC = THERMFACTOR(Data_In[6]); // Fahrenheit
  int cltADC = (THERMFACTOR(Data_In[6])-32)/1.8; // Celsius
  Data_Output[6] = cltADC + " °";
  
  // byte 7 - tpsADC
  Data_Output_desc[7] = "Throttle Position";
  int tpsADC = TPSFACTOR(Data_In[7]);
  Data_Output[7] = tpsADC + " %"; 
  
  // byte 8 - batADC
  Data_Output_desc[8] = "Battery Voltage";
  int batADC = Data_In[8] / 255.0 * 30.0;
  Data_Output[8] = batADC + "V";
   
  // byte 9 - egoADC
  Data_Output_desc[9] = "EGO Sensor";
  int egoADC = Data_In[9] / 255.0 * 5.0;
  Data_Output[9] = egoADC + "V";
  
  // byte 10 - egoCorrection
  Data_Output_desc[10] = "EGO correction";
  int egoCorrection = Data_In[10];
  Data_Output[10] = egoCorrection + " %";
  
  
  // byte 11 - airCorrection
  Data_Output_desc[11] = "Air correction";
  int airCorrection = Data_In[11];
  Data_Output[11] = airCorrection + " %";
  
  // byte 12 - warmupEnrich
  Data_Output_desc[12] = "Warmup enrichment";
  int warmupEnrich = Data_In[12];
  Data_Output[12] = warmupEnrich + "%";
  
  
  // byte 13 - RPM
  Data_Output_desc[13] = "Engine RPM";
  int rpm = Data_In[13]*100;
  Data_Output[13] = rpm + "";
  
  // byte 14 - pulsewidth1
  Data_Output_desc[14] = "Pulse Width 1";
  int pulsewidth1 = Data_In[14]/10;
  Data_Output[14] = pulsewidth1 + " ms";
  
  // byte 15 - accelEnrich
  Data_Output_desc[15] = "accelEnrich";
  int accelEnrich = Data_In[15];
  Data_Output[15] = accelEnrich + " ms";
  
  // byte 16 - baroCorrection
  Data_Output_desc[16] = "baroCorrection";
  int baroCorrection = Data_In[16];
  Data_Output[16] = baroCorrection + "%";
  
  // byte 17 - gammaEnrich
  Data_Output_desc[17] = "gammaEnrich";
  int gammaEnrich = Data_In[17];
  Data_Output[17] = gammaEnrich + "%";
  
  // byte 18 - veCurr1
  Data_Output_desc[18] = "Current VE value tbl_1";
  int veCurr1 = Data_In[18];
  Data_Output[18] = veCurr1 + "%";
  
  // byte 19 - pulsewidth2 (in ms)
   Data_Output_desc[19] = "Pulse Width 2";
  int pulsewidth2 = Data_In[19]/10;
  Data_Output[19] = pulsewidth2 + " ms";
   
  // byte 20 - veCurr2
  Data_Output_desc[20] = "Current VE value tbl_2";
  int veCurr2 = Data_In[20];
  Data_Output[20] = veCurr2 + "%";
  
  // byte 21 - idleDC
  Data_Output_desc[21] = "idleDC";
  int idleDC = Data_In[21];
  Data_Output[21] = idleDC + "%";
  
  // byte 22 iTimeH
  Data_Output_desc[22] = "iTimeH";
  int iTimeH = Data_In[22];
  Data_Output[22] = (String)iTimeH;

  // byte 23 iTimeL
  Data_Output_desc[23] = "iTimeL";
  int iTimeL = Data_In[23];
  Data_Output[23] = (String)iTimeL;

  // byte 24 - advSpark
  Data_Output_desc[24] = "Spark Advance";
  int advance = Data_In[24];
  int advSpark = (advance * 0.352)-10;
  Data_Output[24] = advSpark + "°";
  
  // byte 25 - afrTarget (Settings for LC-1)
  Data_Output_desc[25] = "Lambda - AFR";
  int lambda = Data_In[25]/255.0 + 0.5;
  int afr = lambda * 14.7;
  Data_Output[25] = lambda + "        " + afr;
  
  // byte 26 - fuelADC
  Data_Output_desc[26] = "fuelADC";
  int fuelADC = Data_In[26];
  Data_Output[26] = (String)fuelADC;
  
  // byte 27 - egtADC
  Data_Output_desc[27] = "EGT / CHT";
  //int egtADC = Data_In[27]* 7.15625; // Fahrenheit
  int egtADC = Data_In[27]* 3.90625; // Celsius
  Data_Output[27] = egtADC + "°";
    
  // byte 28 - CltIatAngle
  Data_Output_desc[28] = "CLT-IAT Angle";
  int CltIatAngle = Data_In[28]*0.352;
  Data_Output[28] = CltIatAngle + "°";
  
  // byte 29 - KnockAngle
  Data_Output_desc[29] = "Knock Angle";
  int KnockAngle = Data_In[29];
  Data_Output[29] = KnockAngle + "°";
  
  // byte 30 - egoCorrection2
  Data_Output_desc[30] = "EGO correction 2";
  int egoCorrection2 = Data_In[30];
  Data_Output[30] = egoCorrection2 + " %";
    
  // byte 31 - porta
  Data_Output_desc[31] = "Port A";
  int porta = Data_In[31];
  switch (porta){
   case 0:
    Data_Output[31] = "Fuel Pump on";
    break;
   case 1:
    Data_Output[31] = "Fidle/Spark On";
    break;
   case 2:
    Data_Output[31] = "Output 2 On (X5)";
    break;
   case 3:
    Data_Output[31] = "Output 1/Boost Ctrl On (X4)";
    break;
   case 4:
    Data_Output[31] = "NOS/w Inj Pulsing On (X3)";
    break;
   case 5:
    Data_Output[31] = "Fan/w Inj On (X2)";
    break;
   case 6:
    Data_Output[31] = "Flyback";
    break;
   case 7:
    Data_Output[31] = "Flyback";
    break;
  }
  
  // byte 32 - portb
  Data_Output_desc[32] = "Port B";
  int portb = Data_In[32];
  switch (portb){
   case 0:
    Data_Output[32] = "MAP";
    break;
   case 1:
    Data_Output[32] = "MAT";
    break;
   case 2:
    Data_Output[32] = "CLT";
    break;
   case 3:
    Data_Output[32] = "TPS";
    break;
   case 4:
    Data_Output[32] = "BAT";
    break;
   case 5:
    Data_Output[32] = "EGO";
    break;
   case 6:
    Data_Output[32] = "X7 Spare - EGO2/MAP2/FuelPSI";
    break;
   case 7:
    Data_Output[32] = "X6 Spare - EGT";
    break;  
  }  
 
  // byte 33 - portc
  Data_Output_desc[33] = "Port C";
  int portc = Data_In[33];
   switch (portc){
   case 0:
    Data_Output[33] = "Squirt LED/Coil A";
    break;
   case 1:
    Data_Output[33] = "Accel LED/Coil B";
    break;
   case 2:
    Data_Output[33] = "Fan/Output 4/Coil C on";
    break;
   case 3:
    Data_Output[33] = "Multiplexed Shift/Coil E";
    break;
   case 4:
    Data_Output[33] = "Light Outputs/2nd trig input";
    break;
  } 
  
  // byte 34 - portd
  Data_Output_desc[34] = "Port D";
  int portd = Data_In[34];
  switch (portd){
   case 0:
    Data_Output[34] = "Unused/Coil D";
    break;
   case 1:
    Data_Output[34] = "NOS/Tables off";
    break;
   case 2:
    Data_Output[34] = "No Knock";
    break;
   case 3:
    Data_Output[34] = "Launch Off";
    break;
   case 4:
    Data_Output[34] = "Inj1";
    break;
   case 5:
    Data_Output[34] = "Inj2/Electric Fan Output";
    break;
  } 
  
  // byte 35 - stackL
  Data_Output_desc[35] = "CPU stack";
  int stackL = Data_In[35];
  Data_Output[35] = (byte)stackL + "";
  
  // byte 36 - tpsLast
  Data_Output_desc[36] = "TPS/MAP last value for MT Accel Wizard";
  int tpsLast = Data_In[36];
  Data_Output[36] = (String)tpsLast;
  
  // byte 37 - iTimeX
  Data_Output_desc[37] = "interval Time";
  int iTimeX = Data_In[37];
  Data_Output[37] = iTimeX + "s";
  
  // byte 38 - bcdc
  Data_Output_desc[38] = "bcdc";
  int bcdc = Data_In[38];
  Data_Output[38] = bcdc + "%";
}

void buttonPoller(){
  // read the pushbutton input pin:
  buttonState = digitalRead(buttonPin);

  // compare the buttonState to its previous state
  if (buttonState != lastButtonState) {
    // if the state has changed, increment the counter
    if (buttonState == HIGH) {
      // if the current state is HIGH then the button
      // wend from off to on:
      buttonPushCounter++;
      if (buttonPushCounter == buttonPushMax){ buttonPushCounter = 0; }
    }
    else {
      // if the current state is LOW then the button
      // wend from on to off:
     }
  }
  // save the current state as the last state,
  //for next time through the loop
  lastButtonState = buttonState;
}

void DisplayVal(int dsp)
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(Data_Output_desc[dsp]);
  lcd.setCursor(0,1);
  lcd.print(Data_Output[dsp]);
}
  

// THERM FACTOR VDO 150° CLT
int THERMFACTOR(int val){
  int thermfactor[] = {99999, 366.10, 299.25, 265.32, 243.19, 227.03, 214.42, 204.15, 195.53, 188.11, 181.64, 175.89, 170.74, 166.08, 161.82, 157.91, 154.29, 150.93, 147.80, 144.86, 142.10, 139.49, 137.02, 134.67, 132.45, 130.32, 128.29, 126.34, 124.48, 122.69, 120.97, 119.30, 117.70, 116.16, 114.66, 113.21, 111.81, 110.44, 109.12, 107.84, 106.59, 105.37, 104.18, 103.03, 101.90, 100.80, 99.72, 98.67, 97.64, 96.63, 95.65, 94.68, 93.74, 92.81, 91.90, 91.00, 90.13, 89.27, 88.42, 87.59, 86.77, 85.96, 85.17, 84.39, 83.62, 82.86, 82.11, 81.38, 80.65, 79.94, 79.23, 78.53, 77.84, 77.16, 76.49, 75.83, 75.17, 74.53, 73.89, 73.25, 72.63, 72.01, 71.40, 70.79, 70.19, 69.59, 69.00, 68.42, 67.84, 67.27, 66.70, 66.14, 65.58, 65.03, 64.48, 63.94, 63.40, 62.86, 62.33, 61.80, 61.28, 60.76, 60.24, 59.73, 59.22, 58.72, 58.21, 57.71, 57.22, 56.72, 56.23, 55.74, 55.26, 54.78, 54.30, 53.82, 53.34, 52.87, 52.40, 51.93, 51.47, 51.00, 50.54, 50.08, 49.62, 49.16, 48.71, 48.26, 47.80, 47.35, 46.91, 46.46, 46.01, 45.57, 45.13, 44.68, 44.24, 43.80, 43.36, 42.93, 42.49, 42.05, 41.62, 41.18, 40.75, 40.32, 39.88, 39.45, 39.02, 38.59, 38.16, 37.73, 37.30, 36.87, 36.44, 36.01, 35.58, 35.15, 34.72, 34.29, 33.87, 33.44, 33.01, 32.58, 32.15, 31.71, 31.28, 30.85, 30.42, 29.99, 29.55, 29.12, 28.68, 28.24, 27.81, 27.37, 26.93, 26.49, 26.05, 25.60, 25.16, 24.71, 24.26, 23.81, 23.36, 22.91, 22.46, 22.00, 21.54, 21.08, 20.61, 20.15, 19.68, 19.21, 18.74, 18.26, 17.78, 17.30, 16.81, 16.32, 15.83, 15.33, 14.83, 14.33, 13.82, 13.30, 12.79, 12.26, 11.73, 11.20, 10.66, 10.12, 9.57, 9.01, 8.45, 7.88, 7.30, 6.72, 6.12, 5.52, 4.91, 4.29, 3.66, 3.02, 2.37, 1.71, 1.03, .34, -.36, -1.08, -1.81, -2.56, -3.33, -4.12, -4.93, -5.76, -6.62, -7.50, -8.42, -9.36, -10.35, -11.37, -12.44, -13.55, -14.73, -15.97, -17.28, -18.67, -20.18, -21.80, -23.57, -25.54, -27.76, -30.32, -33.39, 99999};
  return thermfactor[val];
}

// MPX4250 (default MS1 v2.2 MAP)
int MAPFACTOR(int val){
  int mapfactor[] = {100, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255, 255, 255, 255, 255, 100};
  return mapfactor[val];
}

// Bosch-Siemens IAT
int MATFACTOR(int val){
  int matfactor[] = {99999, 657.73, 510.25, 440.97, 397.60, 366.76, 343.15, 324.22, 308.50, 295.14, 283.56, 273.37, 264.29, 256.12, 248.70, 241.92, 235.67, 229.90, 224.52, 219.50, 214.79, 210.36, 206.18, 202.22, 198.46, 194.89, 191.48, 188.22, 185.10, 182.11, 179.24, 176.48, 173.82, 171.26, 168.78, 166.38, 164.06, 161.82, 159.64, 157.52, 155.46, 153.46, 151.52, 149.62, 147.78, 145.97, 144.22, 142.50, 140.82, 139.18, 137.57, 136.00, 134.46, 132.95, 131.47, 130.02, 128.60, 127.20, 125.82, 124.48, 123.15, 121.85, 120.56, 119.30, 118.06, 116.83, 115.63, 114.44, 113.27, 112.12, 110.98, 109.86, 108.75, 107.65, 106.57, 105.51, 104.45, 103.41, 102.38, 101.37, 100.36, 99.37, 98.38, 97.41, 96.45, 95.49, 94.55, 93.61, 92.69, 91.77, 90.86, 89.96, 89.07, 88.18, 87.31, 86.43, 85.57, 84.71, 83.86, 83.02, 82.18, 81.35, 80.53, 79.71, 78.89, 78.09, 77.28, 76.48, 75.69, 74.90, 74.12, 73.34, 72.56, 71.79, 71.03, 70.27, 69.51, 68.75, 68.00, 67.25, 66.51, 65.77, 65.03, 64.29, 63.56, 62.83, 62.11, 61.38, 60.66, 59.94, 59.23, 58.51, 57.80, 57.09, 56.38, 55.68, 54.97, 54.27, 53.57, 52.87, 52.17, 51.48, 50.78, 50.09, 49.39, 48.70, 48.01, 47.32, 46.63, 45.94, 45.25, 44.56, 43.87, 43.19, 42.50, 41.81, 41.12, 40.44, 39.75, 39.06, 38.37, 37.68, 36.99, 36.30, 35.61, 34.92, 34.22, 33.53, 32.84, 32.14, 31.44, 30.74, 30.04, 29.34, 28.63, 27.92, 27.22, 26.50, 25.79, 25.08, 24.36, 23.64, 22.91, 22.18, 21.45, 20.72, 19.98, 19.24, 18.50, 17.75, 16.99, 16.24, 15.47, 14.71, 13.93, 13.16, 12.37, 11.58, 10.79, 9.99, 9.18, 8.36, 7.54, 6.71, 5.88, 5.03, 4.18, 3.31, 2.44, 1.56, .67, -.24, -1.15, -2.08, -3.02, -3.97, -4.93, -5.92, -6.91, -7.92, -8.95, -10.00, -11.07, -12.15, -13.26, -14.39, -15.55, -16.73, -17.94, -19.18, -20.46, -21.76, -23.11, -24.50, -25.93, -27.41, -28.94, -30.53, -32.18, -33.91, -35.72, -37.61, -39.62, -41.73, -43.99, -46.41, -49.01, -51.85, -54.98, -58.48, -62.47, -67.15, -72.86, -80.37, -91.88, 99999};
  return matfactor[val];
}

// Default MS values
int TPSFACTOR(int val){
  int tpsfactor[] = {0, 0, 0, 1, 1, 1, 2, 2, 3, 3, 3, 4, 4, 5, 5, 5, 6, 6, 7, 7, 7, 8, 8, 9, 9, 9, 10, 10, 10, 11, 11, 12, 12, 12, 13, 13, 14, 14, 14, 15, 15, 16, 16, 16, 17, 17, 18, 18, 18, 19, 19, 20, 20, 20, 21, 21, 21, 22, 22, 23, 23, 23, 24, 24, 25, 25, 25, 26, 26, 27, 27, 27, 28, 28, 29, 29, 29, 30, 30, 30, 31, 31, 32, 32, 32, 33, 33, 34, 34, 34, 35, 35, 36, 36, 36, 37, 37, 38, 38, 38, 39, 39, 40, 40, 40, 41, 41, 41, 42, 42, 43, 43, 43, 44, 44, 45, 45, 45, 46, 46, 47, 47, 47, 48, 48, 49, 49, 49, 50, 50, 50, 51, 51, 52, 52, 52, 53, 53, 54, 54, 54, 55, 55, 56, 56, 56, 57, 57, 58, 58, 58, 59, 59, 60, 60, 60, 61, 61, 61, 62, 62, 63, 63, 63, 64, 64, 65, 65, 65, 66, 66, 67, 67, 67, 68, 68, 69, 69, 69, 70, 70, 70, 71, 71, 72, 72, 72, 73, 73, 74, 74, 74, 75, 75, 76, 76, 76, 77, 77, 78, 78, 78, 79, 79, 80, 80, 80, 81, 81, 81, 82, 82, 83, 83, 83, 84, 84, 85, 85, 85, 86, 86, 87, 87, 87, 88, 88, 89, 89, 89, 90, 90, 90, 91, 91, 92, 92, 92, 93, 93, 94, 94, 94, 95, 95, 96, 96, 96, 97, 97, 98, 98, 98, 99, 99, 100};
  return tpsfactor[val];
}
