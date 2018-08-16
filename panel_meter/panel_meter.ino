//Author: Thomas Savage
//Date: 13 August 2018
//Subject: RDE research; Pressure Transducer Display
//Version 1.8.5
//This program reads an analog input and allows the user to display a result in the form of a P1_pressure
#include <LiquidCrystal.h>

const int rs = 12, enable = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, enable, d4, d5, d6, d7);
const int button1 = 6, button2 = 7, button3 = 8, button4 = 9, button5 = 10;

//Names to be used in conjunction with buttons in later programs. These are the states of the button as determined by the debounce() routine. Some buttons have more than one name.
int menu = 0;
int up = 0;
int down = 0;
int enter = 0;
int side = 0;
int tare = 0;

//Used by debounce() to determine if the button has been released
int previous_button1_state = 0;
int previous_button2_state = 0;
int previous_button3_state = 0;
int previous_button4_state = 0;
int previous_button5_state = 0;

//Values used by View();
int last_view = 0;
int choice = 0; //The option the user chose. Used by Menu() to execute routines
int view = 0; // Used by Menu() to display appropriate text to screen

//Values used by Input_Setup()
float to_add = 0.01;
float P1_lower_voltage = 0.5;// Also used by Pressure()
float P1_upper_voltage = 4.5;// Also used by Pressure()
float P2_lower_voltage = 0.5;
float P2_upper_voltage = 4.5;
int CURSOR = 15; //Also used by Output()

//Values used by Output()
int P1_Pressure_range = 15;// Also used by Presssure()
int P2_Pressure_range = 30;
int add = 1000;

//Values used by Pressure()
int P1_Signal_In = 1;
int P2_Signal_In = 0;
int P1_Data = 0;
int P2_Data = 0;
float P1_pressure = 0;
float P2_pressure = 0;
float P1_upper_integer = 0;
float P2_upper_integer = 0;
float P1_lower_integer = 0;
float P2_lower_integer = 0;
float P1_slope = 0;
float P2_slope = 0;
int previous_tare = 0;


void setup() 
{
  lcd.begin(16,2);
  pinMode(button1,INPUT);
  pinMode(button2, INPUT);
  pinMode(button3, INPUT);
  pinMode(button4, INPUT);
  pinMode(button5, INPUT);
  pinMode(P1_Signal_In, INPUT);
  pinMode(P2_Signal_In, INPUT);
}

int debounce(int button, int &previous_button_state) // previous_button_state is referenced so that the information isn't lost. Is referenced later by the code
{
//button is the pin number to look at
//previous_button_state is what LOOP() thinks the state of the button is
  int current_button_state;
  
  current_button_state = digitalRead(button);
  
  if(current_button_state == 1 && previous_button_state == 0)
  {
    previous_button_state = 1;
    return 1;
  }
  if(current_button_state == previous_button_state)
  {
    previous_button_state = current_button_state;
    return 0;
  }
  if(current_button_state == 0)
  {
    previous_button_state = 0;
    return 0;
  }
}

float conversion = 1;          //Will assume everything to be in psi
int unit_types = 0;
int previous_unit_types = 0;   //Global so that it doesn't change. I need this property 
                               //as I need to compare what I had to what I have now         
                               //selected so that I don't the conversion multiply twice.
float previous_conversion = 1;

