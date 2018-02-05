/*
 * LightCtr.cpp
 *
 *  Created on: 22 Feb 2017
 *      Author: Adam Milward
 */

//#define DEBUG
//#define WORDS

#include "LightCtr.h"
#include "Controller.h"

Light LightCtr::Red(Gbl::RED_PIN, 0);
Light LightCtr::Green(Gbl::GREEN_PIN, 1);
Light LightCtr::Blue(Gbl::BLUE_PIN, 2);

LightCtr::LightCtr() {}


#ifdef WORDS
const char LightCtr::remoteAlias
				[LightCtr::remoteAliasCount]
				 [LightCtr::remoteAliasLength]
		  PROGMEM = {
		{"up"},   {"down"},   {"on"},   {"off"},
		{"red"},   {"green"},   {"blue"},   {"white"},
		{"orange"},   {"yellow"},   {"cyan"},   {"purple"},
		{"j1"},   {"j2"},   {"j3"},   {"j4"},
		{"m1"},  {"m2"},   {"m3"},   {"m4"}
};

const char LightCtr::oneWordCommands
				[LightCtr::oneWordCount]
				 [LightCtr::oneWordLength]
				  PROGMEM = {
					{"report"},
					{"top"},
					{"bottom"},
					{"static"},
					{"lin"},
					{"sin"},
					{"exp"},
					{"sinexp"}
};

const char LightCtr::firstOfTwoCommands
				[LightCtr::firstOfTwoCount]
				 [LightCtr::firstOfTwoLength]
				  PROGMEM = {
           "all", "red", "green", "blue",
           "lower", "upper", "delay"
};

const char LightCtr::secondOfTwoCommands
				[LightCtr::secondOfTwoCount]
				 [LightCtr::secondOfTwoLength]
				  PROGMEM = {
		{"up"}, {"down"}, {"bottom"}, {"top"}, {"off"}
};

const char LightCtr::wordAndFloatCommands
				[LightCtr::wordAndFloatCount]
				 [LightCtr::wordAndFloatLength]
				  PROGMEM = {
		{"report"},
		{"all"},
		{"red"},
		{"green"},
		{"blue"},
		{"delay"}
};


bool LightCtr::actionSerial(char **wordPtrs, byte wordCount){
#ifdef DEBUG
	Gbl::strPtr->println(F("LightCtr::actionSerial"));
	Gbl::freeRam();
	Gbl::strPtr->println(F("wordCount"));
	Gbl::strPtr->println(wordCount);
#endif
	if (1 == wordCount) {
        if (actionOneWord(wordPtrs)) return true;
    } else if (2 == wordCount) {
        if (parseTwoWords(wordPtrs)) return true;
    }
	help();
    return false;
}

void LightCtr::help() {
    Gbl::strPtr->println(F("Command not recognised, options are:"));
    Gbl::strPtr->println(F("the name of a button on the remote"));
    Gbl::strPtr->println(F(""));
    Gbl::strPtr->println(F("- or"));
    Gbl::strPtr->println(F("- Single Word Commands "));
    int i = oneWordCount;
    while (i--) {
		char oneItem[oneWordLength];
		memcpy_P (&oneItem, &oneWordCommands[i], sizeof oneItem);
		Gbl::strPtr->println(oneItem);
    }
    Gbl::strPtr->println(F(""));
    Gbl::strPtr->println(F("- or"));
    Gbl::strPtr->println(F("-   word + number "));
    i = wordAndFloatCount;
    while (i--) {
		char oneItem[wordAndFloatLength];
		memcpy_P (&oneItem, &wordAndFloatCommands[i], sizeof oneItem);
    	Gbl::strPtr->print(oneItem);
    	Gbl::strPtr->println(F(" [nn]"));
    }
    Gbl::strPtr->println(F("eg. 'report 10';"));
    Gbl::strPtr->println(F(""));
    Gbl::strPtr->println(F("- or"));
    Gbl::strPtr->println(F("- Two Word Commands"));
    Gbl::strPtr->println(F("all|red|green|blue|lower|upper|delay"));
    Gbl::strPtr->println(F("+"));
    Gbl::strPtr->println(F("up|down|top|bottom"));
    Gbl::strPtr->println(F("eg. delay top"));
}

bool LightCtr::actionOneWord(char **wordPtrs){
#ifdef DEBUG
	Gbl::strPtr->println(F("LightCtr::actionOneWord"));
	Gbl::freeRam();
	Gbl::strPtr->println(F("*wordPtrs"));
	Gbl::strPtr->println(*wordPtrs);
#endif
	int i = oneWordCount;
    while (i--) {
		// make a copy of the current one
		char oneItem[oneWordLength];
		memcpy_P (&oneItem, &oneWordCommands[i], sizeof oneItem);
        if (strcasecmp(oneItem, *wordPtrs) == 0) {
            (this->*oneWordActions[i])();
            return true;
        }
    }
    i = remoteAliasCount;
    while (i--) {
    	// make a copy of the current one
		char oneItem[remoteAliasLength];
		memcpy_P (&oneItem, &remoteAlias[i], sizeof oneItem);
    	if (strcasecmp(oneItem, *wordPtrs) == 0) {
            (this->*actions[ctrMode][i])();
            return true;
        }
    }
    return false;
}

