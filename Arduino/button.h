#ifndef BUTTON_H
#define BUTTON_H

// Button timing variables
const int debounce = 20;            // Ms debounce period to prevent flickering when pressing or releasing the button
const int DCgap = 250;              // Max ms between clicks for a double click event
const int holdTime = 1000;          // Ms hold period: how long to wait for press+hold event
const int longHoldTime = 3000;      // Ms long hold period: how long to wait for press+hold event

class BUTTON {
  private:
    // Button variables
    bool buttonVal = true;         // Value read from button
    bool buttonLast = true;        // Buffered value of the button's previous state
    bool DCwaiting = false;        // Whether we're waiting for a double click (down)
    bool DConUp = false;           // Whether to register a double click on next release, or whether to wait and click
    bool singleOK = true;          // Whether it's OK to do a single click
    long downTime = -1;            // Time the button was pressed down
    long upTime = -1;              // Time the button was released
    bool ignoreUp = false;         // Whether to ignore the button release because the click+hold was triggered
    bool waitForUp = false;        // When held, whether to wait for the up event
    bool holdEventPast = false;    // Whether or not the hold event happened already
    bool longHoldEventPast = false;// Whether or not the long hold event happened already
  public:
    BUTTON();
    int checkButton();
};

#endif
