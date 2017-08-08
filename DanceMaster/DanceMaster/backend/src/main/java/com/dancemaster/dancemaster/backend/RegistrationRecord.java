package com.dancemaster.dancemaster.backend;

import com.googlecode.objectify.annotation.Entity;
import com.googlecode.objectify.annotation.Id;
import com.googlecode.objectify.annotation.Index;

/** The Objectify object model for device registrations we are persisting */
@Entity
public class RegistrationRecord {

    @Id
    Long id;

    @Index
    private String regId;
    private String username;

    public static final String REGISTRATION_PARENT_ENTITY_NAME = "RegistrationParent";
    public static final String REGISTRATION_PARENT_KEY_NAME = "RegistrationParent";
    public static final String REGISTRATION_ENTITY_NAME = "Registration";

    public static final String FIELD_NAME_REGID = "regID";
    public static final String FIELD_NAME_USERNAME = "username";

    public RegistrationRecord() {}

    public String getRegId() {
        return regId;
    }

    public void setRegId(String regId) {
        this.regId = regId;
    }

    public String getUsername() {
        return username;
    }

    public void setUsername(String username) {
        this.username = username;
    }
}