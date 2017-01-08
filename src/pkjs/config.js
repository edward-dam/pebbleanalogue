//Author: Ed Dam

module.exports = [
  { "type": "heading", "defaultValue": "Analogue v1.5" },
  { "type": "text", "defaultValue": "by Edward Dam" },
  { "type": "section", "items": [
    { "type": "heading", "defaultValue": "Seconds Hand" },
    { "type": "text", "defaultValue": "Please Disable or Enable Seconds" },
    { "type": "radiogroup", "messageKey": "interval", "options": [
      { "label": "Disable", "value": "minutechange" },
      { "label": "Enable",  "value": "secondchange" } ],
    "defaultValue": "minutechange" } ]
  },
  { "type": "text", "defaultValue": "Thank you for using Analogue v1.5 by Edward Dam" },
  { "type": "submit", "defaultValue": "Save Settings" }
];