package com.dancemaster.dancemaster.backend;

import com.google.appengine.api.datastore.DatastoreService;
import com.google.appengine.api.datastore.DatastoreServiceFactory;
import com.google.appengine.api.datastore.Entity;
import com.google.appengine.api.datastore.Key;
import com.google.appengine.api.datastore.KeyFactory;

import java.util.ArrayList;

/**
 * A data store to store both user objects (with key username and data about the user) and
 * registration objects (with key registration id and data username)
 */
public class DataStore {
    private static final DatastoreService datastore = DatastoreServiceFactory
            .getDatastoreService();

    private static final Key getUserKey() {
        return KeyFactory.createKey(User.USER_PARENT_ENTITY_NAME,
                User.USER_PARENT_KEY_NAME);
    }

    private static final Key getRegistrationKey() {
        return KeyFactory.createKey(RegistrationRecord.REGISTRATION_PARENT_ENTITY_NAME,
                RegistrationRecord.REGISTRATION_PARENT_KEY_NAME);
    }


    //************ registration entities ************//
    public static void addRegistration(RegistrationRecord registration) {
        // Create a new entity
        Entity entity = new Entity(RegistrationRecord.REGISTRATION_ENTITY_NAME,
                registration.getRegId(), getRegistrationKey());

        // Set the properties of the entity
        entity.setProperty(RegistrationRecord.FIELD_NAME_REGID, registration.getRegId());
        entity.setProperty(RegistrationRecord.FIELD_NAME_USERNAME, registration.getUsername());

        // Save the entity to the data store
        datastore.put(entity);
    }

    public static RegistrationRecord findRegistration(String regID) {
        Entity result = null;
        try {
            result = datastore.get(KeyFactory.createKey(getRegistrationKey(),
                    RegistrationRecord.REGISTRATION_ENTITY_NAME, regID));
        } catch (Exception ex) {
            ex.printStackTrace();
        }
        // Return the entity as a registration record
        return entityToRegistration(result);
    }

    public static void update(RegistrationRecord record) {
        try {
            // Find the correct entity from the data store
            Entity entity = datastore.get(KeyFactory.createKey(getRegistrationKey(),
                    RegistrationRecord.REGISTRATION_ENTITY_NAME, record.getRegId()));

            // Set its properties to the updated properties of the new user object
            entity.setProperty(RegistrationRecord.FIELD_NAME_USERNAME, record.getUsername());

            // Save the updated entity back into the data store
            datastore.put(entity);

        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private static RegistrationRecord entityToRegistration(Entity entity) {
        if (entity == null) {
            return null;
        }

        RegistrationRecord registration = new RegistrationRecord();
        registration.setRegId((String) entity.getProperty(RegistrationRecord.FIELD_NAME_REGID));
        registration.setUsername((String) entity.getProperty(
                RegistrationRecord.FIELD_NAME_USERNAME));
        return registration;
    }


    //************ user entities ************//
    public static void addUser(User user) {
        // Create a new entity
        Entity entity = new Entity(User.USER_ENTITY_NAME,
                user.getUsername(), getUserKey());

        // Set all the properties of the entity
        entity.setProperty(User.FIELD_NAME_USERNAME, user.getUsername());
        entity.setProperty(User.FIELD_NAME_REGID, user.getRegID());
        entity.setProperty(User.FIELD_NAME_FRIENDS, user.getFriends());
        entity.setProperty(User.FIELD_NAME_POINTS, user.JSONofPoints());

        // Save the entity to the data store
        datastore.put(entity);
    }


    public static User findUser(String username) {
        Entity result = null;
        try {
            result = datastore.get(KeyFactory.createKey(getUserKey(),
                    User.USER_ENTITY_NAME, username));
        } catch (Exception ex) {
            ex.printStackTrace();
        }

        // Return the entity as a user
        return entityToUser(result);
    }


    public static void update(User user) {
        try {
            // Find the correct entity from the data store
            Entity entity = datastore.get(KeyFactory.createKey(getUserKey(),
                    User.USER_ENTITY_NAME, user.getUsername()));

            // Set its properties to the updated properties of the new user object
            entity.setProperty(User.FIELD_NAME_FRIENDS, user.getFriends());
            entity.setProperty(User.FIELD_NAME_POINTS, user.JSONofPoints());

            // Save the updated entity back into the data store
            datastore.put(entity);

        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public static boolean checkUsername(String username, String regID) {
        User user = findUser(username);

        // If that user has a different registration,
        // then the username is taken, so return false
        if ((user != null) && (!user.getRegID().equals(regID))) {
            return false;
        }

        // Otherwise it's fine, so save it to the data store and return true
        newEntry(username, regID);
        return true;
    }

    private static void newEntry(String username, String regID) {
        // Create a new User object and save it to the data store
        User user = new User();
        user.setUsername(username);
        user.setRegID(regID);
        addUser(user);

        // Update the registration record to include the username
        RegistrationRecord record = new RegistrationRecord();
        record.setRegId(regID);
        record.setUsername(username);
        update(record);
    }

    private static User entityToUser(Entity entity) {
        if (entity == null) {
            return null;
        }
        User user = new User();
        user.setUsername((String) entity.getProperty(User.FIELD_NAME_USERNAME));
        user.setRegID((String) entity.getProperty(User.FIELD_NAME_REGID));
        user.setFriends((ArrayList<String>) entity.getProperty(User.FIELD_NAME_FRIENDS));
        user.setPoints((String) entity.getProperty(User.FIELD_NAME_POINTS));

        return user;
    }
}