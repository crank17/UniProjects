package tms.sensors;

import java.util.Arrays;

import static java.lang.StrictMath.round;

public class DemoSpeedCamera extends DemoSensor implements SpeedCamera {
    public DemoSpeedCamera(int[] data, int threshold){

        super(data, threshold);
    }

    public int averageSpeed(){
        return getCurrentValue();
    }

    public int getCongestion(){
        float congestionRate = ((float) averageSpeed() / (float) getThreshold()) * 100;
        return Math.round(congestionRate);
    }

    @Override
    public String toString(){
        return "SC:" + threshold + ":" + Arrays.toString(data);
    }

}
