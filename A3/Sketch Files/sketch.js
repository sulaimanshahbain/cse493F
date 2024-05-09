let currentProblem;
let options = [];
let correctAnswer;
let score = 0;
let gameState = 'intro';
let gameStartTime;
let feedbackMessage = '';
let feedbackTimeout;
let correctTallies = [];
let gameDuration = 25000; // 25 seconds for the game

function setup() {
  createCanvas(400, 400);
  textAlign(CENTER, CENTER);
  textSize(24);
}

function draw() {
  if (gameState === 'intro') {
    displayIntroScreen();
  } else if (gameState === 'quiz') {
    displayQuizScreen();
  } else if (gameState === 'end') {
    displayEndScreen();
  }
}

function displayIntroScreen() {
  background(74, 185, 255);
  
  // Add a gradient background
  let c1 = color(74, 185, 255);
  let c2 = color(162, 155, 254);
  for (let y = 0; y < height; y++) {
    let inter = map(y, 0, height, 0, 1);
    let c = lerpColor(c1, c2, inter);
    stroke(c);
    line(0, y, width, y);
  }
  
  // Add a game logo
  textSize(48);
  textFont('Bangers');
  fill(255);
  text("Quick Maths!", width / 2, height / 4);
  
  // Add a subtitle
  textSize(24);
  textFont('Montserrat');
  text("Test your simple math skills!", width / 2, height / 3);
  
  // Draw game rules
  drawRules();
  
  // Add "Press any button to start" text
  textSize(18);
  text("Press any button to start...", width / 2, height - 40);
}

function displayQuizScreen() {
  background(255);
  
  fill(0);
  text(`${currentProblem.question} = ?`, width / 2, height / 3);
  displayOptions();
  displayFeedback();
  displayTallies();
  
  let timeLeft = (gameStartTime + gameDuration - millis()) / 1000;
  if (timeLeft <= 0) {
    gameState = 'end';
    clearTimeout(feedbackTimeout);
    feedbackMessage = ''; // Clear any pending feedback messages
  } else {
    displayTimer(timeLeft);
  }
}

function displayEndScreen() {
  background(74, 185, 255);
  
  fill(255);
  text(`Final Score: ${score}`, width / 2, height / 2);
  text("Press any button to restart...", width / 2, 2 * height / 3);
}

function drawRules() {
  textSize(18);
  fill(255);
  text("Game Rules:", width / 2, height / 2);
  textSize(16);
  text("• 25-second game", width / 2, height / 2 + 30);
  text("• Multi-choice answers", width / 2, height / 2 + 50);
  text("• Integer division", width / 2, height / 2 + 70);
}

function displayOptions() {
  textSize(20);
  for (let i = 0; i < options.length; i++) {
    text(`${String.fromCharCode(65 + i)}. ${options[i]}`, width / 2, height / 2 + 30 * i);
  }
}

function displayFeedback() {
  textSize(16);
  fill(0);
  text(feedbackMessage, width / 2, height - 30);
  textSize(24);
}

function displayTallies() {
  let xStart = 20;
  fill(0);
  for (let i = 0; i < score; i++) {
    text('|', xStart + i * 10, 30); // Draw tally marks
  }
}

function displayTimer(timeLeft) {
  textSize(16);
  fill(0);
  text(`Time Left: ${timeLeft.toFixed(1)}s`, width / 2, 50);
  textSize(24);
}

function keyPressed() {
  if (gameState === 'intro') {
    startGame();
  } else if (gameState === 'quiz' && (key === 'A' || key === 'B' || key === 'C' || key === 'D')) {
    checkAnswer(key);
  } else if (gameState === 'end') {
    resetGame();
  }
}

function startGame() {
  gameState = 'buffer';
  gameStartTime = millis();
  setTimeout(() => {
    nextProblem();
    gameState = 'quiz';
  }, 500); // Wait for .5 second before showing the first problem
}

function nextProblem() {
  clearTimeout(feedbackTimeout);
  feedbackMessage = '';
  let num1 = int(random(1, 20));
  let num2 = int(random(1, 20));
  let operation = random(['+', '-', '*', '/']);
  let result = int(eval(`${num1} ${operation} ${num2}`)); // Ensure integer results
  currentProblem = {question: `${num1} ${operation} ${num2}`, answer: result};
  generateOptions(result);
}

function generateOptions(correct) {
  let answerSet = new Set();
  answerSet.add(correct);
  while (answerSet.size < 4) {
    let offset = int(random(-10, 10));
    answerSet.add(correct + offset);
  }
  options = Array.from(answerSet);
  shuffle(options, true);
  correctAnswer = String.fromCharCode(65 + options.indexOf(correct));
}

function checkAnswer(selected) {
  if (selected === correctAnswer) {
    score++;
    feedbackMessage = "Correct!";
    correctTallies.push(score); // Add to the tally
  } else {
    feedbackMessage = "Incorrect!";
  }
  feedbackTimeout = setTimeout(nextProblem, 1500); // Show feedback for 1.5 seconds before next problem
}

function resetGame() {
  score = 0;
  correctTallies = [];
  feedbackMessage = '';
  gameState = 'intro';
}
