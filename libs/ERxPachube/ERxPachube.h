/*
ERxPachube.h - Pachube library for Arduino. 
Copyright (c) 2011 Jeffrey Sun.  All right reserved.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/*
Function
 * This library is used to communicate with Pachube server through web API.
 * It encapsulates the work to GET and PUT data.
 * The supported data format is csv.

Required library
 * WString, Ethernet, SPI

Project home
 * http://code.google.com/p/pachubelibrary/

*/
#ifndef ERXPACHUBE_H
#define ERXPACHUBE_H

#include <WString.h> // String, Todo: we should remove the usage of string to avoid new/delete.

#define PACHUBE_LIB_VER "3.2"

/************CONFIGURATION********************************/
// Define the max number of the data streams. 
// The value can be changed on demand.
#define MAX_DATASTREAM_NUM 5

// Define the max length of the API key and feed id.
// The length of the current API KEY "8VoKMmqj23MsAaO6VolYGqzKeQ2tpKFASwfARrR2R2j" is 44.
#define COSM_API_KEY_MAX_LENGTH 64
#define COSM_FEED_ID_MAX_LENGTH 10

/********************************************************/

#define INVALID_DATA_STREAM_ID 0xFFFF


// Pachube data stream structure
class ERxDataStream
{
public:
	// Even though the server supports to use any string as the ID.
	// But considering it is too expensive to process the string in MCU,
	// so we just support the digital id.
	unsigned int	mId; 
	String			mValue;
};

// Base class
class ERxPachube
{
protected:
	ERxPachube(const char* APIKey, unsigned int feedId); // backward compatibility
	ERxPachube(const char* APIKey, const char* feedId);

public: // For data in
	// Return the number of data streams.
	unsigned int	countDatastreams() const;

	// Return the string value of a particular data stream. Return an empty string if not exist.
	const String&	getValueString(unsigned int id) const;

	// Convert and return an int data for value of a particular data stream. Return 0 if not exist.
	int				getValueInt(unsigned int id) const;

	// Convert and return a float data for value of a particular data stream. Return 0 if not exist.
	float			getValueFloat(unsigned int id) const;

	// Return id of a particular data stream. Return an empty string if not exist.
	unsigned int	getIdByIndex(unsigned int index) const;

	// Return value of a particular data stream. Return an empty string if not exist.
	const String&	getValueByIndex(unsigned int index) const;

public: // For data out
	// Set up a data stream with a particular id.
	// If the id already exists, clear its value. Return true.
	// Otherwise, if the data stream exceed the count limitation, return false.
	// If not, add a new data stream with the passed in id. Return true.
	bool			addData(unsigned int id);

	// Set up a data stream with a particular id.
	// If the id already exists, update its value. Return true.
	// Otherwise, if the data stream exceed the count limitation, return false.
	// If not, add a new data stream with the passed in id and value. Return true.
	bool			addData(unsigned int id, const String& value);

	// Update a particular data stream with a String value. 
	// If success, return true. Otherwise(such as the id doesn't exist), return false.
	bool			updateData(unsigned int, const String& value);

	// Update a particular data stream with a int value. 
	// If success, return true. Otherwise(such as the id doesn't exist), return false.
	bool			updateData(unsigned int id, int value);

	// Update a particular data stream with a int value. 
	// If success, return true. Otherwise(such as the id doesn't exist), return false.
	bool			updateData(unsigned int id, float value);

public:
	// Return the API key
	const char*		getAPIKey() const;

	// Return the feed Id
	const char*		getFeedId() const;

protected:
	// Get the index of a particular data stream in the array. Return -1 if not find. 
	int				getIndex(unsigned int id) const;

	// Remove all the data.
	void			clearData();

private:
	// Update a particular data stream with a String value. 
	// If success, return true. Otherwise(such as the id doesn't exist), return false.
	// This function is used for avoid the String construction when update data with a string pointer.
	bool			updateDataHelper(unsigned int, char* value);
	void			setKeyAndFeed(const char* APIKey, const char* feedId);

private:
	char			mAPIkey[COSM_API_KEY_MAX_LENGTH];
	char			mFeedId[COSM_FEED_ID_MAX_LENGTH];

	ERxDataStream	mDataStreams[MAX_DATASTREAM_NUM];
	unsigned int	mDatasteamLength; // The actual length
};

// PUT local data up to Pachube server.
class ERxPachubeDataOut : public ERxPachube
{
public:
	ERxPachubeDataOut(const char* APIKey, unsigned int feedId);
	ERxPachubeDataOut(const char* APIKey, const char* feedId);

public:

	// PUT data to Pachube in order to update a feed 
	// (for example when your IP address is not externally accessible 
	// and/or you want to update your datastreams manually).
	// Wait until get the response, return http status code. 
	// 0 - Unknown error
	// 1 - Can't connect to Pachube server
	// 3 - Local data stream is empty.
	// 200 means success/OK.
	int				updatePachube() const;
};

// GET the remote data from Pachube server.
class ERxPachubeDataIn : public ERxPachube
{
public:
	ERxPachubeDataIn(const char* APIKey, unsigned int feedId);
	ERxPachubeDataIn(const char* APIKey, const char* feedId);

public: 

	// GET the remote data from server.
	// Wait until get the response, return http status code.
	// 0 - Unknown error
	// 1 - Can't connect to Pachube server
	// 200 means success/OK.
	int				syncPachube();     
};

#endif //ERXPACHUBE_H
