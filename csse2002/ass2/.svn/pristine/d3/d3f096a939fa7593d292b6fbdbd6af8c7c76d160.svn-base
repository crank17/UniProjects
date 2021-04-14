package tms.congestion;

import tms.sensors.Sensor;

import java.util.ArrayList;
import java.util.List;

import static java.lang.Math.round;

public class AveragingCongestionCalculator implements CongestionCalculator {
    private List<Sensor> sensorList;


    public AveragingCongestionCalculator(List<Sensor> sensors){
        sensorList = sensors;

    }
    public int calculateCongestion(){
        int sensorCongestion = 0;
        int averageCongestion = 0;
        if(sensorList.size() != 0) {
            for (Sensor sensor : sensorList) {
                sensorCongestion += sensor.getCongestion();
            }
            averageCongestion = sensorCongestion / sensorList.size();
        }

        return round(averageCongestion);

    }
}
