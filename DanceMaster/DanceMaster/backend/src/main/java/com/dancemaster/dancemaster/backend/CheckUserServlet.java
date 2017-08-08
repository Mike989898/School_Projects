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
 * A servlet to query the data store to find if the username is unique
 */
public class CheckUserServlet extends HttpServlet {

    @Override
    protected void doGet(HttpServletRequest request, HttpServletResponse response)
            throws IOException, ServletException {
        // Get the phone's registration ID and the proposed username
        String username = request.getParameter("user");
        String regID = request.getParameter("regID");

        // Check the username in the data store
        boolean available = DataStore.checkUsername(username, regID);

        // Get the registration record of the phone
        RegistrationRecord record = DataStore.findRegistration(regID);

        // Create the response to send back to the app
        try {
            JSONArray ja = new JSONArray();
            ja.put(new JSONObject().put("method", "username"));
            ja.put(new JSONObject().put("result",Boolean.toString(available)));

            // Send the message to the app
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
