#include <Process.h>
#include <Console.h>

int sensorPin = A0;
int sensorValue = 0;

void setup() {
  Bridge.begin();   // Initialize the Bridge
  Console.begin();
  
  // Wait for Console port to connect
  while (!Console);
  
  Console.println("Exemple de script python");
}

void loop() {
  Process p;
  // This command line runs the WifiStatus script, (/usr/bin/pretty-wifi-info.lua), then 
  // sends the result to the grep command to look for a line containing the word
  // "Signal:"  the result is passed to this sketch:
  sensorValue = analogRead(sensorPin);
  
  p.begin("python");
  p.addParameter("/mnt/sd/arduino/somme.py");
  p.addParameter(String(sensorValue));
  p.run();

  // do nothing until the process finishes, so you get the whole output:
  while(p.running());  
  Console.println("le script est exécuté");

  // Read command output. runShellCommand() should have passed "Signal: xx&":
  while (p.available()) {
    int result = p.parseInt();          // look for an integer
    Console.println(result);         // print the number as well
  } 
  Console.flush();
  delay(5000);  // wait 5 seconds before you do it again
}
