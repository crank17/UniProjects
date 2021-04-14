package tms.sensors;

import tms.util.TimedItem;

import java.util.Arrays;

public abstract class DemoSensor implements TimedItem {
    int[] data;
    int threshold;
    int counter = 0;

    protected DemoSensor(int[] data, int threshold){


        this.data = data;
        this.threshold = threshold;

    }
    protected int getCurrentValue(){
        return this.data[counter];
    }

    public int getThreshold(){
        return this.threshold;

    }

    public void oneSecond(){

        if (counter > data.length){
            counter = 0;
        }
        else{
            counter++;
        }
        getCurrentValue();
    }
    @Override
    public String toString(){
        return threshold + ":" + Arrays.toString(data);
    }
}
