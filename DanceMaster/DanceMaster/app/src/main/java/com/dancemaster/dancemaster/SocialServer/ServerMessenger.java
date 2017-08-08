package com.dancemaster.dancemaster.SocialServer;

import android.util.Log;

import java.io.IOException;
import java.io.OutputStream;
import java.net.HttpURLConnection;
import java.net.MalformedURLException;
import java.net.URL;
import java.util.Iterator;
import java.util.Map;

/**
 * A helper class to send messages to the server
 */
public class ServerMessenger {
    public static void send(String endpoint, Map<String, String> params)
            throws IOException {

        // Create a url link to the endpoint of the server
        URL url;
        try {
            url = new URL(endpoint);
        } catch (MalformedURLException e) {
            throw new IllegalArgumentException("invalid url: " + endpoint);
        }

        // Turn the map of parameters into a string to post to the server
        StringBuilder bodyBuilder = new StringBuilder();
        Iterator<Map.Entry<String, String>> iterator = params.entrySet().iterator();
        while (iterator.hasNext()) {
            Map.Entry<String, String> param = iterator.next();
            bodyBuilder.append(param.getKey()).append('=')
                    .append(param.getValue());
            if (iterator.hasNext()) {
                bodyBuilder.append('&');
            }
        }

        // Convert the string to bytes
        String body = bodyBuilder.toString();
        byte[] bytes = body.getBytes();

        // Connect to the server and post the request
        HttpURLConnection connection = null;
        try {
            // Set up the connection
            connection = (HttpURLConnection) url.openConnection();
            connection.setDoOutput(true);
            connection.setUseCaches(false);
            connection.setFixedLengthStreamingMode(bytes.length);
            connection.setRequestMethod("POST");
            connection.setRequestProperty("Content-Type",
                    "application/x-www-form-urlencoded;charset=UTF-8");

            // Post the request via the connection
            OutputStream outputStream = connection.getOutputStream();
            outputStream.write(bytes);
            outputStream.close();

            // Handle error responses from the server
            if (connection.getResponseCode() != 200) {
                Log.d("DanceMaster", Integer.toString(connection.getResponseCode()));
                throw new IOException();
            }
        } finally {
            // Close the connection if necessary
            if (connection != null) {
                connection.disconnect();
            }
        }
    }
}
