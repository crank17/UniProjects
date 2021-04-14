package tms.network;

import tms.display.MainViewModel;
import tms.intersection.Intersection;
import tms.sensors.DemoPressurePad;
import tms.sensors.DemoSpeedCamera;
import tms.sensors.DemoVehicleCount;
import tms.sensors.Sensor;
import tms.util.DuplicateSensorException;
import tms.util.IntersectionNotFoundException;
import tms.util.InvalidNetworkException;
import tms.util.RouteNotFoundException;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

public class NetworkInitialiser {

    public NetworkInitialiser(){

    }
    public static final String LINE_INFO_SEPARATOR = ":";
    public static final String LINE_LIST_SEPARATOR = ";";

    public static Network loadNetwork(String filename) throws IOException, InvalidNetworkException{
            BufferedReader textReader = new BufferedReader(new FileReader(filename));
            ArrayList<String> textData = new ArrayList<>(); //list representing each line of data
            String line;
            while ((line = textReader.readLine()) != null) {
                if (!line.contains(LINE_LIST_SEPARATOR)) textData.add(line);
            }

            // close the line-by-line reader
            textReader.close();

            int numIntersections = Integer.parseInt(textData.get(0));
            int numRoutes = Integer.parseInt(textData.get(1));
            int yellowTime = Integer.parseInt(textData.get(2));
            List<String[]> intersections = new ArrayList<>();
            List<String[]> routes = new ArrayList<>();
            List<String[]> sensors = new ArrayList<>();

            for (int i = 3; i <= numIntersections + 2; i++) {
                String string = textData.get(i);
                if (string.contains(":")) {
                    String[] delimString = string.split("[:]");
                    intersections.add(delimString);
                }
                else{
                    String[] id = new String[] {textData.get(i)};
                    intersections.add(id);
                }
            }
            for (int i = 3+numIntersections; i <= textData.size() - 1; i++){
                String string = textData.get(i);
                String[] delimString = string.split("[:]");
                if (delimString.length >= 4){
                    routes.add(delimString);
                }
                else {
                    sensors.add(delimString);
                }
            }

            Network network = new Network();
            network.setYellowTime(yellowTime);

            try {
                for (String[] intersection : intersections) {
                    //Use replaceAll to remove square brackets around data
                    String removeSquares = Arrays.toString(intersection).replaceAll("\\[(.*?)|\\]", "");
                    String[] delimIntersection = removeSquares.split("[,]", 3);
                    network.createIntersection(delimIntersection[0]);

                    if (delimIntersection.length == 2) {
                       network.changeLightDuration(delimIntersection[0], Integer.parseInt(delimIntersection[1]));

                    }
                }

                }catch(IntersectionNotFoundException e) {
                e.printStackTrace();
            }
            int counter = 0;
            int sensCount = 0;
            ;
            try {
                    for (String[] route : routes) {
                        String removeSquares = Arrays.toString(route).replaceAll("\\[(.*?)|\\]", "").trim();
                        String[] delimRoute = removeSquares.split("[,]");
                        int sensorNum = Integer.parseInt(delimRoute[3].strip());
                        int defaultSpeed = Integer.parseInt(delimRoute[2].strip());
                        network.connectIntersections(delimRoute[0].trim(), delimRoute[1].trim(), defaultSpeed);
                        network.addSpeedSign(delimRoute[0].strip(), delimRoute[1].strip(), defaultSpeed);
                        if (sensorNum != 0) {
                            Sensor sensor1;
                            sensCount += sensorNum;
                            for (int i = counter; i < sensCount; i++ ) {

                                String addSens = Arrays.toString(sensors.get(i)).replaceAll("\\[(.*?)|\\]", "");
                                String sensType = addSens.split(",")[0];
                                String[] splitData = addSens.split(",", 3)[2].split(",");
                                int[] data = new int[splitData.length];
                                for (int j = 0; j < addSens.split(",").length - 2; j++) {
                                    data[j] = Integer.parseInt(splitData[j].strip());


                                }
                                int thresh = Integer.parseInt(addSens.split(",")[1].strip());

                                if (sensType.equals("PP")) {
                                    sensor1 = new DemoPressurePad(data, thresh);
                                    network.addSensor(delimRoute[0], delimRoute[1].strip(), sensor1);
                                }
                                if (sensType.equals("SC")){
                                    sensor1 = new DemoSpeedCamera(data, thresh);
                                    network.addSensor(delimRoute[0], delimRoute[1].strip(), sensor1);
                                }
                                if (sensType.equals("VC")){
                                    sensor1 = new DemoVehicleCount(data, thresh);
                                    network.addSensor(delimRoute[0], delimRoute[1].strip(), sensor1);
                                }
                                counter++;
                                //sensorNum += sensorNum;

                            }
                        }
                        if (route.length == 5) {

                            network.setSpeedLimit(delimRoute[0].strip(), delimRoute[1].strip(), Integer.parseInt(delimRoute[4].strip()));
                        }


                    }
                } catch (RouteNotFoundException | IntersectionNotFoundException | DuplicateSensorException e) {
                e.printStackTrace();
            }
            System.out.println(network.getIntersections().toString());
            try {
                System.out.println(network.getConnection("Z", "X").getSensors());
            }catch (IntersectionNotFoundException | RouteNotFoundException e){
                e.printStackTrace();
            }
            System.out.println(network);


            return network;
    }
}
