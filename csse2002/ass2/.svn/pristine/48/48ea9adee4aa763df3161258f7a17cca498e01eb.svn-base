package tms.network;

import tms.intersection.Intersection;
import tms.route.Route;
import tms.sensors.Sensor;
import tms.util.DuplicateSensorException;
import tms.util.IntersectionNotFoundException;
import tms.util.InvalidOrderException;
import tms.util.RouteNotFoundException;

import java.util.ArrayList;
import java.util.List;
import java.util.Objects;

public class Network {
    private int yellowTime = 1;
    private List<Route> connections;
    private List<Intersection> connectedIntersections = new ArrayList<>();
    public Network(){

    }

    public int getYellowTime(){
        return yellowTime;

    }

    public void setYellowTime(int yellowTime) throws IllegalArgumentException {
        if (yellowTime < 1){
            throw new IllegalArgumentException("yellowTime must be > 1");

        }
        this.yellowTime = yellowTime;
    }

    public void createIntersection(String id) throws IllegalArgumentException {
        for (Intersection intersection : connectedIntersections){
            if (intersection.getId().equals(id))
                throw new IllegalArgumentException("Intersection already exists");
        }
        if (id.contains(":"))
            throw new IllegalArgumentException("Invalid ID");
        if (id.isBlank())
            throw new IllegalArgumentException("ID is empty");


        Intersection intersection = new Intersection(id);
        connectedIntersections.add(intersection);


    }

    public void connectIntersections(String from, String to, int defaultSpeed) throws IntersectionNotFoundException,
            IllegalStateException, IllegalArgumentException{
        if (defaultSpeed < 0) {
            throw new IllegalArgumentException("Speed must be positive");
        }
        try {
            findIntersection(from);
            findIntersection(to);

        } catch (IntersectionNotFoundException e){
            throw new IntersectionNotFoundException("Intersection does not exist");
        }


        Intersection intersectionTo = findIntersection(to);
        Intersection intersectionFrom = findIntersection(from);
        intersectionTo.addConnection(intersectionFrom, defaultSpeed);


    }

    public void addLights(String intersectionId, int duration, List<String> intersectionOrder) throws
            IntersectionNotFoundException, InvalidOrderException, IllegalArgumentException {
        List<Route> order = new ArrayList<>();
        List<Route> incomingRoutes = findIntersection(intersectionId).getConnections();


        try{
            findIntersection(intersectionId);


        } catch (IntersectionNotFoundException e){
            throw new IntersectionNotFoundException("Intersection does not exist");
        }
        if (duration < yellowTime + 1){
            throw new IllegalArgumentException("Duration must be greater than yellowTime + 1");
        }

        for(String intersection : intersectionOrder){
            for (Route route : incomingRoutes){
                if (route.getFrom() == findIntersection(intersection))
                    order.add(route);


            }
        }

        if (intersectionOrder.size() == 0 || !(order.containsAll(incomingRoutes))){
            throw new InvalidOrderException("The given list is either empty or not a permutation");
        }
        findIntersection(intersectionId).addTrafficLights(order, yellowTime, duration);

    }
    public void addSpeedSign(String from, String to, int initialSpeed) throws IntersectionNotFoundException, RouteNotFoundException{
        Route speed = getConnection(from, to);
        speed.addSpeedSign(initialSpeed);


    }

    public void setSpeedLimit(String from, String to, int newLimit) throws IntersectionNotFoundException, RouteNotFoundException{
        getConnection(from, to).setSpeedLimit(newLimit);

    }

    public void changeLightDuration(String intersectionId, int duration) throws IntersectionNotFoundException{

    }
    public Route getConnection(String from, String to) throws IntersectionNotFoundException, RouteNotFoundException{
        try {
            findIntersection(to);
        } catch (IntersectionNotFoundException e){
            throw new IntersectionNotFoundException("intersection with id 'to' doesn't exist");
        }
        return findIntersection(to).getConnection(findIntersection(from));

        }


    public void addSensor(String from, String to, Sensor sensor) throws IntersectionNotFoundException,
            RouteNotFoundException, DuplicateSensorException {
        getConnection(from, to).addSensor(sensor);



    }

    public int getCongestion(String from, String to) throws IntersectionNotFoundException, RouteNotFoundException{
        Route route = getConnection(from, to);
        return route.getCongestion();
    }

    public Intersection findIntersection(String id) throws IntersectionNotFoundException{
        for (Intersection intersection : connectedIntersections){
            if (intersection.getId().equals(id)){
                return intersection;
            }
        }
        throw new IntersectionNotFoundException("No intersection was found");

    }

    public void makeTwoWay(String from, String to) throws IntersectionNotFoundException, RouteNotFoundException{
        try{
            findIntersection(from);
            findIntersection(to);
        }catch (IntersectionNotFoundException e){
            e.printStackTrace();
        }
        Route existingRoute = getConnection(from, to);
        int speed = existingRoute.getSpeed();
        connectIntersections(to, from, speed);
        if (existingRoute.hasSpeedSign()){
            getConnection(to, from).addSpeedSign(speed);
        }
  }
    public boolean equals(Object obj){
        if (this == obj)
            return true;
        if (obj == null || this.getClass() != obj.getClass())
            return false;
        Network networkObject = (Network) obj;


        return Objects.equals(connectedIntersections.size(), networkObject.connectedIntersections.size())
                && Objects.equals(connectedIntersections, networkObject.connectedIntersections);

    }
    public int hashCode(){
        return Objects.hash(connectedIntersections.size(), connectedIntersections);
    }
    public String toString(){
        StringBuilder stringBuilder = new StringBuilder();
        Object newline = System.lineSeparator();
        stringBuilder.append(getIntersections().size()).append(newline);
        List<Route> routes = new ArrayList<>();
        for (Intersection intersection: connectedIntersections){
            routes.addAll(intersection.getConnections());
        }
        stringBuilder.append(routes.size()).append(newline);
        stringBuilder.append(yellowTime).append(newline);
        String[] intersections = this.connectedIntersections.stream().map(Object::toString)
                .sorted().toArray(String[]::new);
        for (String intersection : intersections) {
            stringBuilder.append(intersection).append(newline);
        }
        String [] routeStrings = routes.stream().map(Object::toString)
                .sorted().toArray(String[]::new);
        for (String route: routeStrings) {
            stringBuilder.append(route).append(newline);
        }

        return stringBuilder.toString();

    }
    public List<Intersection> getIntersections(){
        return connectedIntersections;
    }
}
