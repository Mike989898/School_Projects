package com.dancemaster.dancemaster.backend;

import com.google.appengine.labs.repackaged.org.json.JSONArray;
import com.google.appengine.labs.repackaged.org.json.JSONException;
import com.google.appengine.labs.repackaged.org.json.JSONObject;

import java.io.IOException;

import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

/**
 * A servlet to check if the given username is a real user,
 * and if so, to add it as a friend.
 */
public class AddFriendServlet extends HttpServlet {

    @Override
    protected void doGet(HttpServletRequest request, HttpServletResponse response)
            throws IOException, ServletException {

        // Get the proposed username to follow and the phone's registration ID
        String username = request.getParameter("user");
        String regID = request.getParameter("regID");

        // Get the registration record of the phone and match it to the saved user entry
        RegistrationRecord record = DataStore.findRegistration(regID);
        User user = DataStore.findUser(record.getUsername());

        // Check if the requested username is a registered user and not already added
        User newFriend = DataStore.findUser(username);

        if (newFriend != null && user.isFriend(newFriend)) {
            try {
                // Create the message
                JSONArray ja = new JSONArray();
                ja.put(new JSONObject().put("method", "add"));
                ja.put(new JSONObject().put("result", "already_added"));

                // Send the message
                MessagingEndpoint messenger = new MessagingEndpoint();
                messenger.sendMessage(ja.toString(), record);

            } catch (JSONException e) {
                e.printStackTrace();
            }
        }

        // If there is no such user, send the response to the phone
        else if (newFriend == null) {
            try {
                // Create the message
                JSONArray ja = new JSONArray();
                ja.put(new JSONObject().put("method", "add"));
                ja.put(new JSONObject().put("result", "none"));

                // Send the message
                MessagingEndpoint messenger = new MessagingEndpoint();
                messenger.sendMessage(ja.toString(), record);

            } catch (JSONException e) {
                e.printStackTrace();
            }
        }

        // Otherwise, add that user to this user's friends list, then send the response to the phone
        else {
            // Add the new friend and update the user in the data store
            user.addFriend(username);
            DataStore.update(user);

            try {
                // Create the message to send to the phone
                JSONArray ja = new JSONArray();
                ja.put(new JSONObject().put("method", "add"));
                ja.put(new JSONObject().put("result", "done"));

                // Send the message
                MessagingEndpoint messenger = new MessagingEndpoint();
                messenger.sendMessage(ja.toString(), record);

            } catch (JSONException e) {
                e.printStackTrace();
            }
        }
    }


    @Override
    protected void doPost(HttpServletRequest request, HttpServletResponse response)
            throws IOException, ServletException {
        doGet(request, response);
    }
}