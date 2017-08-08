package com.dancemaster.dancemaster.backend;

import java.io.IOException;

import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

/**
 * A Servlet to tally the points of this user.
 */
public class AddPointsServlet extends HttpServlet {
    @Override
    protected void doGet(HttpServletRequest request, HttpServletResponse response)
            throws IOException, ServletException {

        // Get the phone's registration ID and match it to a registration record and saved user object
        String regID = request.getParameter("regID");
        RegistrationRecord registration = DataStore.findRegistration(regID);
        User user = DataStore.findUser(registration.getUsername());

        // Get the points information and update the user and save it the data store
        String date = request.getParameter("date");
        String points = request.getParameter("points");
        user.addPoint(date, Integer.parseInt(points));
        DataStore.update(user);
    }


    @Override
    protected void doPost(HttpServletRequest request, HttpServletResponse response)
            throws IOException, ServletException {
        doGet(request, response);
    }
}