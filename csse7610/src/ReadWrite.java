import java.util.concurrent.locks.Condition;
import java.util.concurrent.locks.ReentrantLock;

/** ReadWrite.java - for CSSE7610 Assignment 2
 * Ryan Ward 45813685
 **/

/** Increment-Write Monitor class, used for handling synchronisation functions **/
class IWMonitor {
    volatile int numWriters; // Keep track of the number of writers
    volatile int numIncrementors; // Keep track of number of incrementors
    volatile int waitingWriters = 0; // Keeps track of waiting writers
    ReentrantLock mutex = new ReentrantLock(true);  // Reentrant lock
    Condition writeOK = mutex.newCondition(); // Reentrant lock condition
    Condition incrementOK = mutex.newCondition(); // Reentrant lock condition


    /** Helper function to synchronise the Writer threads
     * Uses mutex and writeOK condition for synchronisation **/
    public void startWrite() throws InterruptedException {
        mutex.lock();
        try {
            // wait for any completing incrementors or writers
            if (numWriters != 0 || numIncrementors != 0) {
                waitingWriters++; // Increment writers waiting
                writeOK.await();
                waitingWriters--; // Remove a waiting writer

            }

            numWriters++;
        } finally {
            mutex.unlock();
        }
    }
    /** Helper function to synchronise Writer threads **/
    public void endWrite() throws InterruptedException {
        mutex.lock();
        try {
            numWriters--; // decrement total writers

            if (waitingWriters == 0) {
                incrementOK.signalAll(); // signal to all waiting incrementors
            } else {
                writeOK.signal(); // if there are waiting writers, signal one
            }
        } finally {
            mutex.unlock();
        }
    }

    /** Helper function to synchronise Increment threads **/
    public void startIncrement() throws InterruptedException {
        mutex.lock();
        try {
            // Wait for writers and other incrementors to finish
            if (waitingWriters != 0 || numWriters != 0 || numIncrementors != 0) {
                incrementOK.await();
            }
            numIncrementors++; // increment incrementors
        } finally {
            mutex.unlock();
        }
    }

    /** Helper function to synchronise Increment threads **/
    public void endIncrement() throws InterruptedException {
        mutex.lock();
        try {
            numIncrementors--; // decrement total incrementors
            // if there are waiting writers and no more incrementors, signal writer
            if (numIncrementors == 0 || waitingWriters != 0) {
                writeOK.signal();

            } else {
                // otherwise signal to any incrementor
                incrementOK.signalAll();
            }
        } finally {
            mutex.unlock();
        }
    }
}

/** Helper class to store all shared variables by processes **/
class SharedValues {
    public volatile int c;
    public volatile int x1;
    public volatile int x2;

    public int killFlag = 0; // keep track of total writes/increments

    public SharedValues() {
        c = 0;
        x1 = 0;
        x2 = 0;
    }
}

/** Reader thread **/
class A2Reader extends Thread {
    int id; //thread id
    SharedValues sv;
    int d1 = 0;
    int d2 = 0;
    int c0;

    public A2Reader(int _id, SharedValues _sv) {
        sv = _sv;
        id = _id;
    }
    /** Reader thread implements the algorithm as seen in Assignment spec **/
    public synchronized void run() {
        try {
            Thread.sleep((long) (Math.random() * 10));
            while (true) {

                do { // while(cond){...} doesn't work - causes hang
                    do { // see above
                        c0 = sv.c;
                    } while(c0 % 2 != 0);
                    d1 = sv.x1;
                    d2 = sv.x2;

                } while (c0 != sv.c);
                // advertise event before checking if killflag has been met
                 A2Event.readData(id, d1, d2);
                 if (sv.killFlag == 10) {
                    return;
                }
            }
        } catch (InterruptedException e) {
            System.out.println(e);
        }
    }
}

/** Writer thread **/
class A2Writer extends Thread {
    int id; // thread id
    SharedValues sv;
    int d1 = 0;
    int d2 = 0;
    IWMonitor mn; // monitor

    public A2Writer(int _id, SharedValues _sv, IWMonitor _mn) {
        sv = _sv;
        id = _id;
        mn = _mn;
    }
    /** Implements the algorithm as seen in part 1b) of assignment **/
    public synchronized void run() {
        try {
            Thread.sleep((long) (Math.random() * 10));
            d1 = (int)(Math.random() * 100); // random data
            d2 = (int)(Math.random() * 100);
            mn.startWrite(); // lock variables - see monitor
            sv.c++;
            sv.x1 = d1;
            sv.x2 = d2;
            A2Event.writeData(id, d1, d2); // advertise data before incrementing c
            sv.c++;
            mn.endWrite(); // unlock variables

        } catch (InterruptedException e) {
            System.out.println(e);
        }
        sv.killFlag++; // after a write, increment kill flag
    }

}

/** Incrementor Thread **/
class A2Incrementor extends Thread {
    int id;
    SharedValues sv;
    int d1 = 0;
    int d2 = 0;
    int c0 = 0;
    IWMonitor mn;

    public A2Incrementor(int _id, SharedValues _sv, IWMonitor _mn) {
        sv = _sv;
        id = _id;
        mn = _mn;
    }

    /** Implements the algorithm as seen in part 1b) of assignment **/
    public synchronized void run() {
        try {
            Thread.sleep((long) (Math.random() * 10));
            /** Read in current value of x1, x2, ignore if changed **/
            do { // while(cond){...} doesn't work
                do  { // see above
                    c0 = sv.c;
                } while (c0 % 2 != 0);
                d1 = sv.x1;
                d2 = sv.x2;
            } while (c0 != sv.c);
            // Only lock when it is time to write
            mn.startIncrement();
            sv.c++;
            sv.x1 = d1 + 1;
            sv.x2 = d2 + 1;
            // broadcast data
            A2Event.incrementData(id, sv.x1, sv.x2);
            sv.c++;
            mn.endIncrement(); // let other writers || incrementors access shared variables

        } catch (InterruptedException e) {
            System.out.println(e);
        }
        sv.killFlag++; // new write, increment kill flag for reader threads
    }
}

/** Main thread to spawn child threads **/
public class ReadWrite {
    public static void main(String[] args) throws InterruptedException {
        SharedValues svMain = new SharedValues(); // instantiate singleton of shared and monitor
        IWMonitor mnMain = new IWMonitor();

        // Store all threads for graceful joining at the end;
        A2Writer[] writers = new A2Writer[5];
        for (int y = 0; y < 5; y++) {
            A2Writer writer = new A2Writer(y, svMain, mnMain);
            writers[y] = writer;
            writer.start();
        }

        A2Incrementor[] incrementors = new A2Incrementor[5];
        for (int z = 0; z < 5; z++) {
            A2Incrementor incrementor = new A2Incrementor(z, svMain, mnMain);
            incrementors[z] = incrementor;
            incrementor.start();
        }

        A2Reader[] readers = new A2Reader[5];
        for (int x = 0; x < 5; x++) {
            A2Reader reader = new A2Reader(x, svMain);
            readers[x] = reader;
            reader.start();
        }

        for (int i = 0; i < 5; i++) {

            writers[i].join();
            incrementors[i].join();
            readers[i].join();
        }
    }
}