bool LightCtr::parseTwoWords(char **wordPtrs) {
#ifdef DEBUG
	Gbl::strPtr->println(F("LightCtr::parseTwoWods"));
	Gbl::freeRam();
#endif
	if (Controller::isNum(wordPtrs[1])) {
        return actionWordAndFloat(wordPtrs, atoi(wordPtrs[1]));
    } else {
        return actionTwoWords(wordPtrs);
    }
}

bool LightCtr::actionWordAndFloat(char **wordPtrs, float value) {
#ifdef DEBUG
	Gbl::strPtr->println(F("LightCtr::actionWordAndFloat"));
	Gbl::freeRam();
#endif
	int i = wordAndFloatCount;
    while (i--) {
    	char oneItem[wordAndFloatLength];
		memcpy_P (&oneItem, &wordAndFloatCommands[i], sizeof oneItem);
        if (strcasecmp(oneItem, *wordPtrs) == 0) {
            (this->*wordAndFloatActions[i])(value);
            return true;
        }
    }
    return false;
}

bool LightCtr::actionTwoWords(char **wordPtrs) {
#ifdef DEBUG
	Gbl::strPtr->println(F("LightCtr::actionTwoWords"));
	Gbl::freeRam();
	Gbl::strPtr->println(wordPtrs[0]);
	Gbl::strPtr->println(wordPtrs[1]);
#endif
	// first word
	int i = firstOfTwoCount;
    while (i --) {
    	char oneItem[firstOfTwoLength];
    	memcpy_P (&oneItem, &firstOfTwoCommands[i], sizeof oneItem);
#ifdef DEBUG
		Gbl::strPtr->println(oneItem);
#endif
        if (strcasecmp(oneItem, wordPtrs[0]) == 0) {

        	//second word
        	int j = secondOfTwoCount;
            while (j--) {
            	char oneItem[secondOfTwoLength];
				memcpy_P (&oneItem, &secondOfTwoCommands[j], sizeof oneItem);
#ifdef DEBUG
				Gbl::strPtr->println(oneItem);
#endif
                if (strcasecmp(oneItem, wordPtrs[1]) == 0) {
                    (this->*twoWordActions[i][j])();
                    return true;
                }
            }
        }
    }
    return false;
}
#endif

bool LightCtr::actionRemote(unsigned long inValue){
    for (int i = 0; i < 20; i++) {
        if (codes[i] == inValue) {
            (this->*actions[ctrMode][i])();
            Gbl::strPtr->println(i);
            return true;
        }
    }
    return false;
}

void LightCtr::setReportDelay(float delaySeconds) {
    reportDelay = (unsigned int)(delaySeconds * 1000);
    waitMillisReport = millis() + reportDelay;
    report();
}

void LightCtr::timer(unsigned long millis) {
    if (ctrMode != LightCtr::STATIC
               &&
               (signed long)(millis - waitMillisLights) >= 0
       )
       {
           waitMillisLights = millis + fadeDelay;
           interrupt();
       }
       else if (reportDelay > 0
               &&
               (signed long)(millis - waitMillisReport) >= 0
       )
       {
           report();
           waitMillisReport = millis + reportDelay;
       }
}

void LightCtr::report() {
    Gbl::strPtr->print(F("Red      base: "));
    Gbl::strPtr->print(Red.base);
    Gbl::strPtr->print(F("  power: "));
    Gbl::strPtr->println(Red.power);

    Gbl::strPtr->print(F("Green  base: "));
    Gbl::strPtr->print(Green.base);
    Gbl::strPtr->print(F("  power: "));
    Gbl::strPtr->println(Green.power);

    Gbl::strPtr->print(F("Blue     base: "));
    Gbl::strPtr->print(Blue.base);
    Gbl::strPtr->print(F("  power: "));
    Gbl::strPtr->println(Blue.power);

    Gbl::strPtr->print(F("Red      gain: "));
    Gbl::strPtr->println(Red.gain);

    Gbl::strPtr->print(F("Green  gain: "));
    Gbl::strPtr->println(Green.gain);

    Gbl::strPtr->print(F("Blue     gain: "));
    Gbl::strPtr->println(Blue.gain);

    Gbl::strPtr->print(F("Red      lower: "));
    Gbl::strPtr->print(Red.lower);
    Gbl::strPtr->print(F("  range: "));
    Gbl::strPtr->println(Red.range);

    Gbl::strPtr->print(F("Green  lower: "));
    Gbl::strPtr->print(Green.lower);
    Gbl::strPtr->print(F("  range: "));
    Gbl::strPtr->println(Green.range);

    Gbl::strPtr->print(F("Blue     lower: "));
    Gbl::strPtr->print(Blue.lower);
    Gbl::strPtr->print(F("  range: "));
    Gbl::strPtr->println(Blue.range);

    if (STATIC == ctrMode) {
        Gbl::strPtr->println(F("Mode: STATIC"));
    } else {
        Gbl::strPtr->print(F("FadeMode: "));
        Gbl::strPtr->println(Light::fMode);
        Gbl::strPtr->print(F("Delay: "));
        Gbl::strPtr->println(fadeDelay);
    }
    Gbl::strPtr->print(F("report delay seconds = "));
    Gbl::strPtr->println((int)reportDelay/1000);
}