void units()
{
menu = 0;
enter = 0;
while(enter == 0)
  {
  enter = debounce(button4, previous_button4_state);
  up = debounce(button2, previous_button2_state);
  down = debounce(button3, previous_button3_state);
/*deals with the wrapping around of options*/
  if(unit_types > 2)
    {
    unit_types = 0;
    }
  if(unit_types < 0)
    {
    unit_types = 2;
    }
  
  if(up == 1)
    {
    unit_types++;
    }
  if(down == 1)
    {
    unit_types--;
    }
  lcd.setCursor(0,0);
  switch(unit_types)
    {
    case 0:
    lcd.print("psi");
      break;
    case 1:
    lcd.print("kPa");
      break;
    case 2:
    lcd.print("Pa ");
      break;
    }
  }
switch(unit_types)
  {
  case 0:
  conversion = 1;
    break;
  case 1:
  conversion = 6.89476;
    break;
  case 2:
  conversion = 6894.76;
    break;
  }
if(unit_types != previous_unit_types)
  {
  P1_Pressure_range = (P1_Pressure_range*conversion)/previous_conversion;
  P2_Pressure_range = (P2_Pressure_range*conversion)/previous_conversion;
  previous_unit_types = unit_types;
  previous_conversion = conversion; //Order of this statement in the code is important.
  }
}

void Input_Setup()
{
  int count = 0; //Deals with blinking of cursor
  enter = 0;
  to_add = 0.01;
  CURSOR = 15;
  int ROW = 0;
  int row_change = 0;

  lcd.setCursor(0,0);
  lcd.print("P1");
  lcd.setCursor(5,0);
  lcd.print(P1_lower_voltage);
  lcd.print(" - ");
  lcd.print(P1_upper_voltage);
  lcd.setCursor(0,1);
  lcd.print("P2");
  lcd.setCursor(5, 1);
  lcd.print(P2_lower_voltage);
  lcd.print(" - ");
  lcd.print(P2_upper_voltage);

  while(enter == 0)
  {
    enter = debounce(button1, previous_button1_state);
    up = debounce(button2, previous_button2_state);
    down = debounce(button3, previous_button3_state);
    side = debounce(button4, previous_button4_state);
    row_change = debounce(button5, previous_button5_state);

    if(side == 1)
    {
      to_add = to_add*10;
      CURSOR--;
    }
    if(CURSOR == 11) //Forces cursor to jump dash
    {
      CURSOR = 8;
      to_add = 0.01;
    }
    if(CURSOR == 4) //Forces cursor to loop back
    {
      CURSOR = 15;
      to_add = 0.01;
    }
    if(CURSOR == 13)  //Deals with visual cue being under decimal point
    {
      CURSOR = 12;
    }

    if(CURSOR == 6) //Deals with visual cue being under decimal point
    {
      CURSOR = 5;
    }

    if(row_change == 1)
    {
    ROW++;
    }

    if(ROW > 2)
    {
    ROW = 0;
    }

    if(ROW < 0)
    {
    ROW = 1;
    }

    if(CURSOR <= 8 && CURSOR >= 5 && ROW == 0)
    { 
      if(up == 1)
      {    
      P1_lower_voltage = P1_lower_voltage + to_add;
      }
      if(down == 1)
      {
        P1_lower_voltage = P1_lower_voltage - to_add;
      }
      if(P1_lower_voltage > 5)
      {
        P1_lower_voltage = 0;
      }
      
    }
    if(CURSOR <= 15 && CURSOR >= 12 && ROW == 0)
    {
      if(up == 1)
      {
      P1_upper_voltage = P1_upper_voltage + to_add;
      }
      if(down == 1)
      {
        P1_upper_voltage = P1_upper_voltage - to_add;
      }
      if(P1_upper_voltage > 5)
      {
        P1_upper_voltage = 0;
      }
    }

    if(CURSOR <= 8 && CURSOR >= 5 && ROW == 1)
    { 
      if(up == 1)
      {    
      P2_lower_voltage = P2_lower_voltage + to_add;
      }
      if(down == 1)
      {
        P2_lower_voltage = P2_lower_voltage - to_add;
      }
      if(P2_lower_voltage > 5)
      {
        P2_lower_voltage = 0;
      }
      
    }
    if(CURSOR <= 15 && CURSOR >= 12 && ROW == 1)
    {
      if(up == 1)
      {
      P2_upper_voltage = P2_upper_voltage + to_add;
      }
      if(down == 1)
      {
        P2_upper_voltage = P2_upper_voltage - to_add;
      }
      if(P2_upper_voltage > 5)
      {
        P2_upper_voltage = 0;
      }
    }

    count ++;
    if(up == 1 || down == 1)
    {
      lcd.setCursor(5,0);
      lcd.print(P1_lower_voltage);
      lcd.print(" - ");
      lcd.print(P1_upper_voltage);

      lcd.setCursor(5, 1);
      lcd.print(P2_lower_voltage);
      lcd.print(" - ");
      lcd.print(P2_upper_voltage);
    }
    lcd.setCursor(CURSOR, ROW);
    if(count < 500)
    {
      lcd.blink();
    }
    if(count == 1000)
    {
      lcd.noBlink();
      count = 0;
    }
  }
  lcd.clear();
}
  
