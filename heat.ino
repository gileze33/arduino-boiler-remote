#include "heat.h"

int rfPin = 4;
int ledPin = 6;

float sampleMicrosecondsLength = 22.6; // 44100hz

Bit preambleBit = { 204, HIGH, 'P' };
Bit highBit = { 16, HIGH, 'H' };
Bit shortSpacerBit = { 13, LOW, 'S' };
Bit longSpacerBit = { 39, LOW, 'L' };
Bit groupSpacerBit = { 1195, LOW, ' ' };
int repeatDelaySamples = 824;

/** @public */
void setupHeat() {
  pinMode(ledPin, OUTPUT);
  pinMode(rfPin, OUTPUT);
}

void sendBit(Bit bit) {
  digitalWrite(rfPin, bit.value);
  delayMicroseconds(bit.lengthInSamples * sampleMicrosecondsLength);
}

Sequence buildSequence(char sequenceInput[]) {
  int sequenceLength = strlen(sequenceInput);

  Serial.print("sequenceLen ");
  Serial.println(sequenceLength);
  
  // Dynamically allocate memory for the sequence
  Bit* builtSequence = new Bit[sequenceLength];

  for (int sequenceIndex = 0; sequenceIndex < sequenceLength; sequenceIndex++) {
    switch (sequenceInput[sequenceIndex]) { 
      case 'P':
        builtSequence[sequenceIndex] = preambleBit;
        break;
      case 'H':
        builtSequence[sequenceIndex] = highBit;
        break;
      case 'S':
        builtSequence[sequenceIndex] = shortSpacerBit;
        break;
      case 'L':
        builtSequence[sequenceIndex] = longSpacerBit;
        break;
      case ' ':
        builtSequence[sequenceIndex] = groupSpacerBit;
        break;
    }
  }

  Sequence sequence = {
    sequenceLength,
    builtSequence 
  };

  return sequence;
}

void sendSequence(Sequence sequence, int repeats) {
  Serial.print("Sending sequence of length ");
  Serial.println(sequence.length);

  for (int i = 0; i < sequence.length; i++) {
    sendBit(sequence.bits[i]);
  }

  Serial.println("");

  // go back low at end of sequence
  digitalWrite(rfPin, LOW);

  if (repeats > 0) {
    delayMicroseconds(repeatDelaySamples * sampleMicrosecondsLength);
    sendSequence(sequence, repeats - 1);
  }
}

void freeSequence(Sequence& sequence) {
    delete[] sequence.bits;
    sequence.bits = nullptr;
}

void demandHeat(bool on) {
  if (on) {
    Serial.println("Demand heat: on");

    Sequence sequence = buildSequence("PLHSHLHSHSHLHLHLHLHLHSHLHLHLHSHSHLHSHSHSHSHSHSHSHLHSHSHSHSHSHSHSHLH PLHLHSHLHLHSHSHSHSHSHLHSHSHSHLHLHSHLHLHLHLHLHLHLHSHLHLHLHLHLHLHLHSH");
    sendSequence(sequence, 3);
    freeSequence(sequence);

    digitalWrite(ledPin, HIGH);
  }
  else {
    Serial.println("Demand heat: off");

    Sequence sequence = buildSequence("PLHSHLHSHSHLHLHLHLHSHSHLHLHLHSHSHLHSHSHSHSHSHSHSHLHSHSHSHSHSHSHSHSH PLHLHSHLHLHSHSHSHSHLHLHSHSHSHLHLHSHLHLHLHLHLHLHLHSHLHLHLHLHLHLHLHLH");
    sendSequence(sequence, 3);
    freeSequence(sequence);

    digitalWrite(ledPin, LOW);
  }
}