// Simple class to hold workout data
public class WorkoutSession {
    // Fields to store workout information
    int id;
    String workout;
    int duration;
    int heartRate;
    int calories;
    String intensity;
    String month;

    // Constructor - creates a new workout session
    public WorkoutSession(int id, String workout, int duration, int heartRate, 
                         int calories, String intensity, String month) {
        this.id = id;
        this.workout = workout;
        this.duration = duration;
        this.heartRate = heartRate;
        this.calories = calories;
        this.intensity = intensity;
        this.month = month;
    }
}
