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
  
  // Draw Numbers
  drawText(ctx, '1',  'white', 'center', '28px bold Droid-serif', cx + 40, - 6);
  drawText(ctx, '2',  'white', 'right',  '28px bold Droid-serif', width,   cy     - 55);
  drawText(ctx, '3',  'white', 'right',  '28px bold Droid-serif', width,   cy     - 18);
  drawText(ctx, '4',  'white', 'right',  '28px bold Droid-serif', width,   cy     + 20);
  drawText(ctx, '5',  'white', 'center', '28px bold Droid-serif', cx + 40, height - 30);
  drawText(ctx, '6',  'white', 'center', '28px bold Droid-serif', cx,      height - 30);
  drawText(ctx, '7',  'white', 'center', '28px bold Droid-serif', cx - 40, height - 30);
  drawText(ctx, '8',  'white', 'left',   '28px bold Droid-serif', 0,       cy     + 20);
  drawText(ctx, '9',  'white', 'left',   '28px bold Droid-serif', 0,       cy     - 18);
  drawText(ctx, 'T',  'white', 'left',   '28px bold Droid-serif', 0,       cy     - 55);
  drawText(ctx, 'E',  'white', 'center', '28px bold Droid-serif', cx - 40, - 6);
  drawText(ctx, '12', 'white', 'center', '28px bold Droid-serif', cx,      - 6);
  
  // Draw Day
  var day = new Date().toLocaleDateString(undefined, {day: 'short'});
  drawText(ctx, day, 'gray', 'center', '14px bold Gothic', width - 33, cy - 15);

  // Draw Date
  var date = new Date().toLocaleDateString(undefined, {day: 'numeric'});
  drawText(ctx, date, 'gray', 'center', '14px bold Gothic', width - 33, cy - 3);

  // Define Max Length of Hands
  var maxLength = (Math.min(width, height) - 15) / 2;

  // Draw Minute Hand
  var minuteFraction = (new Date().getMinutes()) / 60;
  var minuteAngle    = fractionToRadian(minuteFraction);
  drawHand(ctx, cx, cy, minuteAngle, maxLength, 'white', 7);

  // Draw Hour Hand
  var hourFraction = (new Date().getHours() % 12 + minuteFraction) / 12;
  var hourAngle    = fractionToRadian(hourFraction);
  drawHand(ctx, cx, cy, hourAngle, maxLength * 0.6, 'gray', 7);
  
  // Draw Centre Dot
  drawHand(ctx, cx, cy, 0, 0, 'white', 13);
});