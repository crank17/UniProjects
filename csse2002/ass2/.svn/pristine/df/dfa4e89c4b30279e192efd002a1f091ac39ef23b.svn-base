package tms.sensors;

public class DemoVehicleCount extends DemoSensor implements VehicleCount {

    public DemoVehicleCount(int[] data, int threshold)  {
        super(data, threshold);

    }

    public int countTraffic(){
        return this.getCurrentValue();

    }
    public int getCongestion(){
        float congestion = (float) this.countTraffic() / this.getThreshold();
        int congestionval = Math.round(100 - 100 * congestion);
        return Math.min(Math.max(congestionval, 0), 100);

    }
    public String toString(){
        return "VC" + ":" + super.toString();

    }
}
