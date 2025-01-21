#include <Regexp.h>

char input[]="abc<alt-A><ctrl-C>DEF<CR><Alt-tab><left><l-shift><win+d>";
  MatchState ms;

int result_test(char result)
{
  char buf[16];
  int length = 0;
switch (result)
  {
    case REGEXP_MATCHED:

      Serial1.println ("-----");
      Serial1.print ("Matched on: ");
      Serial1.println (ms.GetMatch (buf));

      // matching offsets in ms.capture

      Serial1.print ("Captures: ");
      Serial1.println (ms.level);

      for (int j = 0; j < ms.level; j++)
      {
        Serial1.print ("Capture number: ");
        Serial1.println (j + 1, DEC);
        Serial1.print ("Text: '");
        Serial1.print (ms.GetCapture (buf, j));
        length += strlen(ms.GetCapture(buf, j));
        Serial1.println ("'");

      }
    return length;

    case REGEXP_NOMATCH:
      Serial1.println ("No match.");
      return 0;

    default:
      Serial1.print ("Regexp error: ");
      Serial1.println (result, DEC);
      return 0;

  }  // end of switch
}


void parseit(char * input)
{
  char matched;
  int index = 0;
  int length = strlen(input);
  ms.Target(input);
  while (index < length)
  {
    Serial1.print("Index ");
    Serial1.print(index);
    Serial1.print(" Length ");
    Serial1.println(length);
    matched = ms.Match("([^<]*)", index);
    if (REGEXP_MATCHED == matched)
    {
      index += result_test(matched);
      continue;
    }
//    matched = ms.Match("(<alt\-[%a%d]+>)", index);
    matched = ms.Match("(\<alt\-)", index);
   if (REGEXP_MATCHED == matched)
    {
      index += result_test(matched);
      continue;
    }
    delay(1000);
 
  }


}

void setup() {
  // put your setup code here, to run once:
  Serial1.begin(115200);
  Serial1.println("Hello, Raspberry Pi Pico!");
  parseit(input);
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(1); // this speeds up the simulation
}

