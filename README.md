# Let's Log Shit

LLShit (LLS if you must) is a set of logging classes, designed for Arduino boards, with the goal of providing a logging system useful for debugging and auditing.

### Modes
  - fileMode (default) will use the SD card and standard features
  - ramMode does not require an SD card (suitable for nano).  It also disables many features
  - offMode disables the logger while keeping the library around.  This could be used to conditionally enable/disable logging

### So who gives a shit?
You should be excited to use this library because it will do what you need, log messages for future review, without requiring a bunch of set up, work, or debugging.  In my experience all projects benefit from a healthy dose of logging, and this library has a very small footprint

In addition, it does some trick shit you probably didn't even know you wanted.

One feature, optional (but read on for ramMode), is setting the avgMessageLength (call setAverageMessageLength).
This variable is used for estimating offsets, when you want 3 messages it guesses 3*avgMessageLength back.  Having a bad estimate in fileMode is only inefficient, the library will still function.  In ramMode this controls how much ram is allocated for storing log messages; so be sure to aim a little high or be mindful of what you are logging.


### Samples
```c+
void setup(){
    LLSLogger lls = LLSLogger();

    lls.writeEvent("Hello Logger World!"); //You should never do this, eats sram
    lls.writeEvent(F("I'm better behaved")); //This eats flash which is the best variable free option

    char message[20];
    uint8_t number = 8;
    sprintf_P(message,PSTR("This is also okay to do %u"),number); //Do this to store the format in flash
    lls.writeEvent(message); //Good with char[] / char*

    String stringsAreBadAndYouShouldFeelBadUsingThem;
    stringsAreBadAndYouShouldFeelBadUsingThem = "The logger is still kind even with bad things";
    lls.writeEvent(stringsAreBadAndYouShouldFeelBadUsingThem); //Even allows strings
}
```
This will write the various strings to the log file (note it's able to take multiple inputs such as flash memory, strings, or char arrays). The file written is controlled by the library based on either the RTC time you supplied (resolving to 1-31 based on date) or it will start at 1 and increment every 24 hours, wrapping automatically.
```c+
void setup(){
	Serial.begin(9600);

    LLSLogger lls = LLSLogger();
	lls.setRTC(50);//Set logger clock (setTime function)

    lls.writeEvent(F("I'm better behaved"));

    //Event List - create one to get log messages into memory
    LSLoggerEventList* list = NULL;
    //Load the last 3 messages
	list = lls.getRecentEventList(list,3);
	//Iterate the list
	LLSLoggerEventList* node = list;
    while(node != NULL){
        Serial.println(node->message);
        node = node->next;
    }
    //Free the list from memory
    LLSLoggerEvent::clearList(list);
}
```

```c+
void setup(){
	LLSLogger lls = LLSLogger();
	//Set average length
	lls.setAverageMessageLength(50);
	//Switch to ramMode (pure memory)
	lls.ramMode(10);

	lls.writeEvent(F("I'm better behaved"));
}
```


License
----
MIT

I do additionally ask you don't be a social toddler and rename the classes based on an aversion to vulgarity.  Just be happy I didn't call it AllMuslimsAreTerrorists.  Buck up princess.
