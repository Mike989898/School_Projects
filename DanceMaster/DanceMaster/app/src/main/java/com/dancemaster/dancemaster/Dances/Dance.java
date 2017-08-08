package com.dancemaster.dancemaster.Dances;

/**
 * Dance.java is a class representing representing a Dance object
 * A Dance object has a title, description, and two video codes:
 * one for an instructional video and one for a dancing video!
 */
public class Dance {
    private String title;           // A short title that also acts as its unique identifier
    private String description;     // A brief description for the list view
    private String videoCode;       // The code to access the youtube video with the instructional video
    private String videoCodeTrain;

    public Dance(String title, String description, String videoCodeTrain, String videoCode) {
        this.title = title;
        this.description = description;
        this.videoCode = videoCode;
        this.videoCodeTrain = videoCodeTrain;
    }

    public String getTitle() {
        return title;
    }

    public void setTitle(String title) {
        this.title = title;
    }

    public String getDescription() {
        return description;
    }

    public String getVideoCode() {
        return videoCode;
    }

    public String getVideoCodeTrain() {
        return videoCodeTrain;
    }

}
