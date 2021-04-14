package tms.route;

public class TrafficLight {
    TrafficSignal lightSignal;

    public TrafficLight(){
        lightSignal = TrafficSignal.RED;
    }

    public TrafficSignal getSignal(){
        return lightSignal;
    }

    public void setSignal(TrafficSignal signal){
        lightSignal = signal;
    }
}
