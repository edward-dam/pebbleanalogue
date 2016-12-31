// Author: Ed Dam

// Rocky.js

var rocky = require('rocky');

// Draw Text

function drawText(ctx, text, color, align, font, width, height) {
  ctx.fillStyle = color;
  ctx.textAlign = align;
  ctx.font      = font;
  ctx.fillText(text, width, height);
}

// Draw Hand

function fractionToRadian(fraction) {
  return fraction * 2 * Math.PI;
}

function drawHand(ctx, cx, cy, angle, length, color, linewidth) {
  var x2          = cx + Math.sin(angle) * length;
  var y2          = cy - Math.cos(angle) * length;
  ctx.lineWidth   = linewidth;
  ctx.strokeStyle = color;
  ctx.beginPath();
  ctx.moveTo(cx, cy);
  ctx.lineTo(x2, y2);
  ctx.stroke();
}

// Redraw Every Minute

rocky.on('minutechange', function(event) {
  rocky.requestDraw();
});

// Draw Watchface

rocky.on('draw', function(event) {
  var ctx = event.context;

  // Clear Canvas
  ctx.clearRect(0, 0, ctx.canvas.clientWidth, ctx.canvas.clientHeight);

  // Define Screen Size
  var width  = ctx.canvas.unobstructedWidth;
  var height = ctx.canvas.unobstructedHeight;
  
  // Define Centre of Screen
  var cx = width  / 2;
  var cy = height / 2;
  
  // Draw Fixed Text
  drawText(ctx, 'pebble',    'white', 'center', '24px bold Gothic', cx, cy - 57);
  drawText(ctx, 'edwarddam', 'white', 'center', '14px bold Gothic', cx, cy - 32);
  
  // Draw Numbers
  drawText(ctx, '1',  'white', 'center', '20px bold Leco-numbers', cx + 40, 0);
  drawText(ctx, '2',  'white', 'right',  '20px bold Leco-numbers', width,   cy     - 50);
  drawText(ctx, '3',  'white', 'right',  '20px bold Leco-numbers', width,   cy     - 13);
  drawText(ctx, '4',  'white', 'right',  '20px bold Leco-numbers', width,   cy     + 25);
  drawText(ctx, '5',  'white', 'center', '20px bold Leco-numbers', cx + 40, height - 25);
  drawText(ctx, '6',  'white', 'center', '20px bold Leco-numbers', cx,      height - 25);
  drawText(ctx, '7',  'white', 'center', '20px bold Leco-numbers', cx - 40, height - 25);
  drawText(ctx, '8',  'white', 'left',   '20px bold Leco-numbers', 0,       cy     + 25);
  drawText(ctx, '9',  'white', 'left',   '20px bold Leco-numbers', 0,       cy     - 13);
  drawText(ctx, '10', 'white', 'left',   '20px bold Leco-numbers', 0,       cy     - 50);
  drawText(ctx, '11', 'white', 'center', '20px bold Leco-numbers', cx - 40, 0);
  drawText(ctx, '12', 'white', 'center', '20px bold Leco-numbers', cx,      0);
  
  // Draw Day
  var dayNames  = ['Sunday', 'Monday', 'Tuesday', 'Wednesday', 'Thursday', 'Friday', 'Saturday'];
  var dayDate   = dayNames[new Date().getDay()];
  drawText(ctx, dayDate, 'white', 'center', '14px bold Gothic', cx, cy + 15);
  
  // Draw Date
  var dateDate  = new Date().toLocaleDateString(undefined, {day:   '2-digit'});
  var dateMonth = new Date().toLocaleDateString(undefined, {month: 'long'});
  var date      = dateDate + " " + dateMonth;
  drawText(ctx, date, 'white', 'center', '14px bold Gothic', cx, cy + 27);
  
  // Define Max Length of Hands
  var maxLength = (Math.min(width, height) - 20) / 2;

  // Draw Minute Hand
  var minuteFraction = (new Date().getMinutes()) / 60;
  var minuteAngle    = fractionToRadian(minuteFraction);
  drawHand(ctx, cx, cy, minuteAngle, maxLength, 'white', 6);

  // Draw Hour Hand
  var hourFraction = (new Date().getHours() % 12 + minuteFraction) / 12;
  var hourAngle    = fractionToRadian(hourFraction);
  drawHand(ctx, cx, cy, hourAngle, maxLength * 0.6, 'white', 6);
});