void LightCtr::interrupt(){
    static int counter = 0;  // increments each repetition
    switch (counter) {
    case 1:
        // slide should not be more than 0.002 for smootheness
        Red.slide();
        break;
    case 2:
        Green.slide();
        break;
    case 3:
        Blue.slide();
        break;
    default:
        counter = 0;
        break;
    }
    counter++;
}
void LightCtr::retrieveStore(colour inColour){
    //Serial.print("retrieveStore Colour: ");
    //Serial.println(inColour);
    tempStore[0] = Red.base;
    tempStore[1] = Green.base;
    tempStore[2] = Blue.base;
    //Serial.print("Stored Values: ");
    //Serial.print(colourStore[inColour][0]);
    //Serial.print(colourStore[inColour][1]);
    //Serial.println(colourStore[inColour][2]);
    Red.set(colourStore[inColour][0]);
    Green.set(colourStore[inColour][1]);
    Blue.set(colourStore[inColour][2]);
    ctrMode = STATIC;
    Gbl::strPtr->println(F("lights STATIC"));
}
void LightCtr::storeThis(colour inColour){
    //Serial.print("store New Colour ");
    //Serial.println(inColour);
    Red.set(tempStore[0]);
    Green.set(tempStore[1]);
    Blue.set(tempStore[2]);
    colourStore[inColour][0] = tempStore[0];
    colourStore[inColour][1] = tempStore[1];
    colourStore[inColour][2] = tempStore[2];
    //Serial.print("Stored Values: ");
    //Serial.print(colourStore[inColour][0]);
    //Serial.print(colourStore[inColour][1]);
    //Serial.println(colourStore[inColour][2]);
}

void LightCtr::allSet(float inBase) {
    Red.set((inBase * 0.11) - 0.1);
    Green.set((inBase * 0.11) - 0.1);
    Blue.set((inBase * 0.11) - 0.1);
}
void LightCtr::redSet(float inBase) {
    Red.set((inBase * 0.11) - 0.1);
}
void LightCtr::greenSet(float inBase) {
    Green.set((inBase * 0.11) - 0.1);
}
void LightCtr::blueSet(float inBase) {
    Blue.set((inBase * 0.11) - 0.1);
}

void LightCtr::fadeOff() {
    ctrMode = STATIC;
}

void LightCtr::allBot(){
    Red.set(0);
    Green.set(0);
    Blue.set(0);
}
void LightCtr::allTop(){
    Red.set(1);
    Green.set(1);
    Blue.set(1);
}
void LightCtr::redBot() { Red.set(0);}
void LightCtr::redTop() { Red.set(1);}
void LightCtr::redOff() { Red.set(-1);}
void LightCtr::greenBot() { Green.set(0);}
void LightCtr::greenTop() { Green.set(1);}
void LightCtr::greenOff() { Green.set(-1);}
void LightCtr::blueBot() { Blue.set(0);}
void LightCtr::blueTop() { Blue.set(1);}
void LightCtr::blueOff() { Blue.set(-11);}
void LightCtr::lowerBot() {
    Red.changeLower(-1, 1);
    Green.changeLower(-1, 1);
    Blue.changeLower(-1, 1);
}
void LightCtr::lowerTop() {
    Red.changeLower(1, 1);
    Green.changeLower(1, 1);
    Blue.changeLower(1, 1);
}
void LightCtr::upperBot() {
    Red.changeUpper(-1, 1);
    Green.changeUpper(-1, 1);
    Blue.changeUpper(-1, 1);
}
void LightCtr::upperTop() {
    Red.changeUpper(1, 1);
    Green.changeUpper(1, 1);
    Blue.changeUpper(1, 1);
}
void LightCtr::delayBot(){
    fadeDelay = Gbl::DELAY_MIN-1;
    ctrMode = FADE;
    checkDelay();
}
void LightCtr::delayTop() {
    fadeDelay = Gbl::DELAY_MAX+1;
    ctrMode = FADE;
    checkDelay();
}
void LightCtr::delaySet(float inDelay) {
    fadeDelay = (int)inDelay;
    ctrMode = FADE;
    checkDelay();
}

