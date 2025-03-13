// When the button is clicked, process the selected image

document.getElementById('findClothesBtn').addEventListener('click', function() {

    const photoInput = document.getElementById('photoInput');
  
    if (photoInput.files.length > 0) {
  
      const file = photoInput.files[0];
  
      const reader = new FileReader();
  
      
  
      reader.onload = function(event) {
  
        const imageUrl = event.target.result;
  
        classifyImage(imageUrl);
  
      };
  
      
  
      reader.readAsDataURL(file);
  
    } else {
  
      alert('Please select a photo first!');
  
    }
  
  });
  
  
  
  // Function to classify the image using MobileNet via TensorFlow.js
  
  async function classifyImage(imageUrl) {
  
    // Create an image element
  
    const img = new Image();
  
    img.src = imageUrl;
  
    img.onload = async () => {
  
      // Load the MobileNet model (this is free and runs in the browser)
  
      const model = await mobilenet.load();
  
      // Classify the image
  
      const predictions = await model.classify(img);
  
      displayResults(predictions);
  
    }
  
  }
  
  
  
  // Function to display the classification results
  
  function displayResults(predictions) {
  
    const resultsDiv = document.getElementById('results');
  
    resultsDiv.innerHTML = ''; // Clear previous results
  
    
  
    if (predictions && predictions.length > 0) {
  
      predictions.forEach(prediction => {
  
        const itemDiv = document.createElement('div');
  
        itemDiv.innerHTML = `<p>${prediction.className} - Probability: ${prediction.probability.toFixed(2)}</p>`;
  
        resultsDiv.appendChild(itemDiv);
  
      });
  
    } else {
  
      resultsDiv.innerHTML = '<p>No items detected. Please try again.</p>';
  
    }
  
  }