void Output()
{
  lcd.clear();
  enter = 0;
  add = 1000;
  int row_change = 0;
  int ROW = 0;
  int change_add = 0;
  int count = 0;

    lcd.setCursor(0, 0);
    lcd.print("P1:");
    lcd.print(P1_Pressure_range);
    lcd.print("+");
    lcd.print(add);
    lcd.print(" ");
    switch (unit_types)
      {
      case 0:
      lcd.print("psi");
        break;
      case 1:
      lcd.print("kPa");
        break;
      case 2:
      lcd.print("Pa");
        break;
      }

    lcd.setCursor(0,1);
    lcd.print("P2:");
    lcd.print(P2_Pressure_range);
    lcd.print("+");
    lcd.print(add);
    lcd.print(" ");
    switch (unit_types)
      {
      case 0:
      lcd.print("psi");
        break;
      case 1:
      lcd.print("kPa");
        break;
      case 2:
      lcd.print("Pa");
        break;
      }
    

  while(enter == 0)
  {
    enter = debounce(button1, previous_button1_state);
    up = debounce(button2, previous_button2_state);
    down = debounce(button3, previous_button3_state);
    change_add = debounce(button4, previous_button4_state);
    row_change = debounce(button5, previous_button5_state);
    
    if(row_change == 1)
    {
    ROW++;
    }
    if(ROW > 1)
    {
    ROW = 0;
    }

    if(ROW == 0)
    {
        if(change_add == 1)
        {
          add = add/10;
        }
        if(add == 0)
        {
          add = 1000;
        }
        if(up == 1)
        {
          P1_Pressure_range = P1_Pressure_range + add;
        }
        if(down == 1)
        {
          P1_Pressure_range = P1_Pressure_range - add;
        }
    }

    if(ROW == 1)
    {
        if(change_add == 1)
        {
          add = add/10;
        }
        if(add == 0)
        {
          add = 1000;
        }
        if(up == 1)
        {
          P2_Pressure_range = P2_Pressure_range + add;
        }
        if(down == 1)
        {
          P2_Pressure_range = P2_Pressure_range - add;
        }
    }

    if(up == 1 || down == 1 || change_add == 1)
    {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("P1:");
      lcd.print(P1_Pressure_range);
      lcd.print("+");
      lcd.print(add);
      lcd.print(" ");
      switch (unit_types)
        {
        case 0:
        lcd.print("psi");
          break;
        case 1:
        lcd.print("kPa");
          break;
        case 2:
        lcd.print("Pa");
          break;
        }
  
      lcd.setCursor(0,1);
      lcd.print("P2:");
      lcd.print(P2_Pressure_range);
      lcd.print("+");
      lcd.print(add);
      lcd.print(" ");
      switch (unit_types)
        {
        case 0:
        lcd.print("psi");
          break;
        case 1:
        lcd.print("kPa");
          break;
        case 2:
        lcd.print("Pa");
          break;
        } 
    }

    lcd.setCursor(0, ROW);
    if(count < 500)
    {
    lcd.blink();
    }
    if(count > 500)
    {
    lcd.noBlink();
    }
    if(count == 1000)
    {
    count = 0;
    }

  }
  lcd.clear();
  
}