void LightCtr::allUp(){
    Red.shift(+1);
    Green.shift(+1);
    Blue.shift(+1);
}

void LightCtr::allDown(){
    Red.shift(-1);
    Green.shift(-1);
    Blue.shift(-1);
}
void LightCtr::on (){ //off
    Red.set(0.5);
    Green.set(0.5);
    Blue.set(0.5);
}
void LightCtr::off (){ //off
    Red.set(-1);
    Green.set(-1);
    Blue.set(-1);
}
void LightCtr::redUp   () { Red.shift(+1);}
void LightCtr::greenUp () { Green.shift(+1); }
void LightCtr::blueUp () { Blue.shift(+1); }
void LightCtr::redDown() { Red.shift(-1); }
void LightCtr::greenDown() { Green.shift(-1); }
void LightCtr::blueDown() { Blue.shift(-1); }
void LightCtr::store1 () {
    if (holdCount == 0) {
        retrieveStore(RED);
    } else if (holdCount == 4) {
        storeThis(RED);
    }
    ctrMode = STATIC;
}
void LightCtr::store2 () {
    if (holdCount == 0) {
        retrieveStore(GREEN);
    } else if (holdCount == 4) {
        storeThis(GREEN);
    }
    ctrMode = STATIC;
}
void LightCtr::store3 () {
    if (holdCount == 0) {
        retrieveStore(BLUE);
    } else if (holdCount == 4) {
        storeThis(BLUE);
    }
    ctrMode = STATIC;
}
void LightCtr::store4 () {
    if (holdCount == 0) {
        retrieveStore(WHITE);
    } else if (holdCount == 4) {
        storeThis(WHITE);
    }
    ctrMode = STATIC;
}
void LightCtr::lin    () {
    ctrMode = FADE;
    Light::fMode = Light::LIN;
    Red.flashOff();
    Green.flashOff();
    Blue.flashOff();
	Gbl::strPtr->println(F("fadeMode LIN"));
}
void LightCtr::sin    () {
    ctrMode = FADE;
    Light::fMode = Light::SIN;
    Red.flashOff();
    Green.flashOff();
    Blue.flashOff();
	Gbl::strPtr->println(F("fadeMode SIN"));
}
void LightCtr::exp    () {
    ctrMode = FADE;
    Light::fMode = Light::EXP;
    Red.flashOff();
    Green.flashOff();
    Blue.flashOff();
	Gbl::strPtr->println(F("fadeMode EXP"));
}
void LightCtr::sinExp    () {
    ctrMode = FADE;
    Light::fMode = Light::EXPSIN;
    Red.flashOff();
    Green.flashOff();
    Blue.flashOff();
	Gbl::strPtr->println(F("fadeMode EXPSIN"));
}

void LightCtr::red_f(){
    Red.changeLower(+1, 0.2);
    Green.changeLower(+1, 0.2);
    Blue.changeLower(+1, 0.2);
}
void LightCtr::orange_f (){
    Red.changeLower(-1, 0.2);
    Green.changeLower(-1, 0.2);
    Blue.changeLower(-1, 0.2);
}
void LightCtr::green_f (){
    Red.changeUpper(+1, 0.2);
    Green.changeUpper(+1, 0.2);
    Blue.changeUpper(+1, 0.2);
}
void LightCtr::yellow_f(){
    Red.changeUpper(-1, 0.2);
    Green.changeUpper(-1, 0.2);
    Blue.changeUpper(-1, 0.2);
}
void LightCtr::white_f(){
    fadeDelay *= 4;
    checkDelay();
}
void LightCtr::purple_f(){
    fadeDelay /= 4;
    checkDelay();
}
void LightCtr::checkDelay(){
    if (fadeDelay > Gbl::DELAY_MAX) {
        fadeDelay = Gbl::DELAY_MAX;
        Red.flashHalf();
        Green.flashHalf();
        Blue.flashHalf();
    } else if (fadeDelay < Gbl::DELAY_MIN) {
        fadeDelay = Gbl::DELAY_MIN;
        Red.flashHalf();
        Green.flashHalf();
        Blue.flashHalf();
    }
    Red.flashOff();
    Green.flashOff();
    Blue.flashOff();
    Serial.println(fadeDelay);
    Gbl::strPtr->print(F("delay= "));
    Gbl::strPtr->println(fadeDelay);
}
void LightCtr::null(){};
