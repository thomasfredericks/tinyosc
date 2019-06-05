/**
 * By Thomas O Fredericks (tof@tofstuff.com) 
 * Base on TinyOsc by :
 * Copyright (c) 2015-2018, Martin Roth (mhroth@gmail.com)
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef _TINY_OSC_
#define _TINY_OSC_

#include <stdbool.h>
#include <stdint.h>


#define OSC_RECEIVE_BUFFER_SIZE 256
#define OSC_SEND_BUFFER_SIZE 256

/*
#ifdef __cplusplus
extern "C" {
#endif
*/

typedef struct tosc_message {
  char *format;  // a pointer to the format field
  char *marker;  // the current read head
  char *buffer;  // the original message data (also points to the address)
  uint32_t len;  // length of the buffer data
} tosc_message;

typedef struct tosc_bundle {
  char *marker; // the current write head (where the next message will be written)
  char *buffer; // the original buffer
  uint32_t bufLen; // the byte length of the original buffer
  uint32_t bundleLen; // the byte length of the total bundle
} tosc_bundle;





class TinyOsc {
	 public:
	 	typedef void (*tOscCallbackFunction)(void);

/*!
    @brief  Create an instance of the TinyOsc class.

    @code

    // Create an instance of the TinyOsc class.
    TinyOsc() osc;

    @endcode
*/
    TinyOsc();

 /**
 * Parse a SLIP stream.
 * Calls the callback for every message received.
 * Bundles are unpacked into individual messages.
 */
void parseSlip( Stream& stream, tOscCallbackFunction callback );

/**
 * Writes an OSC packet to a SLIP stream. 
 */
uint32_t writeSlip(Stream& stream, const char *address,
    const char *fmt, ...);




 private:
     	tosc_bundle bundle;
     	tosc_bundle* b;
     	tosc_message* o;
     	tosc_message message;
     	tOscCallbackFunction callback;
     	uint64_t timetag;
     	bool isPartOfABundle;

     	char inputBuffer[OSC_RECEIVE_BUFFER_SIZE];
     	char outputBuffer[OSC_SEND_BUFFER_SIZE];

uint64_t parseBundleTimeTag();


 

void parseBundle();

 /**
 * Parse a buffer containing an OSC message.
 * The contents of the buffer are NOT copied.
 * Returns 0 if there is no error. An error code (a negative number) otherwise.
 */
int parseMessage();

/**
 * Resets the read head to the first element.
 *
 * @return  The same tosc_message pointer.
 */
void reset();


/**
 * Returns true if the message is a bundle. False otherwise.
 */
bool isABundle();


/**
 * Parses the next message in a bundle. Returns true if successful.
 * False otherwise.
 */
bool getNextMessage();





public:

// BUNDLES ARE PARSED AS INDIVIDUAL MESSAGES
/**
 * Returns true if the message was part of a bundle. False otherwise.
 */
// bool isBundled();


// TIMETAGS ARE NOT SUPPORTED YET
/**
 * Returns the timetag of an OSC bundle.
 * If the received message is not part of a bundle, returns 0.
 */
// uint64_t getBundleTimetag();


/**
 * Returns a point to the address block of the OSC buffer.
 * This is also the start of the buffer.
 */
char* getAddress();

/**
 * Returns true if the address matches.
 */
bool fullMatch(const char* address);

/**
 * Returns true if the address and type tag matches.
 */
bool fullMatch(const char* address, const char * typetags);

/**
 * Returns a pointer to the format block of the OSC buffer.
 */
char* getTypeTags();

/**
 * Returns the length in bytes of this message.
 */
uint32_t getLength();

/**
 * Returns the next 32-bit int. Does not check buffer bounds.
 */
int32_t getNextInt32();

/**
 * Returns the next 64-bit int. Does not check buffer bounds.
 */
int64_t getNextInt64();

/**
 * Returns the next 64-bit timetag. Does not check buffer bounds.
 */
uint64_t getNextTimetag();

/**
 * Returns the next 32-bit float. Does not check buffer bounds.
 */
float getNextFloat();

/**
 * Returns the next 64-bit float. Does not check buffer bounds.
 */
double getNextDouble();

/**
 * Returns the next string, or NULL if the buffer length is exceeded.
 */
const char* getNextString();

/**
 * Points the given buffer pointer to the next blob.
 * The len pointer is set to the length of the blob.
 * Returns NULL and 0 if the OSC buffer bounds are exceeded.
 */
void getNextBlob(const char **buffer, int *len);

/**
 * Returns the next set of midi bytes. Does not check bounds.
 * Bytes from MSB to LSB are: port id, status byte, data1, data2.
 */
unsigned char* getNextMidi();



// BUNDLE SENDING IS NOT SUPPORTED AS TIMESTAMP ARE NOT YET SUPPORTED

/**
 * Starts writing a bundle to the given buffer with length.
 */
/*void writeBundle(tosc_bundle *newBundle, uint64_t timetag, char *buffer, const int len);*/

/**
 * Write a message to a bundle buffer. Returns the number of bytes written.
 */
/*uint32_t addMessageToBundle(tosc_bundle *newBundle,
    const char *address, const char *format, ...);
*/
/**
 * Returns the length in bytes of the created bundle.
 */
/*uint32_t getBundleLength(tosc_bundle *newBundlenewBundle);*/






  /// Parse messages and executes the callback.
  //virtual void parseMessages(tOscCallbackFunction callback) = 0;

private:
  /// Sends the actual message.
  //virtual void write() = 0;


};



/**
 * A convenience function to (non-destructively) print a buffer containing
 * an OSC message to stdout.
 */
//void tosc_printOscBuffer(char *buffer, const int len);

/**
 * A convenience function to (non-destructively) print a pre-parsed OSC message
 * to stdout.
 */
//void tosc_printMessage(tosc_message *o);






#endif // _TINY_OSC_
