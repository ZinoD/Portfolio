import javax.swing.*;
import java.awt.*;
import java.io.*;
import java.util.ArrayList;

// Main application window
public class FitnessExplorer extends JFrame {
    
    // Store all workout sessions in a list
    ArrayList<WorkoutSession> sessions = new ArrayList<>();
    
    // Text areas to show data and stats
    JTextArea displayArea;
    JTextArea statsArea;
    
    // Set up the window when program starts
    public FitnessExplorer() {
        setTitle("Fitness Data Explorer");
        setSize(900, 600);
        setDefaultCloseOperation(EXIT_ON_CLOSE);
        setLayout(new BorderLayout());
        
        // Make the buttons at the top
        JPanel topPanel = new JPanel();
        JButton loadBtn = new JButton("Load File");
        JButton addBtn = new JButton("Add Row");
        JButton searchBtn = new JButton("Search");
        
        // Connect buttons to methods
        loadBtn.addActionListener(e -> loadFile());
        addBtn.addActionListener(e -> addRow());
        searchBtn.addActionListener(e -> search());
        
        // Add buttons to top
        topPanel.add(loadBtn);
        topPanel.add(addBtn);
        topPanel.add(searchBtn);
        add(topPanel, BorderLayout.NORTH);
        
        // Make the main data display area
        displayArea = new JTextArea();
        displayArea.setFont(new Font("Monospaced", Font.PLAIN, 11));
        add(new JScrollPane(displayArea), BorderLayout.CENTER);
        
        // Make the statistics area on the right
        statsArea = new JTextArea(20, 25);
        statsArea.setFont(new Font("Monospaced", Font.PLAIN, 12));
        statsArea.setBackground(Color.LIGHT_GRAY);
        add(new JScrollPane(statsArea), BorderLayout.EAST);
        
        setLocationRelativeTo(null);
    }
    
    // Load workout data from a file
    void loadFile() {
        // Open file picker
        JFileChooser chooser = new JFileChooser(".");
        
        if (chooser.showOpenDialog(this) == JFileChooser.APPROVE_OPTION) {
            try {
                // Open the file to read it
                BufferedReader reader = new BufferedReader(
                    new FileReader(chooser.getSelectedFile()));
                
                // Clear old data
                sessions.clear();
                
                // Skip the first line (it's just headers)
                reader.readLine();
                
                // Read each line one by one
                String line;
                while ((line = reader.readLine()) != null) {
                    // Split line by spaces
                    String[] parts = line.split("\\s+");
                    
                    // Make sure we have enough data
                    if (parts.length >= 7) {
                        // Create a workout object and add to list
                        sessions.add(new WorkoutSession(
                            Integer.parseInt(parts[0]),  // ID
                            parts[1],                     // Workout type
                            Integer.parseInt(parts[2]),   // Duration
                            Integer.parseInt(parts[3]),   // Heart rate
                            Integer.parseInt(parts[4]),   // Calories
                            parts[5],                     // Intensity
                            parts[6]                      // Month
                        ));
                    }
                }
                
                // Close the file
                reader.close();
                
                // Show the data and stats
                showData();
                showStats();
                
                // Tell user how many rows loaded
                JOptionPane.showMessageDialog(this, 
                    "Loaded " + sessions.size() + " rows");
                
            } catch (Exception ex) {
                // Show error if something goes wrong
                JOptionPane.showMessageDialog(this, "Error: " + ex.getMessage());
            }
        }
    }
    
    // Display all workout data in the text area
    void showData() {
        StringBuilder sb = new StringBuilder();
        
        // Add header row
        sb.append("ID      Workout  Duration  Heart  Calories  Intensity  Month\n");
        sb.append("------------------------------------------------------------\n");
        
        // Add each workout to display
        for (WorkoutSession s : sessions) {
            sb.append(String.format("%-8d%-9s%-10d%-7d%-10d%-11s%s\n",
                s.id, s.workout, s.duration, s.heartRate, 
                s.calories, s.intensity, s.month));
        }
        
        // Put text in the display area
        displayArea.setText(sb.toString());
    }
    
    // Calculate and show statistics
    void showStats() {
        // Check if we have data
        if (sessions.isEmpty()) {
            statsArea.setText("No data loaded");
            return;
        }
        
        // Count total rows
        int totalRows = sessions.size();
        
        // Add up all durations, heart rates, and calories
        int totalDuration = 0;
        int totalHeartRate = 0;
        int totalCalories = 0;
        
        for (WorkoutSession s : sessions) {
            totalDuration += s.duration;
            totalHeartRate += s.heartRate;
            totalCalories += s.calories;
        }
        
        // Calculate averages
        double avgDuration = (double)totalDuration / totalRows;
        double avgHeartRate = (double)totalHeartRate / totalRows;
        
        // Find most common workout type
        String mostCommon = findMostCommon();
        
        // Build the statistics text
        StringBuilder sb = new StringBuilder();
        sb.append("STATISTICS\n");
        sb.append("================\n\n");
        sb.append("Total Rows: " + totalRows + "\n\n");
        sb.append("Avg Duration: " + String.format("%.1f", avgDuration) + " min\n\n");
        sb.append("Avg Heart Rate: " + String.format("%.1f", avgHeartRate) + " bpm\n\n");
        sb.append("Total Calories: " + totalCalories + "\n\n");
        sb.append("Most Common:\n" + mostCommon);
        
        // Show statistics
        statsArea.setText(sb.toString());
    }
    
