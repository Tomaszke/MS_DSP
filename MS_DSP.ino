int result;
int intomingByte = 0;
int MAX_WAIT = 1000;
unsigned long starttime;
byte Data_In[39];
int thermfactor[255];


// intlude the LCD library code:
#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2); // initialize the LCD-library with the numbers of the interface pins

void setup() {
    Serial.begin(9600); // opens serial port, sets data rate to 9600 bps
    lcd.begin(16, 2); // Set the LCD as 16x2
}

void loop(){
  Serial.print("R"); // Send command to read realtime values (output 39 bytes)
  result = GetSerialData();
  if(result=1) // Hop to function to get the 39 bytes of data
  {
    CalcData(); // Hop to function to calculate the received bytes
  }
  else
  {
    // Do nothing and let it loop again
    
  }
  delay(200); // Add a 200ms delay to prevent MS to stress
}

int GetSerialData(){
    starttime = millis(); 
  
  // Start loop to get the 39 bytes
  while ( (Serial.available()<39) && ((millis() - starttime) < MAX_WAIT) )
  {
    // Just looping until we have all data
  }
  
  // Verify if we have received 39 bytes
  
  if(Serial.available() < 39)
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
    for(int n=0; n<39; n++)
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
  
  // byte 1 - squirt
  // NOT NEEDED
  int squirt = Data_In[1];
  
  // byte 2 - Engine status
  // NOT NEEDED
  int engine = Data_In[2];
  
  // byte 3 - baroADC
  int baroADC = Data_In[3];
  
  // byte 4 - MAP
  int mapADC = Data_In[4];
  
  // byte 5 - matADC
  int matADC = Data_In[5];
  
  // byte 6 - cltADC
  int cltADC = THERMFACTOR(Data_In[6]);
 
  
  // byte 7 - tpsADC
  int tpsADC = Data_In[7];
  
  // byte 8 - batADC
  int batADC = Data_In[8] / 255.0 * 30.0;
   
  // byte 9 - egoADC
  int egoADC = Data_In[9];
  
  // byte 10 - egoCorrection
  int egoCorrection = Data_In[10];
  
  // byte 11 - airCorrection
  int airCorrection = Data_In[11];
  
  // byte 12 - warmupEnrich
  int warmupEnrich = Data_In[12];
  
  // byte 13 - RPM
  int rpm = Data_In[13]*100;
  
  // byte 14 - pulsewidth1
  int pulsewidth1 = Data_In[14];
  
  // byte 15 - accelEnrich
  int accelEnrich = Data_In[15];
  
  // byte 16 - baroCorrection
  int baroCorrection = Data_In[16];
  
  // byte 17 - gammaEnrich
  int gammaEnrich = Data_In[17];
  
  // byte 18 - veCurr1
  int veCurr1 = Data_In[18];
  
  // byte 19 - pulsewidth2 (in ms)
  int pulsewidth2 = Data_In[19]/10;
  
  // byte 20 - veCurr2
  int veCurr2 = Data_In[20];
  
  // byte 21 - idleDC
  int idleDC = Data_In[21];
  
  // byte 22-23 NOT NEEDED

  // byte 24 - advSpark
  int advance = Data_In[24];
  int advSpark = (advance * 0.352)-10;
  
  // byte 25 - afrTarget
  int afrTarget = Data_In[25];
  
  // byte 26 - fuelADC
  int fuelADC = Data_In[26];
  
  // byte 27 - egtADC
  int egtADC = Data_In[27]* 3.90625;
    
  // byte 28 - CltAngle
  int CltIatAngle = Data_In[28];
  
  // byte 29 - KnockAngle
  int KnockAngle = Data_In[29];
  
  // byte 30 - egoCorrection2
  int egoCorrection2 = Data_In[30];
  
  // byte 31 - porta
  int porta = Data_In[31];
  
  // byte 32 - portb
  int portb = Data_In[32];
  
  // byte 33 - portc
  int portc = Data_In[33];
  
  // byte 34 - portd
  int portd = Data_In[34];
  
  // byte 35 - stackL
  int stackL = Data_In[35];
  
  // byte 36 - tpsLast
  int tpsLast = Data_In[36];
  
  // byte 37 - iTimeX
  int iTimeX = Data_In[37];
  
  // byte 38 - bcdc
  int bcdc = Data_In[38];
  
} 

int THERMFACTOR(int val){
  int thermfactor[] = {99999, 366.10, 299.25, 265.32, 243.19, 227.03, 214.42, 204.15, 195.53, 188.11, 181.64, 175.89, 170.74, 166.08, 161.82, 157.91, 154.29, 150.93, 147.80, 144.86, 142.10, 139.49, 137.02, 134.67, 132.45, 130.32, 128.29, 126.34, 124.48, 122.69, 120.97, 119.30, 117.70, 116.16, 114.66, 113.21, 111.81, 110.44, 109.12, 107.84, 106.59, 105.37, 104.18, 103.03, 101.90, 100.80, 99.72, 98.67, 97.64, 96.63, 95.65, 94.68, 93.74, 92.81, 91.90, 91.00, 90.13, 89.27, 88.42, 87.59, 86.77, 85.96, 85.17, 84.39, 83.62, 82.86, 82.11, 81.38, 80.65, 79.94, 79.23, 78.53, 77.84, 77.16, 76.49, 75.83, 75.17, 74.53, 73.89, 73.25, 72.63, 72.01, 71.40, 70.79, 70.19, 69.59, 69.00, 68.42, 67.84, 67.27, 66.70, 66.14, 65.58, 65.03, 64.48, 63.94, 63.40, 62.86, 62.33, 61.80, 61.28, 60.76, 60.24, 59.73, 59.22, 58.72, 58.21, 57.71, 57.22, 56.72, 56.23, 55.74, 55.26, 54.78, 54.30, 53.82, 53.34, 52.87, 52.40, 51.93, 51.47, 51.00, 50.54, 50.08, 49.62, 49.16, 48.71, 48.26, 47.80, 47.35, 46.91, 46.46, 46.01, 45.57, 45.13, 44.68, 44.24, 43.80, 43.36, 42.93, 42.49, 42.05, 41.62, 41.18, 40.75, 40.32, 39.88, 39.45, 39.02, 38.59, 38.16, 37.73, 37.30, 36.87, 36.44, 36.01, 35.58, 35.15, 34.72, 34.29, 33.87, 33.44, 33.01, 32.58, 32.15, 31.71, 31.28, 30.85, 30.42, 29.99, 29.55, 29.12, 28.68, 28.24, 27.81, 27.37, 26.93, 26.49, 26.05, 25.60, 25.16, 24.71, 24.26, 23.81, 23.36, 22.91, 22.46, 22.00, 21.54, 21.08, 20.61, 20.15, 19.68, 19.21, 18.74, 18.26, 17.78, 17.30, 16.81, 16.32, 15.83, 15.33, 14.83, 14.33, 13.82, 13.30, 12.79, 12.26, 11.73, 11.20, 10.66, 10.12, 9.57, 9.01, 8.45, 7.88, 7.30, 6.72, 6.12, 5.52, 4.91, 4.29, 3.66, 3.02, 2.37, 1.71, 1.03, .34, -.36, -1.08, -1.81, -2.56, -3.33, -4.12, -4.93, -5.76, -6.62, -7.50, -8.42, -9.36, -10.35, -11.37, -12.44, -13.55, -14.73, -15.97, -17.28, -18.67, -20.18, -21.80, -23.57, -25.54, -27.76, -30.32, -33.39, 99999};
  return thermfactor[val];
}
