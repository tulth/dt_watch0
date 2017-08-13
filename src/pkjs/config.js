module.exports = [
  {
    "type": "heading",
    "defaultValue": "App Configuration"
  },
  {
    "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "Colors"
      },
      {
        "type": "color",
        "messageKey": "TimeFontColor",
        "defaultValue": "0x000000",
        "label": "Time font color"
      },
      {
        "type": "color",
        "messageKey": "DateFontColor",
        "defaultValue": "0x000000",
        "label": "Date font color"
      },
      {
        "type": "color",
        "messageKey": "TimeDateBackgroundColor",
        "defaultValue": "0xFFFFFF",
        "label": "Time/Date background color"
      },
      {
        "type": "color",
        "messageKey": "InfoBackgroundColor",
        "defaultValue": "0xFFFFFF",
        "label": "Info section background color"
      },
      {
        "type": "input",
        "messageKey": "DateStrftimeStr",
        "defaultValue": "%a %d%b",
        "label": "Date strftime string"
      }
    ]
  },
  {
    "type": "submit",
    "defaultValue": "Save Settings"
  }
];
