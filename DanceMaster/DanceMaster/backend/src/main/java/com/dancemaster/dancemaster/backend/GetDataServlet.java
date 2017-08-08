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
 * A Servlet to get data from the server.
 */
public class GetDataServlet extends HttpServlet {
    @Override
    protected void doGet(HttpServletRequest request, HttpServletResponse response)
            throws IOException, ServletException {

        // Get the phone's registration ID and match it to a
        // registration record and saved user object
        String regID = request.getParameter("regID");
        RegistrationRecord record = DataStore.findRegistration(regID);
        User user = DataStore.findUser(record.getUsername());

        // For each friend in the user's friend list, get that user's points for the week
        try {
            JSONArray ja = new JSONArray();
            ja.put(new JSONObject().put("method", "data"));

            JSONArray data = new JSONArray();
            if (user.getFriends() != null) {
                for (String friend_username : user.getFriends()) {
                    // Get the friend's user profile from the data store
                    User friend = DataStore.findUser(friend_username);

                    // Add the friend's points and username to the JSON array
                    data.put(new JSONObject().put("username", friend_username));
                    data.put(new JSONObject().put("points", friend.pointsPerWeek()));
                }
            }

            // Add the data to the JSON array to send
            ja.put(data);

            // Send the completed message to the phone
            MessagingEndpoint messenger = new MessagingEndpoint();
            messenger.sendMessage(ja.toString(), record);

        } catch (JSONException e) {
            e.printStackTrace();
        }
    }

    @Override
    protected void doPost(HttpServletRequest request, HttpServletResponse response)
            throws IOException, ServletException {
        doGet(request, response);
    }
}