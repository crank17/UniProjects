package tms.intersection;

import tms.route.Route;
import tms.route.TrafficSignal;
import tms.util.TimedItem;
import tms.util.TimedItemManager;

import java.lang.reflect.Array;
import java.util.Arrays;
import java.util.GregorianCalendar;
import java.util.List;

public class IntersectionLights {
    private List<Route> connections;
    private int yellowTime;
    private int duration;
    private int secondsPassed;
    public IntersectionLights(List<Route> connections, int yellowTime, int duration){
        this.connections = connections;
        this.yellowTime = yellowTime;
        this.duration = duration;
        this.secondsPassed = 0;

    }
    public int getYellowTime(){
        return yellowTime;

    }

    /**
     * Sets a new duration of each green-yellow cycle.
     * The current progress of the lights cycle should be reset, such that on the next call to oneSecond(),
     * only one second of the new duration has been elapsed for the incoming route that currently has a green light.
     * @param duration
     * @require duration > getYellowTime()
     */
    public void setDuration(int duration){
        this.duration = duration;

    }
    public void oneSecond() {
        secondsPassed++;
        for (int i = 0; i <= connections.size(); i++) {
            if (secondsPassed >= duration || connections.get(i).getTrafficLight().getSignal() == TrafficSignal.YELLOW) {
                if (connections.get(i).getTrafficLight().getSignal() == TrafficSignal.GREEN && secondsPassed < duration - yellowTime)
                    connections.get(i).setSignal(TrafficSignal.YELLOW);
                if (connections.get(i).getTrafficLight().getSignal() == TrafficSignal.YELLOW && secondsPassed < yellowTime) {
                    connections.get(i).setSignal(TrafficSignal.RED);
                    if (i < connections.size()) {
                        connections.get(i + 1).setSignal(TrafficSignal.GREEN);
                    } else {
                        connections.get(0).setSignal(TrafficSignal.GREEN);
                    }
                }
            }
        }
    }
    public String toString(){
        String[] array = new String[connections.size()];
        int index = 0;
        for (Route value : connections) {
            array[index] = value.getFrom().getId();
            index++;
        }

        return Integer.toString(duration) + Arrays.toString(array);

    }

}