    // Find which workout type appears most often
    String findMostCommon() {
        if (sessions.isEmpty()) return "None";
        
        // Arrays to store workout types and their counts
        String[] workouts = new String[sessions.size()];
        int[] counts = new int[sessions.size()];
        int uniqueCount = 0;
        
        // Count how many times each workout appears
        for (WorkoutSession s : sessions) {
            boolean found = false;
            
            // Check if we've seen this workout before
            for (int i = 0; i < uniqueCount; i++) {
                if (workouts[i].equals(s.workout)) {
                    counts[i]++;  // Add to count
                    found = true;
                    break;
                }
            }
            
            // If it's a new workout type, add it to the list
            if (!found) {
                workouts[uniqueCount] = s.workout;
                counts[uniqueCount] = 1;
                uniqueCount++;
            }
        }
        
        // Find which workout has highest count
        int maxIndex = 0;
        for (int i = 1; i < uniqueCount; i++) {
            if (counts[i] > counts[maxIndex]) {
                maxIndex = i;
            }
        }
        
        // Return workout name with count
        return workouts[maxIndex] + " (" + counts[maxIndex] + "x)";
    }
    
    // Add a new workout row
    void addRow() {
        // Make sure data is loaded first
        if (sessions.isEmpty()) {
            JOptionPane.showMessageDialog(this, "Load data first!");
            return;
        }
        
        // Create input form
        JPanel panel = new JPanel(new GridLayout(7, 2, 5, 5));
        
        // Create text fields for each piece of data
        JTextField workoutField = new JTextField();
        JTextField durationField = new JTextField();
        JTextField heartRateField = new JTextField();
        JTextField caloriesField = new JTextField();
        JTextField intensityField = new JTextField();
        JTextField monthField = new JTextField();
        
        // Add labels and text fields to form
        panel.add(new JLabel("Workout:"));
        panel.add(workoutField);
        panel.add(new JLabel("Duration (min):"));
        panel.add(durationField);
        panel.add(new JLabel("Heart Rate:"));
        panel.add(heartRateField);
        panel.add(new JLabel("Calories:"));
        panel.add(caloriesField);
        panel.add(new JLabel("Intensity:"));
        panel.add(intensityField);
        panel.add(new JLabel("Month:"));
        panel.add(monthField);
        
        // Show the form and wait for OK or Cancel
        int result = JOptionPane.showConfirmDialog(this, panel, 
            "Add New Workout", JOptionPane.OK_CANCEL_OPTION);
        
        // If user clicked OK, add the row
        if (result == JOptionPane.OK_OPTION) {
            try {
                // Get values from text fields
                String workout = workoutField.getText().trim();
                int duration = Integer.parseInt(durationField.getText().trim());
                int heartRate = Integer.parseInt(heartRateField.getText().trim());
                int calories = Integer.parseInt(caloriesField.getText().trim());
                String intensity = intensityField.getText().trim();
                String month = monthField.getText().trim();
                
                // Check if any field is empty
                if (workout.isEmpty() || intensity.isEmpty() || month.isEmpty()) {
                    JOptionPane.showMessageDialog(this, "All fields required!");
                    return;
                }
                
                // Find highest ID and add 1 for new ID
                int maxId = sessions.get(0).id;
                for (WorkoutSession s : sessions) {
                    if (s.id > maxId) maxId = s.id;
                }
                
                // Create new workout and add to list
                sessions.add(new WorkoutSession(maxId + 1, workout, duration, 
                    heartRate, calories, intensity, month));
                
                // Update display and statistics
                showData();
                showStats();
                
                // Tell user it worked
                JOptionPane.showMessageDialog(this, "Row added!");
                
            } catch (Exception ex) {
                // Show error if user entered bad data
                JOptionPane.showMessageDialog(this, "Invalid input!");
            }
        }
    }
    
    // Search for a term in the data
    void search() {
        // Ask user what to search for
        String term = JOptionPane.showInputDialog(this, "Enter search term:");
        
        // If they cancel or enter nothing, stop
        if (term == null || term.trim().isEmpty()) {
            return;
        }
        
        // Make search case-insensitive
        term = term.toLowerCase();
        int count = 0;
        
        // Look through every workout
        for (WorkoutSession s : sessions) {
            // Check if term appears in any field
            if (s.workout.toLowerCase().contains(term)) count++;
            if (s.intensity.toLowerCase().contains(term)) count++;
            if (s.month.toLowerCase().contains(term)) count++;
            if (String.valueOf(s.id).contains(term)) count++;
            if (String.valueOf(s.duration).contains(term)) count++;
            if (String.valueOf(s.heartRate).contains(term)) count++;
            if (String.valueOf(s.calories).contains(term)) count++;
        }
        
        // Show how many times we found it
        JOptionPane.showMessageDialog(this, 
            "Found " + count + " occurrences of '" + term + "'");
    }
    
    // Start the program here
    public static void main(String[] args) {
        new FitnessExplorer().setVisible(true);
    }
}