void Show(int &choice)
{
  switch(choice)
    {
    case 0:
    lcd.print("Version 1.8.5");
    while(menu != 1) //For some reason the condition menu = 0 doesn't work. It seems that menu can never truly be 0
      {
        menu = debounce(button1, previous_button1_state);
      }
    break;
    case 1:
    Input_Setup();
    break;
    case 2:
    Pressure();
    break;
    case 3:
    Output();
    break;
    case 4:
    units();
    break;
    }
}

void View(int option, int &last_option)
{
  if(last_option != option)
  {
    lcd.clear();
  }
  lcd.setCursor(0,0);
  switch(option)
  {
    case 0:
    lcd.print("Display Version");
      break;
    case 1:
    lcd.print("Set up Input");
      break;
    case 2:
    lcd.print("Read Pressure");
    break;
    case 3:
    lcd.print("Set up output");
    break;
    case 4:
    lcd.print("Set units");
    break;
  }
}

void Pressure()
{
  lcd.clear();
  lcd.setCursor(0,0);
  menu = 0;
  tare = 0;
  P1_upper_integer = P1_upper_voltage*204.6;
  P1_lower_integer = P1_lower_voltage*204.6;
  P2_upper_integer = P2_upper_voltage*204.6;
  P2_lower_integer = P2_lower_voltage*204.6;
  P1_slope = (P1_Pressure_range)/(P1_upper_integer - P1_lower_integer);
  P2_slope = (P2_Pressure_range)/(P2_upper_integer - P2_lower_integer);
  while(menu == 0)
  {
    menu = debounce(button1, previous_button1_state);
    P1_Data = analogRead(P1_Signal_In);
    delay(100);
    P2_Data = analogRead(P2_Signal_In);
    P1_pressure = (P1_slope*P1_Data) - (P1_slope*P1_lower_integer);
    P2_pressure = (P2_slope*P2_Data) - (P2_slope*P2_lower_integer);
    /*tare = debounce(button5, previous_button5_state);
    if(tare == 1 && previous_tare == 0)
    {
      P1_lower_integer = P1_Data;
      previous_tare = 1;
    }
    else if(tare == 1 && previous_tare == 1)
    {
      P1_lower_integer = P1_lower_voltage*204.6;
      previous_tare = 0;
    }*/
    lcd.setCursor(0,0);
    lcd.print(P1_pressure);
    switch (unit_types)
      {
      case 0:
      lcd.print("psi    ");
        break;
      case 1:
      lcd.print("kPa    ");
        break;
      case 2:
      lcd.print("Pa    ");
        break;
      }
     lcd.print(P1_Data);
     lcd.print("  ");

    lcd.setCursor(0,1);
    lcd.print(P2_pressure);
    switch (unit_types)
      {
      case 0:
      lcd.print("psi    ");
        break;
      case 1:
      lcd.print("kPa    ");
        break;
      case 2:
      lcd.print("Pa    ");
        break;
      }
     lcd.print(P2_Data);
     lcd.print("  ");
     
     delay(100); //To prevent a flickering screen
  }
  lcd.clear();
}

 

void loop() 
{
  lcd.setCursor(0,0);
  view = 0;
  last_view = 0;
  menu = 0;
  enter = 0; 
    while(enter == 0)
    {
      up = debounce(button2, previous_button2_state);
      down = debounce(button3, previous_button3_state);
      enter = debounce(button4, previous_button4_state);
        if(up == 1)
        {
          view--;
          if(view < 0)
          {
            view = 4;
          }
        }
        if(down == 1)
        {
          view++;
          if(view > 4)
          {
            view = 0;
          }
        }
      View(view, last_view);
      last_view = view;
      if(enter == 1)
      {
        choice = view;
        lcd.clear();
        Show(choice);
      }
    }        
}

