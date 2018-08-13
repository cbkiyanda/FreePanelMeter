//Author: Thomas Savage
//Date: 13 August 2018
//Subject: RDE research; Pressure Transducer Display
//Version 1.8 (Adding cursor to Input Setup)
//This program reads an analog input and allows the user to display a result in the form of a pressure
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
float lower_voltage = 0.5;// Also used by Pressure()
float upper_voltage = 4.5;// Also used by Pressure()
int CURSOR = 15; //Also used by Output()

//Values used by Output()
int Pressure_range = 50;// Also used by Presssure()
int add = 1000;

//Values used by Pressure()
int Signal_In = 1;
int Data = 0;
float pressure = 0;
float upper_integer = 0;
float lower_integer = 0;
float slope = 0;
int previous_tare = 0;


void setup() 
{
  lcd.begin(16,2);
  pinMode(button1,INPUT);
  pinMode(button2, INPUT);
  pinMode(button3, INPUT);
  pinMode(button4, INPUT);
  pinMode(button5, INPUT);
  pinMode(Signal_In, INPUT);
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
  Pressure_range = (Pressure_range*conversion)/previous_conversion;
  previous_unit_types = unit_types;
  previous_conversion = conversion; //Order of this statement in the code is important.
  }
}

void Input_Setup()
{
  int count = 0; //Deals with blinking of cursor
  menu = 0;
  enter = 0;
  to_add = 0.01;
  CURSOR = 15;
  lcd.setCursor(5,0);
  lcd.print(lower_voltage);
  lcd.print(" - ");
  lcd.print(upper_voltage);
  while(enter == 0)
  {
    enter = debounce(button4, previous_button4_state);
    up = debounce(button2, previous_button2_state);
    down = debounce(button3, previous_button3_state);
    side = debounce(button5, previous_button5_state);

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

    if(CURSOR <= 8 && CURSOR >= 5)
    { 
      if(up == 1)
      {    
      lower_voltage = lower_voltage + to_add;
      }
      if(down == 1)
      {
        lower_voltage = lower_voltage - to_add;
      }
      if(lower_voltage > 5)
      {
        lower_voltage = 0;
      }
      
    }
    if(CURSOR <= 15 && CURSOR >= 12)
    {
      if(up == 1)
      {
      upper_voltage = upper_voltage + to_add;
      }
      if(down == 1)
      {
        upper_voltage = upper_voltage - to_add;
      }
      if(upper_voltage > 5)
      {
        upper_voltage = 0;
      }
    }
    count ++;
    if(up == 1 || down == 1)
    {
      lcd.setCursor(5,0);
      lcd.print(lower_voltage);
      lcd.print(" - ");
      lcd.print(upper_voltage);
    }
    lcd.setCursor(CURSOR, 0);
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
  CURSOR = 0;
  add = 1000;
  while(enter == 0)
  {
    enter = debounce(button4, previous_button4_state);
    up = debounce(button2, previous_button2_state);
    down = debounce(button3, previous_button3_state);
    side = debounce(button5, previous_button5_state);
    if(side == 1)
    {
      CURSOR++;
      add = add/10;
    }
    if(CURSOR > 3)
    {
      CURSOR = 0;
      add = 1000;
    }
    if(up == 1)
    {
      lcd.clear();
      Pressure_range = Pressure_range + add;
    }
    if(down == 1)
    {
      lcd.clear();
      Pressure_range = Pressure_range - add;
    }
    lcd.setCursor(0, 0);
    lcd.print(Pressure_range);
    lcd.print(" ");
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
    lcd.print("    ");
    lcd.setCursor(0,1);
    lcd.print("+");
    lcd.print(add);
    lcd.print("  ");
  }
  lcd.clear();
}

void Show(int &choice)
{
  switch(choice)
    {
    case 0:
    lcd.print("Version 1.8");
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
  upper_integer = upper_voltage*204.6;
  lower_integer = lower_voltage*204.6;
  slope = (Pressure_range)/(upper_integer - lower_integer);
  while(menu == 0)
  {
    menu = debounce(button1, previous_button1_state);
    Data = analogRead(Signal_In);
    pressure = (slope*Data) - (slope*lower_integer);
    tare = debounce(button5, previous_button5_state);
    if(tare == 1 && previous_tare == 0)
    {
      lower_integer = Data;
      previous_tare = 1;
    }
    else if(tare == 1 && previous_tare == 1)
    {
      lower_integer = lower_voltage*204.6;
      previous_tare = 0;
    }
    lcd.setCursor(0,0);
    lcd.print(pressure);
    lcd.print("  ");
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
     lcd.setCursor(0,1);
     lcd.print(Data);
     lcd.print(" ");
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
