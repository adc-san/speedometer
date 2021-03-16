enum pins {
	s1pin = 8,	//sensor start rider 1
	s2pin = 7,	//sensor start rider 2
	f1pin = 5,	//sensor finish rider 1
	f2pin = 6,	//sensor finish rider 2
	ledpin = 10	//Output 38kHz for infrared led
          };
bool start = 0, test = 0, prt = 0, printed = 0,
     s1, s2, f1, f2, //sensors states
     g1, g2,         // flags of start measuring (Go!) test for mistake
     go1, go2,       // flags of measuring
     fi1, fi2,       // flags of finish, test for mistake
     fin1, fin2     // finish flags
     ;
unsigned long t, t1, t2, tf1, tf2; //times in milliseonds
void reset_all() {
  start = g1 = g2 = go1 = go2 = fi1 = fi2 = fin1 = fin2 = 0;
}
void setup() {

  pinMode(ledpin, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  tone(ledpin, 38000);
  pinMode(s1pin, INPUT);
  pinMode(s2pin, INPUT);
  pinMode(f1pin, INPUT);
  pinMode(f2pin, INPUT);
  // initialize serial:
  Serial.begin(9600);

}

void loop() {
  //get time
  t = millis();
  //test sensors
  s1 = digitalRead(s1pin);
  s2 = digitalRead(s2pin);
  f1 = digitalRead(f1pin);
  f2 = digitalRead(f2pin);


  //-----------LINE1-----------
  if (!fin1) {
    if (!go1) {
      //test for start sensor on
      if (s1) {
        if (!g1) t1 = t;
        g1 = 1;
      } else {
        g1 = 0;
      }
      if (g1) {
        //check for stability
        if ((unsigned long)(t1 - t) > 3) {
          go1 = 1;
        }
      }
    } else {
      //when we go
      //test for fin sensors on
      if (f1) {
        if (!fi1) tf1 = t;
        fi1 = 1;
      } else {
        fi1 = 0;
      }
      //check for stability
      if (fi1) {
        if ((unsigned long)(tf1 - t) > 3) {
          fin1 = 1;
        }
      }
    }
  }

  //-----------LINE2-----------
  if (!fin2) {
    if (!go2) {
      //test for start sensor on
      if (s2) {
        if (!g2) t2 = t;
        g2 = 1;
      } else {
        g2 = 0;
      }
      if (g2) {
        //check for stability
        if ((unsigned long)(t2 - t) > 3) {
          go2 = 1;
        }
      }
    } else {
      //when we go
      //test for fin sensors on
      if (f2) {
        if (!fi2) tf2 = t;
        fi2 = 1;
      } else {
        fi2 = 0;
      }
      //check for stability
      if (fi2) {
        if ((unsigned long)(tf2 - t) > 3) {
          fin2 = 1;
        }
      }
    }
  }

  //indicate for callibration
  if (s1 || s2 || f1 || f2)digitalWrite(LED_BUILTIN, HIGH);
  else digitalWrite(LED_BUILTIN, LOW);

  //autoprint
  if (fin1 && fin2) {
    if (!printed) prt = 1;
  } else {
    printed = 0;
  }

  // print result
  if (prt)
  {

    unsigned long tmp_t, tmp_spd;

    Serial.print(' ');
    if (fin1) {
      tmp_t = tf1 - t1;
      if (tmp_t < 1)tmp_t = 1;
      tmp_spd = 3600000 / tmp_t; //calculate speed*100 in km/h on 10 m.
      if (tmp_spd % 10 > 4)tmp_spd += 10; //round
      tmp_spd /= 10;
      Serial.print(tmp_spd / 10, DEC);
      Serial.print('.');
      Serial.print(tmp_spd % 10, DEC);
    }
    else Serial.print("----");
    Serial.print(" | ");
    if (fin2) {
      tmp_t = tf2 - t2;
      if (tmp_t < 1)tmp_t = 1;
      tmp_spd = 3600000 / tmp_t; //calculate speed*100 in km/h on 10 m.
      if (tmp_spd % 10 > 4)tmp_spd += 10; //round
      tmp_spd /= 10;
      Serial.print(tmp_spd / 10, DEC);
      Serial.print('.');
      Serial.print(tmp_spd % 10, DEC);
    }
    else Serial.print("----");
    Serial.println(" km/h");

    prt = 0;
    printed = 1;
  }


  // print sensors state:
  if (test) {
    Serial.print(' ');
    if (fin1) Serial.print((tf1 - t1), DEC);
    else {
      Serial.print('[');
      Serial.print(s1);
      Serial.print(f1);
      Serial.print(go1);
      Serial.print(']');
    }
    Serial.print(" | ");
    if (fin2) Serial.print((tf2 - t2), DEC);
    else {
      Serial.print('[');
      Serial.print(s2);
      Serial.print(f2);
      Serial.print(go2);
      Serial.print(']');
    }
    Serial.println();
    test = 0;
  }


  //clear time and ready to start
  if (start) {
    reset_all();
    Serial.println("----START----");
    start = 0;
  }

}

/*
  SerialEvent occurs whenever a new data comes in the hardware serial RX. This
  routine is run between each time loop() runs, so using delay inside loop can
  delay response. Multiple bytes of data may be available.
*/
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    if (inChar == 's') start = true;
    if (inChar == 't') test = true;
    if (inChar == 'p') prt = true;
  }
}
