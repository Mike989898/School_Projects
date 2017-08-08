#ifndef __MISSION_H
#define __MISSION_H

/* 
 * AppMessage keys (***DO NOT MODIFY***).
 *
 * You can communicate with the companion smartphone over Bluetooth using data 
 * stored in a dictionary data structure. The "keys" are important and must be 
 * well-defined between the Pebble and its companion app running on your smartphone
 * in order for communication to work properly. 
 *
 * NOTE:
 * -----
 * You should **NOT** need to add new keys -- if you think you do, please email 
 * Travis (traviswp@cs.dartmouth.edu) to discuss if this is actually necessary/supported.
 * Currently, the provided Pebble JS app only supports the below keys and you 
 * risk breaking the code if you try to change any of these keys.

 * See the Pebble "Communication" documentation for more information.
 * -> https://developer.pebble.com/guides/communication/
 */
enum {
  AppKeyJSReady = 0,      // The JS environment on the companion smartphone is up and running!
  AppKeySendMsg,          // Send a message over Bluetooth to the companion smartphone and then on to the Game Server
  AppKeyRecvMsg,          // A message from the Game Server is available (arrived over Bluetooth)
  AppKeyLocation,         // Request your GPS location from the companion smartphone
  AppKeyPebbleId,         // Request your unique pebble ID from the companion smartphone
  AppKeySendError         // Error: companion app can't connect to the Proxy (and ultimately the Game Server).
};

/*
 * Convenience LOG definitions that use the underlying app_log.
 * @param fmt A C formatting string
 * @param args The arguments for the formatting string
 */
#define LOG(fmt, args...) \
  app_log(APP_LOG_LEVEL_DEBUG, __FILE_NAME__, __LINE__, fmt, ## args)

#define LOG_ERROR(fmt, args...) \
  app_log(APP_LOG_LEVEL_ERROR, __FILE_NAME__, __LINE__, fmt, ## args)

#endif // __MISSION_H
