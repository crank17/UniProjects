/**
 * Class A2Event
 * DO NOT MODIFY THIS CLASS
 *
 * This class is used to print all events happening during the lifetime of your program
 *
 * NOTE : Please make sure that you have called EVERY event method below in your main program 
 * when ever such an event happens.
 * This class will be replaced with a similar class that will validate the program flow 
 * automatically.
 *
 */
public class A2Event {

    /**
     * Call this event method when a reader reads the current data
     *
     * @param reader id (value between 0 and 4 inclusive)
     * @param x1 (the value of x1 read)
     * @param x2 (the value for x2 read)
     *
     */
    public static void readData(int reader, int x1, int x2){
        System.out.println("Reader #"+reader+" has read values: x1 "+x1+" x2 "+x2);
    }


    /**
     * Call this event method when a writer writes to the data
     *
     * @param writer id (value between 0 and 4 inclusive)
     * @param int x1 (the value written to x1)
     * @param int x2 (the value written to x2)
     *
     */
    public static void writeData(int writer, int x1, int x2){
        System.out.println("Writer #"+writer+" has written values: x1 "+x1+" x2 "+x2);
    }

    /**
     * Call this event method when an incrementer increments the data
     *
     * @param incrementer id (value between 0 and 4 inclusive)
     * @param int x1 (the incremented value of x1)
     * @param int x2 (the incremented value of x2)
     *
     */
    public static void incrementData(int incrementer, int x1, int x2){
        System.out.println("Incrementer #"+incrementer+" has incremented values: x1 "+x1+" x2 "+x2);
    }

    /**
     * Call this event method when all 5 readers have read the final result
     *
     */
    public static void terminateReadWrite(){
        System.out.println("All readers have read the final data.");
    }

}
