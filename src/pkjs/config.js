//Author: Ed Dam

module.exports = [
  { "type": "heading", "defaultValue": "Analogue v2.0" },
  { "type": "text", "defaultValue": "by Edward Dam" },
  { "type": "section", "items": [
    { "type": "heading", "defaultValue": "Date" },
    { "type": "text", "defaultValue": "Please Enable or Disable" },
    { "type": "radiogroup", "messageKey": "DATE", "options": [
      { "label": "Disable", "value": "false" },
      { "label": "Enable", "value": "true" } ],
    "defaultValue": "false" } ]
  },
  { "type": "section", "capabilities": ["HEALTH"], "items": [
    { "type": "heading", "defaultValue": "Health" },
    { "type": "text", "defaultValue": "Steps & Sleep" },
    { "type": "radiogroup", "messageKey": "HEALTH", "options": [
      { "label": "Disable", "value": "false" },
      { "label": "Enable", "value": "true" } ],
    "defaultValue": "false" } ]
  },
  { "type": "text", "defaultValue": "Thank you for using my watchface." },
  { "type": "submit", "defaultValue": "Save Settings" }
];
