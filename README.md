# Let's Log Shit

LLShit (LLS if you must) is a set of logging classes, designed for Arduino boards, with the goal of providing a logging system useful for debugging and auditing.

### Flavors
  - Core provides the base class with only vital functions shared across all flavors
  - Full provides default, robust logging behavior.  It requires an SD card and uses some ram for its settings.  No feature (other than the obvious "do said thing but without an SD card") is to be missing from this Full flavor
  - SRam will provide memory based logging, no SD card.  Obviously this will be very size limited however you will be able to specify how many messages to keep, and retreive them as needed.  Interfacing via a serial connection and some basic logic would be one use case
  - NoRam will provide full SD based logging.  All settings are stored on the SD card at a hardcoded path.  Settings are loaded on demand, using minimal overhead only as required.

### So who gives a shit?
You should be excited to use this library because it will do what you need, log messages for future review, without requiring a bunch of set up, work, or debugging.  In my experience all projects benefit from a healthy dose of logging, and since all flavors fall back to a simple low overhead base class you can disable logging by swapping your LLS object class.

In addition, it does some trick shit you probably didn't even know you wanted.

### Samples
```c+
void setup(){
    LLSLogger lls = LLSLogger();

    lls.writeEvent("Hello Logger World!"); //You should never do this, eats sram
    lls.writeEvent(F("I'm better behaved")); //This eats flash which is fine

    char message[20];
    uint8_t number = 8;
    sprintf_P(message,PSTR("This is also okay to do %u"),number);
    lls.writeEvent(message);

    String stringsAreBadAndYouShouldFeelBadUsingThem;
    stringsAreBadAndYouShouldFeelBadUsingThem = "The logger is still kind even with bad things";
    lls.writeEvent(stringsAreBadAndYouShouldFeelBadUsingThem);
}
```

License
----
MIT

I do additionally ask you don't be a social toddler and rename the classes based on an aversion to vulgarity.  Just be happy I didn't call it AllMuslimsAreTerrorists.  Buck up princess.
